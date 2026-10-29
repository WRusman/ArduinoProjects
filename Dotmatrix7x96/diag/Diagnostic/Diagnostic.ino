// Diagnostic.ino - bare-metal bring-up for the 7-row LED matrix.
// No framebuffer, no timer: raw SPI + GPIO, slowed down, so the panel
// reveals its true polarities. Run PHASE 1, read the result off the panel,
// hardcode the 3 results below, then run PHASE 2 for column/row ordering.
//
// Open Serial Monitor at 115200.

#include <SPI.h>

// ---- EDIT to match your wiring ----
const uint8_t ROW_PINS[7] = {32, 33, 25, 26, 27, 14, 13}; // index 0 = expected top row
const int8_t  OE_PIN   = 4;
const int8_t  STROBE   = -1;     // set to a GPIO only if your latch is controllable
const int8_t  MOSI_PIN = 23;
const int8_t  SCLK_PIN = 18;
const uint8_t MODULES  = 1;
const uint32_t SPI_HZ  = 4000000;

// ---- PHASE 2 controls: fill these in from PHASE 1, then set RUN_PHASE2 = true ----
bool RUN_PHASE2     = false;
int  OE_ON_LEVEL    = LOW;    // OE level that ENABLES output
int  ROW_ON_LEVEL   = HIGH;   // row level that ENABLES a row
bool DATA_INVERT    = false;  // true if 0x00 lit the panel (columns active-low)
// ---------------------------------------------------------------------------------

const int BPR = 12 * MODULES;
SPIClass spi(VSPI);

static inline int notLevel(int l) { return l == HIGH ? LOW : HIGH; }

void pulseStrobe() {
  if (STROBE < 0) return;
  digitalWrite(STROBE, LOW);
  digitalWrite(STROBE, HIGH);   // assume rising-edge capture; flip if needed
  digitalWrite(STROBE, LOW);
}

// raw shift, no inversion (used in PHASE 1 to discover data polarity)
void rawFill(uint8_t v) {
  uint8_t tmp[128];
  memset(tmp, v, BPR);
  spi.beginTransaction(SPISettings(SPI_HZ, MSBFIRST, SPI_MODE0));
  spi.writeBytes(tmp, BPR);
  spi.endTransaction();
  pulseStrobe();
}

// logical shift, 1 = lit; applies DATA_INVERT (used in PHASE 2)
void sendLogical(uint8_t* b) {
  uint8_t tmp[128];
  for (int i = 0; i < BPR; i++) tmp[i] = DATA_INVERT ? (uint8_t)~b[i] : b[i];
  spi.beginTransaction(SPISettings(SPI_HZ, MSBFIRST, SPI_MODE0));
  spi.writeBytes(tmp, BPR);
  spi.endTransaction();
  pulseStrobe();
}

void allRows(int level) { for (int i = 0; i < 7; i++) digitalWrite(ROW_PINS[i], level); }
void onlyRow(int r, int onLevel) {
  int off = notLevel(onLevel);
  for (int i = 0; i < 7; i++) digitalWrite(ROW_PINS[i], i == r ? onLevel : off);
}

void setup() {
  Serial.begin(115200);
  delay(400);
  for (int i = 0; i < 7; i++) pinMode(ROW_PINS[i], OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  if (STROBE >= 0) pinMode(STROBE, OUTPUT);
  spi.begin(SCLK_PIN, -1, MOSI_PIN, -1);

  if (!RUN_PHASE2) {
    Serial.println("=== PHASE 1: find polarities ===");
    Serial.println("Watch the panel. Note the combo number where the WHOLE panel lights.");
    int idx = 1;
    for (int draw = 0; draw < 2; draw++) {
      uint8_t v = draw ? 0xFF : 0x00;
      for (int rl = 0; rl < 2; rl++) {
        int rowLevel = rl ? HIGH : LOW;
        for (int ol = 0; ol < 2; ol++) {
          int oeLevel = ol ? HIGH : LOW;
          rawFill(v);
          allRows(rowLevel);
          digitalWrite(OE_PIN, oeLevel);
          Serial.printf("Combo %d: data=0x%02X  rowLevel=%-4s  oeLevel=%-4s\n",
                        idx, v, rowLevel ? "HIGH" : "LOW", oeLevel ? "HIGH" : "LOW");
          delay(2500);
          idx++;
        }
      }
    }
    digitalWrite(OE_PIN, HIGH);
    allRows(LOW);
    Serial.println();
    Serial.println("From the combo that lit the WHOLE panel:");
    Serial.println("  OE_ON_LEVEL  = its oeLevel");
    Serial.println("  ROW_ON_LEVEL = its rowLevel");
    Serial.println("  DATA_INVERT  = true if its data was 0x00, else false");
    Serial.println("If NOTHING ever lit fully, your latch needs pulsing: set STROBE and re-run.");
    Serial.println("Enter those 3 values above, set RUN_PHASE2 = true, re-flash.");
  }
}

void loop() {
  if (!RUN_PHASE2) { delay(2000); return; }

  int oeOff = notLevel(OE_ON_LEVEL);

  // --- Column walk: single lit column steps across, TOP row (index 0) only ---
  Serial.println("PHASE 2a: a single column should start TOP-LEFT and step RIGHT.");
  for (int c = 0; c < BPR * 8; c++) {
    digitalWrite(OE_PIN, oeOff);
    onlyRow(0, ROW_ON_LEVEL);
    uint8_t buf[128];
    memset(buf, 0, BPR);
    buf[c >> 3] |= (0x80 >> (c & 7));   // logical 'on' at column c, MSB = lower index
    sendLogical(buf);
    digitalWrite(OE_PIN, OE_ON_LEVEL);
    delay(200);
  }
  Serial.println("  starts top-RIGHT, steps left -> set flipX");
  Serial.println("  reverses within each block of 8 -> switch bitOrder (LSBFIRST)");
  Serial.println("  blocks of 8 out of order -> set byteReverse");
  Serial.println("  panels out of order (N>1) -> set moduleReverse");

  delay(1000);

  // --- Row walk: single lit column at col 0, active row steps 0..6 ---
  Serial.println("PHASE 2b: the lit dot should move TOP -> BOTTOM as index goes 0..6.");
  uint8_t buf0[128];
  memset(buf0, 0, BPR);
  buf0[0] = 0x80;
  for (int r = 0; r < 7; r++) {
    digitalWrite(OE_PIN, oeOff);
    onlyRow(r, ROW_ON_LEVEL);
    sendLogical(buf0);
    digitalWrite(OE_PIN, OE_ON_LEVEL);
    Serial.printf("  rowPins index %d active\n", r);
    delay(900);
  }
  Serial.println("  index 0 lights the BOTTOM row -> set flipY (or reorder ROW_PINS)");
  Serial.println("--- repeating ---");
  delay(1500);
}
