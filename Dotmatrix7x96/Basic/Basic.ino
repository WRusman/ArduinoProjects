#include <LedMatrix7.h>
#include "DateTime.h"

LedMatrix7 matrix;

void setup() {
  LedMatrix7::Config cfg;     // start from defaults, override what you wired
  cfg.rowPins[0] = 32;        // top row
  cfg.rowPins[1] = 33;
  cfg.rowPins[2] = 25;
  cfg.rowPins[3] = 26;
  cfg.rowPins[4] = 27;
  cfg.rowPins[5] = 14;
  cfg.rowPins[6] = 13;        // bottom row
  cfg.oePin      = 4;
  cfg.strobePin  = -1;        // -1: latch is transparent / not controllable
  cfg.mosiPin    = 23;        // -> shift register serial in
  cfg.sclkPin    = 18;        // -> shift register clock
  cfg.numModules = 1;         // set to how many panels are chained left->right
  cfg.spiHz      = 3000000;

  // Polarity / orientation: leave as-is, then run the BRING-UP block below
  // and flip booleans until the test pixel is top-left and a line runs L->R.
  cfg.rowActiveHigh    = false;
  cfg.oeActiveHigh     = false;  // /OE active-low
  cfg.flipX            = false;
  cfg.flipY            = true;
  cfg.byteReverse      = false;
  cfg.moduleReverse    = false;
  cfg.bitOrder         = MSBFIRST;

  matrix.begin(cfg);
  matrix.setBrightness(255);

  // ---- BRING-UP: uncomment to find correct orientation flags ----
  // matrix.clear(); matrix.setPixel(0, 0, true); matrix.show();
  // delay(3000);                       // expect ONLY the top-left LED
  // matrix.clear(); matrix.drawText(0, 0, "01", true); matrix.show();
  // delay(3000);                       // expect "01" reading left to right
}

void loop() {
  matrix.scrollText("ESP32 LED MATRIX  ", 40, /*leftToRight=*/false, 8, 1);
  matrix.slideInText("HELLO", LedMatrix7::FROM_LEFT,   60, 800);
  matrix.slideInText("WORLD", LedMatrix7::FROM_RIGHT,  60, 800);
  matrix.slideInText("UP",    LedMatrix7::FROM_BOTTOM, 30, 800);
  matrix.slideInText("DOWN",  LedMatrix7::FROM_TOP,    30, 800);
  matrix.clear(); 
  matrix.show(); 
  delay(300);
}
