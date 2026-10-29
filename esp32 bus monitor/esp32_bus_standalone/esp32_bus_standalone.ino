/*
 * ESP32 — STANDALONE live Groningen bus counter on an SPI OLED.
 * No proxy: the ESP32 fetches the OVapi GTFS-RT VehiclePositions feed over
 * HTTPS and decodes the protobuf on-device with a minimal streaming scanner.
 *
 * Libraries (Library Manager):
 *   - U8g2         (oliver kraus)
 * (WiFiClientSecure is part of the ESP32 core; no protobuf lib needed.)
 *
 * Wiring (HW I2C):
 *   classic ESP32: SDA->GPIO21  SCL->GPIO22
 *   ESP32-S2 Mini: SDA->GPIO33  SCL->GPIO35   (set below)
 *   VCC->3V3  GND->GND
 *
 * How it works: the feed is ~487 KB, far too big for RAM. We stream the TLS
 * body and walk the protobuf wire format with recursive descent, descending
 * only into entity->vehicle->position (lat/lon, fixed32) and vehicle.timestamp.
 * Everything else is skipped by length. Nothing is fully buffered.
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <U8g2lib.h>
#include <Wire.h>

// ---- config ---------------------------------------------------------------
const char* WIFI_SSID = "WOMANETOMA";
const char* WIFI_PASS = "koekenbakkerknol";

const char* HOST = "gtfs.ovapi.nl";
const uint16_t PORT = 443;
const char* PATH = "/nl/vehiclePositions.pb";

const uint32_t UPDATE_MS = 60000;    // feed refreshes ~every 60 s
const int32_t  MAX_AGE_S = 120;      // drop vehicles older than this vs feed time

// Groningen urban bounding box (WGS84)
const float LAT_MIN = 53.180f, LAT_MAX = 53.260f;
const float LON_MIN = 6.500f,  LON_MAX = 6.620f;

#define OLED_SDA 33   // ESP32-S2 Mini default SDA (use 21 on a classic ESP32)
#define OLED_SCL 35   // ESP32-S2 Mini default SCL (use 22 on a classic ESP32)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// If your panel is not SSD1306 128x64, swap only this constructor.
// Default I2C address is 0x3C; for a 0x3D module add: u8g2.setI2CAddress(0x3D << 1);

// ---- streaming byte source (TLS body, chunked-aware) ----------------------
WiFiClientSecure net;
uint8_t  rbuf[1024];
int      rpos = 0, rlen = 0;
bool     srcEof = false;
long     contentLength = -1;   // -1 = unknown (read until close)
long     bodyRead = 0;
bool     chunked = false;
long     chunkRemaining = -1;  // -1 = identity, else bytes left in current chunk

static String readLine() {                 // header lines, '\n'-terminated
  String s;
  uint32_t t0 = millis();
  while (millis() - t0 < 8000) {
    int c = net.read();
    if (c < 0) { if (!net.connected()) break; delay(1); continue; }
    if (c == '\n') break;
    if (c != '\r') s += (char)c;
  }
  return s;
}

// pull up to max raw body bytes, honouring chunked transfer-encoding
static int bodyRaw(uint8_t* buf, int max) {
  if (srcEof) return -1;

  if (chunked) {
    if (chunkRemaining <= 0) {
      String sz = readLine();              // hex chunk size [;ext]
      long n = strtol(sz.c_str(), nullptr, 16);
      if (n <= 0) { srcEof = true; return -1; }
      chunkRemaining = n;
    }
    int want = (max < chunkRemaining) ? max : (int)chunkRemaining;
    int got = net.read(buf, want);
    if (got <= 0) { if (!net.connected()) { srcEof = true; return -1; } delay(1); return 0; }
    chunkRemaining -= got;
    if (chunkRemaining == 0) { readLine(); } // consume trailing CRLF
    return got;
  }

  // identity
  if (contentLength >= 0 && bodyRead >= contentLength) { srcEof = true; return -1; }
  int got = net.read(buf, max);
  if (got <= 0) { if (!net.connected()) { srcEof = true; return -1; } delay(1); return 0; }
  bodyRead += got;
  return got;
}

static bool refill() {
  while (true) {
    int n = bodyRaw(rbuf, sizeof(rbuf));
    if (n > 0) { rlen = n; rpos = 0; return true; }
    if (n < 0) return false;               // eof
    yield();                               // n==0: wait for more, keep WDT happy
  }
}

static int rdByte() {                      // 0..255, or -1 at eof
  if (rpos >= rlen) { if (!refill()) return -1; }
  return rbuf[rpos++];
}

// ---- protobuf primitives --------------------------------------------------
static uint64_t rdVarint(uint32_t& used, bool& trunc) {
  uint64_t v = 0; int shift = 0; used = 0;
  while (shift < 64) {
    int b = rdByte();
    if (b < 0) { trunc = true; return v; }
    used++;
    v |= (uint64_t)(b & 0x7F) << shift;
    if (!(b & 0x80)) return v;
    shift += 7;
  }
  trunc = true; return v;
}

static uint32_t rdFixed32(bool& trunc) {
  uint32_t v = 0;
  for (int i = 0; i < 4; i++) {
    int b = rdByte();
    if (b < 0) { trunc = true; return v; }
    v |= (uint32_t)b << (8 * i);
  }
  return v;
}

static void skipBytes(long n, bool& trunc) {
  while (n-- > 0) { if (rdByte() < 0) { trunc = true; return; } }
}

static float bitsToFloat(uint32_t bits) { float f; memcpy(&f, &bits, 4); return f; }

// ---- decode state ---------------------------------------------------------
int      gCount = 0;
uint64_t feedTs = 0;

struct VInfo { bool hasLat, hasLon, hasTs; float lat, lon; uint64_t ts; };

// generic: consume exactly 'len' bytes of a message, dispatching by field
// 'mode': 0 = FeedHeader, 1 = FeedEntity, 2 = VehiclePosition, 3 = Position
static void parseMsg(long len, int mode, VInfo* vi, bool& trunc);

static void parseMsg(long len, int mode, VInfo* vi, bool& trunc) {
  while (len > 0 && !trunc) {
    uint32_t u;
    uint64_t tag = rdVarint(u, trunc); len -= u;
    if (trunc) return;
    int field = tag >> 3;
    int wt = tag & 7;

    if (wt == 0) {                         // varint
      uint64_t val = rdVarint(u, trunc); len -= u;
      if (mode == 0 && field == 3) feedTs = val;          // FeedHeader.timestamp
      else if (mode == 2 && field == 5 && vi) { vi->ts = val; vi->hasTs = true; } // VP.timestamp
    } else if (wt == 5) {                  // fixed32
      uint32_t bits = rdFixed32(trunc); len -= 4;
      if (mode == 3 && field == 1 && vi) { vi->lat = bitsToFloat(bits); vi->hasLat = true; }
      else if (mode == 3 && field == 2 && vi) { vi->lon = bitsToFloat(bits); vi->hasLon = true; }
    } else if (wt == 1) {                   // fixed64
      skipBytes(8, trunc); len -= 8;
    } else if (wt == 2) {                   // length-delimited
      uint64_t sub = rdVarint(u, trunc); len -= u; len -= sub;
      if (trunc) return;
      if (mode == 1 && field == 4) {        // FeedEntity.vehicle -> VehiclePosition
        parseMsg(sub, 2, vi, trunc);
      } else if (mode == 2 && field == 2) { // VehiclePosition.position -> Position
        parseMsg(sub, 3, vi, trunc);
      } else {
        skipBytes(sub, trunc);
      }
    } else {                                // unknown wiretype -> bail
      trunc = true; return;
    }
  }
}

// parse one FeedEntity and, if it qualifies, increment the counter
static void parseEntity(long len, bool& trunc) {
  VInfo vi = {false, false, false, 0, 0, 0};
  parseMsg(len, 1, &vi, trunc);
  if (!vi.hasLat || !vi.hasLon) return;
  if (vi.lat < LAT_MIN || vi.lat > LAT_MAX || vi.lon < LON_MIN || vi.lon > LON_MAX) return;
  if (feedTs && vi.hasTs && (long)(feedTs - vi.ts) > MAX_AGE_S) return;
  gCount++;
}

// top-level FeedMessage: no length prefix, read until eof
static bool parseFeed() {
  gCount = 0; feedTs = 0;
  bool trunc = false;
  while (!trunc) {
    uint32_t u;
    uint64_t tag = rdVarint(u, trunc);
    if (trunc) break;                       // eof reached cleanly
    int field = tag >> 3;
    int wt = tag & 7;
    if (wt != 2) {                          // header/entity are length-delimited;
      // skip any stray scalar at top level
      if (wt == 0) { rdVarint(u, trunc); }
      else if (wt == 5) { rdFixed32(trunc); }
      else if (wt == 1) { skipBytes(8, trunc); }
      else { trunc = true; }
      continue;
    }
    uint64_t sub = rdVarint(u, trunc);
    if (trunc) break;
    if (field == 1) parseMsg(sub, 0, nullptr, trunc);   // FeedHeader
    else if (field == 2) parseEntity(sub, trunc);       // FeedEntity
    else skipBytes(sub, trunc);
  }
  return true;
}

// ---- HTTPS fetch ----------------------------------------------------------
static bool fetchAndCount(int& outCount) {
  net.setInsecure();                        // skip cert validation (hobby use)
  net.setTimeout(8000);
  if (!net.connect(HOST, PORT)) return false;

  net.print(String("GET ") + PATH + " HTTP/1.1\r\n");
  net.print(String("Host: ") + HOST + "\r\n");
  net.print("Accept-Encoding: identity\r\n");
  net.print("Connection: close\r\n\r\n");

  // status line
  String status = readLine();
  if (status.indexOf(" 200") < 0) { net.stop(); return false; }

  // headers
  contentLength = -1; chunked = false; bodyRead = 0; chunkRemaining = -1;
  while (true) {
    String h = readLine();
    if (h.length() == 0) break;
    String l = h; l.toLowerCase();
    if (l.startsWith("content-length:")) contentLength = h.substring(15).toInt();
    else if (l.startsWith("transfer-encoding:") && l.indexOf("chunked") >= 0) chunked = true;
  }

  // reset stream buffers and parse
  rpos = rlen = 0; srcEof = false;
  parseFeed();
  net.stop();

  outCount = gCount;
  return true;
}

// ---- display + wifi -------------------------------------------------------
int  lastCount = -1;
bool lastOk = false;

void drawScreen(int count, bool ok) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  const char* label = "Bussen Groningen";
  u8g2.drawStr((128 - u8g2.getStrWidth(label)) / 2, 10, label);
  char buf[8];
  if (ok) snprintf(buf, sizeof(buf), "%d", count); else snprintf(buf, sizeof(buf), "--");
  u8g2.setFont(u8g2_font_logisoso32_tn);
  u8g2.drawStr((128 - u8g2.getStrWidth(buf)) / 2, 56, buf);
  u8g2.sendBuffer();
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) delay(250);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  u8g2.begin();
  drawScreen(0, false);
  connectWiFi();
}

void loop() {
  static uint32_t lastPoll = 0;
  if (lastCount < 0 || millis() - lastPoll >= UPDATE_MS) {
    lastPoll = millis();
    if (WiFi.status() != WL_CONNECTED) connectWiFi();
    int c;
    lastOk = fetchAndCount(c);
    if (lastOk) lastCount = c;
    drawScreen(lastCount, lastOk);
    Serial.printf("count=%d ok=%d heap=%u\n", lastCount, lastOk, ESP.getFreeHeap());
  }
  delay(50);
}
