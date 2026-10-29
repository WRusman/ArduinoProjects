#pragma once
//
// LedMatrix7 - ESP32 driver for 7-row x (96*N)-column LED matrix panels
// whose columns are fed by daisy-chained 8-bit shift registers (single
// serial input) and whose 7 rows are selected by direct GPIO.
//
// Design constraint this library is built around:
//   The storage latch of the column shift registers is NOT controllable
//   (it is hardwired transparent/enabled). The library therefore uses
//   /OE (output enable) as a global blank and does its own row multiplex
//   from a RAM framebuffer. SPI streams the column bits; GPIO picks rows.
//
// Target: arduino-esp32 core, original ESP32 (WROOM, DevKit V1).
//
#include <Arduino.h>
#include <SPI.h>
#include "esp_timer.h"

class LedMatrix7 {
public:
  // Direction = the edge the content enters from.
  enum SlideDir { FROM_LEFT, FROM_RIGHT, FROM_TOP, FROM_BOTTOM };

  struct Config {
    // 7 row-enable GPIOs, index 0 = top row
    uint8_t  rowPins[7]   = {32, 33, 25, 26, 27, 14, 13};
    int8_t   oePin        = 4;     // shift-register /OE (blank)
    int8_t   strobePin    = -1;    // -1 = no controllable latch (transparent)
    int8_t   mosiPin      = 23;    // SPI data  -> register serial in
    int8_t   sclkPin      = 18;    // SPI clock -> register clock
    uint8_t  numModules   = 1;     // horizontal daisy-chain count
    uint32_t spiHz        = 4000000;

    // Electrical polarity
    bool rowActiveHigh    = true;  // level that enables a row
    bool oeActiveHigh     = false; // false => /OE active-low (ON when LOW)
    bool strobeActiveHigh = true;  // latch capture edge polarity

    // Orientation / wiring knobs (tune once during bring-up)
    uint8_t bitOrder      = MSBFIRST; // bit order within each byte (SPI)
    bool moduleReverse    = false;    // reverse module order along the chain
    bool byteReverse      = false;    // reverse byte order within a module
    bool flipX            = false;    // mirror columns
    bool flipY            = false;    // mirror rows

    uint8_t frameRateHz   = 120;   // whole-panel refresh rate
    uint8_t spiHost       = VSPI;  // VSPI or HSPI
  };

  bool begin(const Config& cfg);
  void end();

  int width()  const { return _w; }
  int height() const { return 7; }

  // --- framebuffer drawing (writes to the back/draw buffer) ---
  void clear(bool on = false);
  void setPixel(int x, int y, bool on);
  bool getPixel(int x, int y) const;
  void fillRect(int x, int y, int w, int h, bool on);
  void drawChar(int x, int y, char c, bool on = true);
  int  drawText(int x, int y, const char* s, bool on = true); // returns px width
  int  textWidth(const char* s) const;

  void show();                              // swap back->front at frame edge
  void setBrightness(uint8_t b) { _brightness = b; } // 0..255

  // --- blocking animations ---
  void scrollText(const char* s, int speedPxPerSec,
                  bool leftToRight = false, int gapPx = 8, int loops = 1);
  void slideInText(const char* s, SlideDir dir,
                   int speedPxPerSec, int holdMs = 600);

private:
  static constexpr int ROWS = 7;
  static constexpr int PER_MODULE_BYTES = 12; // 96 cols / 8

  Config        _cfg;
  SPIClass*     _spi  = nullptr;
  SPISettings   _spiSettings;
  esp_timer_handle_t _timer      = nullptr; // periodic row scan
  esp_timer_handle_t _blankTimer = nullptr; // one-shot brightness blank

  uint8_t* _bufA = nullptr;
  uint8_t* _bufB = nullptr;
  uint8_t* _active = nullptr; // currently displayed
  uint8_t* _draw   = nullptr; // being composed
  uint8_t* _line   = nullptr; // scratch row for SPI

  int _w   = 96;
  int _bpr = 12;              // bytes per row across the whole chain
  volatile uint8_t _curRow = 0;
  volatile bool    _swapPending = false;
  volatile uint8_t _brightness  = 255;
  uint32_t _rowPeriodUs = 1190;
  bool _running = false;

  // refresh internals
  static void rowTimerThunk(void* arg);
  static void blankThunk(void* arg);
  void onRow();
  void shiftRow(const uint8_t* buf, int row);
  void selectRow(int r);       // -1 = all off
  inline void setOE(bool on);
  inline void pulseStrobe();
  inline int  mapRow(int r) const { return _cfg.flipY ? (ROWS - 1 - r) : r; }

  // buffer helpers (logical coords, MSB = lower x within a byte)
  static inline void bufSet(uint8_t* b, int bpr, int x, int y, bool on);
  static inline bool bufGet(const uint8_t* b, int bpr, int x, int y);
  void drawCharBuf(uint8_t* b, int bpr, int x, int y, char c);
  void blit(const uint8_t* scene, int sBpr, int sW, int xoff, int yoff);
};
