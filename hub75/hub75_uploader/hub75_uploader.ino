/*
 * HUB75 web image/GIF uploader — ESP32-S2 (also runs on ESP32 / ESP32-S3)
 *
 *
 * Libraries (Arduino Library Manager / platformio lib_deps):
 *   - ESP32 HUB75 LED MATRIX PANEL DMA Display (mrfaptastic)
 *   - AnimatedGIF (bitbank2)
 *   - WiFiManager (tzapu, >= 2.0.16 for S2/S3)
 *   - ESPAsyncWebServer + AsyncTCP (ESP32Async forks)
 *   - ElegantOTA (ayushsharma82) — set ELEGANTOTA_USE_ASYNC_WEBSERVER 1
 *     in ElegantOTA.h, or add build flag -DELEGANTOTA_USE_ASYNC_WEBSERVER=1
 *
 * Design:
 *   - Static images are scaled IN THE BROWSER (canvas, aspect-fit,
 *     letterboxed) and uploaded as raw RGB565-LE, exactly W*H*2 bytes.
 *     No image decoder needed on the ESP.
 *   - GIFs are uploaded as-is to LittleFS and scaled at decode time with
 *     a nearest-neighbour row/column map in the GIFDraw callback, so no
 *     full-frame buffer is needed (works without PSRAM).
 *   - OTA firmware updates via /update (ElegantOTA).
 */

#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <AnimatedGIF.h>

// ---------- Panel configuration ----------
#define PANEL_W   64
#define PANEL_H   32          // 64x64 panels: set 64 and wire PIN_E
#define PANEL_CHAIN 1

// Example pin map for a Wemos S2 mini. Any output-capable GPIOs work,
// the library bit-bangs the mapping into the LCD peripheral via GPIO matrix.
#define PIN_R1   1
#define PIN_G1   2
#define PIN_B1   3
#define PIN_R2   4
#define PIN_G2   5
#define PIN_B2   6
#define PIN_A    7
#define PIN_B    8
#define PIN_C    9
#define PIN_D    10
#define PIN_E    -1           // only needed for 1/32 scan (64px high) panels
#define PIN_LAT  11
#define PIN_OE   12
#define PIN_CLK  13

// ---------- Files / state ----------
static const char *IMG_PATH = "/img.565";
static const char *GIF_PATH = "/anim.gif";

MatrixPanel_I2S_DMA *dma = nullptr;
AnimatedGIF *gif = nullptr;
Preferences prefs;
AsyncWebServer server(80);

enum DisplayMode : uint8_t { MODE_NONE = 0, MODE_IMAGE = 1, MODE_GIF = 2 };
volatile uint8_t displayMode = MODE_NONE;
volatile bool uploadBusy = false;   // pause GIF playback while LittleFS is written
bool gifOpen = false;
uint32_t nextFrameAt = 0;

// nearest-neighbour scaling maps, computed once per GIF
int16_t rowMap[PANEL_H];
int16_t colMap[PANEL_W];
int drawW, drawH, offX, offY;

// ---------------------------------------------------------------
// GIF file callbacks (LittleFS)
// ---------------------------------------------------------------
static File gifFile;

void *GIFOpen(const char *name, int32_t *pSize) {
  gifFile = LittleFS.open(name, "r");
  if (!gifFile) return nullptr;
  *pSize = gifFile.size();
  return (void *)&gifFile;
}
void GIFClose(void *h) { ((File *)h)->close(); }
int32_t GIFRead(GIFFILE *pf, uint8_t *buf, int32_t len) {
  File *f = (File *)pf->fHandle;
  int32_t n = f->read(buf, len);
  pf->iPos = f->position();
  return n;
}
int32_t GIFSeek(GIFFILE *pf, int32_t pos) {
  File *f = (File *)pf->fHandle;
  f->seek(pos);
  pf->iPos = f->position();
  return pf->iPos;
}

// ---------------------------------------------------------------
// GIF draw callback with integrated nearest-neighbour scaling.
// Called once per decoded source scanline; we paint every destination
// row that maps back to this source row (handles up- and downscaling).
// Transparent pixels are skipped => previous frame content persists,
// which approximates disposal method 1 (fine for the vast majority
// of GIFs; disposal 2/3 are not fully honoured).
// ---------------------------------------------------------------
void GIFDraw(GIFDRAW *pDraw) {
  const int srcY = pDraw->iY + pDraw->y;           // canvas coordinates
  const uint8_t  *px  = pDraw->pPixels;            // 8-bit palette indices
  const uint16_t *pal = pDraw->pPalette;           // RGB565

  for (int dy = 0; dy < drawH; dy++) {
    if (rowMap[dy] != srcY) continue;
    const int y = offY + dy;
    for (int dx = 0; dx < drawW; dx++) {
      const int sx = colMap[dx] - pDraw->iX;       // into this frame's line
      if (sx < 0 || sx >= pDraw->iWidth) continue;
      const uint8_t idx = px[sx];
      if (pDraw->ucHasTransparency && idx == pDraw->ucTransparent) continue;
      dma->drawPixel(offX + dx, y, pal[idx]);
    }
  }
}

void buildScaleMaps(int cw, int ch) {
  float sc = fminf((float)PANEL_W / cw, (float)PANEL_H / ch);
  drawW = constrain((int)roundf(cw * sc), 1, PANEL_W);
  drawH = constrain((int)roundf(ch * sc), 1, PANEL_H);
  offX = (PANEL_W - drawW) / 2;
  offY = (PANEL_H - drawH) / 2;
  for (int dx = 0; dx < drawW; dx++)
    colMap[dx] = min(cw - 1, (int)((dx + 0.5f) / sc));
  for (int dy = 0; dy < drawH; dy++)
    rowMap[dy] = min(ch - 1, (int)((dy + 0.5f) / sc));
}

bool startGif() {
  if (gifOpen) { gif->close(); gifOpen = false; }
  if (!LittleFS.exists(GIF_PATH)) return false;
  if (!gif->open(GIF_PATH, GIFOpen, GIFClose, GIFRead, GIFSeek, GIFDraw))
    return false;
  buildScaleMaps(gif->getCanvasWidth(), gif->getCanvasHeight());
  dma->clearScreen();
  gifOpen = true;
  nextFrameAt = millis();
  return true;
}

// ---------------------------------------------------------------
// Static image: raw RGB565 little-endian, exactly PANEL_W*PANEL_H*2 bytes.
// The browser already did the scaling + letterboxing.
// ---------------------------------------------------------------
bool showImage() {
  File f = LittleFS.open(IMG_PATH, "r");
  if (!f || f.size() != (size_t)PANEL_W * PANEL_H * 2) { if (f) f.close(); return false; }
  static uint8_t line[PANEL_W * 2];
  for (int y = 0; y < PANEL_H; y++) {
    f.read(line, sizeof(line));
    for (int x = 0; x < PANEL_W; x++) {
      uint16_t c = line[2 * x] | (line[2 * x + 1] << 8);
      dma->drawPixel(x, y, c);
    }
  }
  f.close();
  return true;
}

void setMode(uint8_t m) {
  displayMode = m;
  prefs.putUChar("mode", m);
}

// ---------------------------------------------------------------
// Web UI (served from PROGMEM; panel size fetched from /info)
// ---------------------------------------------------------------
static const char INDEX_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>HUB75 panel</title>
<style>
:root{--bg:#101418;--card:#1a2027;--fg:#dfe6ec;--dim:#7d8a96;--acc:#ffb547;--ok:#5dd39e;--err:#ff6b6b}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--fg);font:15px/1.5 system-ui,sans-serif;
 display:flex;justify-content:center;padding:24px 12px}
main{width:100%;max-width:460px}
h1{font-size:18px;font-weight:600;letter-spacing:.04em;margin:0 0 2px}
h1 span{color:var(--acc)}
p.sub{margin:0 0 18px;color:var(--dim);font-size:13px}
.card{background:var(--card);border-radius:10px;padding:16px;margin-bottom:14px}
#drop{border:2px dashed #34404c;border-radius:10px;padding:26px 14px;text-align:center;
 color:var(--dim);cursor:pointer;transition:border-color .15s}
#drop.hover{border-color:var(--acc);color:var(--fg)}
#drop input{display:none}
canvas,img.gifprev{display:block;margin:14px auto 0;image-rendering:pixelated;
 width:100%;max-width:384px;background:#000;border-radius:4px}
#status{font-size:13px;margin-top:10px;min-height:1.2em;color:var(--dim)}
#status.ok{color:var(--ok)} #status.err{color:var(--err)}
label{font-size:13px;color:var(--dim)}
input[type=range]{width:100%;accent-color:var(--acc)}
footer{display:flex;justify-content:space-between;font-size:12px;color:var(--dim)}
footer a{color:var(--dim)}
</style></head><body><main>
<h1>HUB75 <span id="res">…</span></h1>
<p class="sub">Drop an image or animated GIF — it is scaled to fit the panel.</p>
<div class="card">
 <div id="drop">Drop file here or tap to choose
  <input type="file" id="file" accept="image/*"></div>
 <canvas id="prev" hidden></canvas>
 <img class="gifprev" id="gifprev" hidden alt="GIF preview">
 <div id="status"></div>
</div>
<div class="card">
 <label>Brightness <span id="bval"></span></label>
 <input type="range" id="bri" min="10" max="255" step="5">
</div>
<footer><a href="/update">firmware update</a><a href="/wifireset">reset WiFi</a></footer>
<script>
let W=64,H=32;
const $=id=>document.getElementById(id),st=$('status');
const say=(t,c)=>{st.textContent=t;st.className=c||''};
fetch('/info').then(r=>r.json()).then(i=>{W=i.w;H=i.h;
 $('res').textContent=W+'\u00d7'+H;
 $('prev').width=W;$('prev').height=H;
 $('bri').value=i.brightness;$('bval').textContent=i.brightness;});
$('bri').oninput=e=>{$('bval').textContent=e.target.value;
 fetch('/brightness?v='+e.target.value);};
const drop=$('drop');
drop.onclick=()=>$('file').click();
$('file').onchange=e=>handle(e.target.files[0]);
['dragover','dragleave','drop'].forEach(ev=>drop.addEventListener(ev,e=>{
 e.preventDefault();drop.classList.toggle('hover',ev==='dragover');
 if(ev==='drop')handle(e.dataTransfer.files[0]);}));
async function handle(f){
 if(!f)return;
 try{
  if(f.type==='image/gif'){await sendGif(f);}else{await sendImage(f);}
 }catch(err){say('Upload failed: '+err.message,'err');}
}
async function sendGif(f){
 $('prev').hidden=true;const g=$('gifprev');
 g.src=URL.createObjectURL(f);g.hidden=false;
 say('Uploading GIF ('+(f.size/1024).toFixed(0)+' kB)…');
 const r=await fetch('/upload/gif',{method:'POST',body:f});
 if(!r.ok)throw new Error(await r.text());
 say('GIF playing on panel','ok');
}
async function sendImage(f){
 const bmp=await createImageBitmap(f);
 const c=$('prev'),x=c.getContext('2d');
 $('gifprev').hidden=true;c.hidden=false;
 x.fillStyle='#000';x.fillRect(0,0,W,H);
 const s=Math.min(W/bmp.width,H/bmp.height);
 const dw=Math.max(1,Math.round(bmp.width*s)),dh=Math.max(1,Math.round(bmp.height*s));
 x.imageSmoothingQuality='high';
 x.drawImage(bmp,(W-dw)/2,(H-dh)/2,dw,dh);
 const d=x.getImageData(0,0,W,H).data,buf=new Uint8Array(W*H*2);
 for(let i=0,j=0;i<d.length;i+=4){
  const v=((d[i]&0xF8)<<8)|((d[i+1]&0xFC)<<3)|(d[i+2]>>3);
  buf[j++]=v&255;buf[j++]=v>>8;}
 say('Uploading image…');
 const r=await fetch('/upload/image',{method:'POST',
  headers:{'Content-Type':'application/octet-stream'},body:buf});
 if(!r.ok)throw new Error(await r.text());
 say('Image shown on panel','ok');
}
</script></main></body></html>)HTML";

// generic raw-body-to-file upload handler
void bodyToFile(AsyncWebServerRequest *req, const char *path,
                uint8_t *data, size_t len, size_t index, size_t total) {
  static File up;
  if (index == 0) {
    uploadBusy = true;
    if (gifOpen) { gif->close(); gifOpen = false; }
    up = LittleFS.open(path, "w");
  }
  if (up) up.write(data, len);
  if (index + len == total) {
    if (up) up.close();
    uploadBusy = false;
  }
}

void setupWeb() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *r) {
    r->send(200, "text/html", INDEX_HTML);
  });

  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *r) {
    char buf[96];
    snprintf(buf, sizeof(buf), "{\"w\":%d,\"h\":%d,\"brightness\":%u,\"mode\":%u}",
             PANEL_W, PANEL_H, prefs.getUChar("bri", 128), displayMode);
    r->send(200, "application/json", buf);
  });

  server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *r) {
    if (r->hasParam("v")) {
      uint8_t v = r->getParam("v")->value().toInt();
      dma->setBrightness8(v);
      prefs.putUChar("bri", v);
      if (displayMode == MODE_IMAGE) showImage(); // brightness needs redraw
    }
    r->send(200, "text/plain", "ok");
  });

  server.on("/upload/image", HTTP_POST,
    [](AsyncWebServerRequest *r) {
      dma->clearScreen();
      if (showImage()) { setMode(MODE_IMAGE); r->send(200, "text/plain", "ok"); }
      else r->send(400, "text/plain", "bad size: expected exactly W*H*2 bytes RGB565-LE");
    },
    nullptr,
    [](AsyncWebServerRequest *r, uint8_t *d, size_t l, size_t i, size_t t) {
      if (t != (size_t)PANEL_W * PANEL_H * 2 && i == 0) return; // still drain, reject in handler
      bodyToFile(r, IMG_PATH, d, l, i, t);
    });

  server.on("/upload/gif", HTTP_POST,
    [](AsyncWebServerRequest *r) {
      if (startGif()) { setMode(MODE_GIF); r->send(200, "text/plain", "ok"); }
      else r->send(400, "text/plain", "GIF decode failed");
    },
    nullptr,
    [](AsyncWebServerRequest *r, uint8_t *d, size_t l, size_t i, size_t t) {
      bodyToFile(r, GIF_PATH, d, l, i, t);
    });

  server.on("/wifireset", HTTP_GET, [](AsyncWebServerRequest *r) {
    r->send(200, "text/plain", "WiFi credentials wiped, rebooting into setup portal");
    WiFiManager wm;
    wm.resetSettings();
    delay(500);
    ESP.restart();
  });

  ElegantOTA.begin(&server);   // /update
  server.begin();
}

// ---------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // --- Panel ---
  HUB75_I2S_CFG::i2s_pins pins = {PIN_R1, PIN_G1, PIN_B1, PIN_R2, PIN_G2, PIN_B2,
                                  PIN_A, PIN_B, PIN_C, PIN_D, PIN_E,
                                  PIN_LAT, PIN_OE, PIN_CLK};
  HUB75_I2S_CFG cfg(PANEL_W, PANEL_H, PANEL_CHAIN, pins);
  cfg.clkphase = false;        // flip if you see a shifted/ghosting column
  dma = new MatrixPanel_I2S_DMA(cfg);
  dma->begin();

  prefs.begin("hub75");
  dma->setBrightness8(prefs.getUChar("bri", 128));
  dma->clearScreen();

  LittleFS.begin(true);
  gif = new AnimatedGIF();
  gif->begin(GIF_PALETTE_RGB565_LE);

  // --- WiFi provisioning ---
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  wm.setConfigPortalTimeout(180);          // fall through after 3 min
  if (!wm.autoConnect("HUB75-Setup")) {    // captive portal on first boot
    Serial.println("no WiFi, rebooting");
    ESP.restart();
  }
  Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
  MDNS.begin("hub75");                     // http://hub75.local/

  setupWeb();

  // --- Restore last content ---
  uint8_t m = prefs.getUChar("mode", MODE_NONE);
  if (m == MODE_GIF && startGif())       displayMode = MODE_GIF;
  else if (m == MODE_IMAGE && showImage()) displayMode = MODE_IMAGE;
}

void loop() {
  ElegantOTA.loop();

  if (displayMode == MODE_GIF && gifOpen && !uploadBusy) {
    if ((int32_t)(millis() - nextFrameAt) >= 0) {
      int delayMs = 0;
      if (!gif->playFrame(false, &delayMs)) {   // 0 => last frame reached
        gif->reset();                           // loop forever
        delayMs = 1;
      }
      nextFrameAt = millis() + max(delayMs, 20); // clamp broken 0ms GIFs
    }
  }
  delay(1);
}
