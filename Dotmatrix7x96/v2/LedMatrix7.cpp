#include "LedMatrix7.h"
#include "font5x7.h"
#include <string.h>
#include <stdlib.h>

// ---------------------------------------------------------------- lifecycle

bool LedMatrix7::begin(const Config& cfg) {
  _cfg = cfg;
  if (_cfg.numModules < 1) _cfg.numModules = 1;
  if (_cfg.frameRateHz < 30) _cfg.frameRateHz = 30;

  _w   = 96 * _cfg.numModules;
  _bpr = PER_MODULE_BYTES * _cfg.numModules;
  _rowPeriodUs = 1000000UL / ((uint32_t)_cfg.frameRateHz * ROWS);

  _bufA = (uint8_t*)calloc(ROWS * _bpr, 1);
  _bufB = (uint8_t*)calloc(ROWS * _bpr, 1);
  _line = (uint8_t*)malloc(_bpr);
  if (!_bufA || !_bufB || !_line) { end(); return false; }
  _active = _bufA;
  _draw   = _bufB;

  // GPIO
  for (int i = 0; i < ROWS; i++) pinMode(_cfg.rowPins[i], OUTPUT);
  pinMode(_cfg.oePin, OUTPUT);
  if (_cfg.strobePin >= 0) pinMode(_cfg.strobePin, OUTPUT);
  setOE(false);          // start blanked
  selectRow(-1);
  if (_cfg.strobePin >= 0)
    digitalWrite(_cfg.strobePin, _cfg.strobeActiveHigh ? LOW : HIGH);

  // SPI (no CS line; registers just take continuous clock+data)
  _spi = new SPIClass(_cfg.spiHost);
  _spi->begin(_cfg.sclkPin, -1, _cfg.mosiPin, -1);
  _spiSettings = SPISettings(_cfg.spiHz, _cfg.bitOrder, SPI_MODE0);

  // Brightness one-shot (task-dispatch, so it may touch GPIO safely)
  esp_timer_create_args_t ba = {};
  ba.callback = &LedMatrix7::blankThunk;
  ba.arg = this;
  ba.dispatch_method = ESP_TIMER_TASK;
  ba.name = "ledmtx_blank";
  esp_timer_create(&ba, &_blankTimer);

  // Periodic row scanner. Task dispatch => SPI calls in the callback are OK,
  // and the CPU is free between callbacks (no busy-wait, no watchdog starve).
  esp_timer_create_args_t ta = {};
  ta.callback = &LedMatrix7::rowTimerThunk;
  ta.arg = this;
  ta.dispatch_method = ESP_TIMER_TASK;
  ta.name = "ledmtx_scan";
  esp_timer_create(&ta, &_timer);

  _running = true;
  esp_timer_start_periodic(_timer, _rowPeriodUs);
  return true;
}

void LedMatrix7::end() {
  _running = false;
  if (_timer)      { esp_timer_stop(_timer);      esp_timer_delete(_timer);      _timer = nullptr; }
  if (_blankTimer) { esp_timer_stop(_blankTimer); esp_timer_delete(_blankTimer); _blankTimer = nullptr; }
  setOE(false);
  selectRow(-1);
  if (_spi) { _spi->end(); delete _spi; _spi = nullptr; }
  free(_bufA); free(_bufB); free(_line);
  _bufA = _bufB = _line = _active = _draw = nullptr;
}

// ---------------------------------------------------------------- refresh

void LedMatrix7::rowTimerThunk(void* arg) { static_cast<LedMatrix7*>(arg)->onRow(); }
void LedMatrix7::blankThunk(void* arg)    { static_cast<LedMatrix7*>(arg)->setOE(false); }

void LedMatrix7::onRow() {
  // 1. blank, 2. drop row, 3. load columns, 4. (latch), 5. pick row, 6. unblank
  setOE(false);
  selectRow(-1);
  shiftRow(_active, mapRow(_curRow));
  if (_cfg.strobePin >= 0) pulseStrobe();
  selectRow(_curRow);
  setOE(true);

  // brightness: re-blank partway through the row slot
  if (_brightness < 255) {
    uint32_t onUs = (uint32_t)((uint64_t)_rowPeriodUs * _brightness / 255);
    if (onUs < _rowPeriodUs) esp_timer_start_once(_blankTimer, onUs);
  }

  // advance row; swap buffers only at a frame boundary (no tearing)
  uint8_t nr = _curRow + 1;
  if (nr >= ROWS) {
    nr = 0;
    if (_swapPending) {
      uint8_t* t = _active; _active = _draw; _draw = t;
      _swapPending = false;
    }
  }
  _curRow = nr;
}

void LedMatrix7::shiftRow(const uint8_t* buf, int row) {
  const uint8_t* src = buf + row * _bpr;
  const int N = _cfg.numModules;
  int idx = 0;
  for (int m = 0; m < N; m++) {
    int sm = _cfg.moduleReverse ? (N - 1 - m) : m;
    for (int k = 0; k < PER_MODULE_BYTES; k++) {
      int sk = _cfg.byteReverse ? (PER_MODULE_BYTES - 1 - k) : k;
      _line[idx++] = src[sm * PER_MODULE_BYTES + sk];
    }
  }
  _spi->beginTransaction(_spiSettings);
  _spi->writeBytes(_line, _bpr);
  _spi->endTransaction();
}

void LedMatrix7::selectRow(int r) {
  const int off = _cfg.rowActiveHigh ? LOW : HIGH;
  const int on  = _cfg.rowActiveHigh ? HIGH : LOW;
  for (int i = 0; i < ROWS; i++) digitalWrite(_cfg.rowPins[i], off);
  if (r >= 0 && r < ROWS) digitalWrite(_cfg.rowPins[r], on);
}

inline void LedMatrix7::setOE(bool on) {
  int lvl = on ? (_cfg.oeActiveHigh ? HIGH : LOW)
               : (_cfg.oeActiveHigh ? LOW  : HIGH);
  digitalWrite(_cfg.oePin, lvl);
}

inline void LedMatrix7::pulseStrobe() {
  int a = _cfg.strobeActiveHigh ? HIGH : LOW;
  int i = _cfg.strobeActiveHigh ? LOW  : HIGH;
  digitalWrite(_cfg.strobePin, i);
  digitalWrite(_cfg.strobePin, a);   // capture edge
  digitalWrite(_cfg.strobePin, i);
}

// ---------------------------------------------------------------- buffer

inline void LedMatrix7::bufSet(uint8_t* b, int bpr, int x, int y, bool on) {
  uint8_t* p = &b[y * bpr + (x >> 3)];
  uint8_t  m = 0x80 >> (x & 7);
  if (on) *p |= m; else *p &= ~m;
}
inline bool LedMatrix7::bufGet(const uint8_t* b, int bpr, int x, int y) {
  return b[y * bpr + (x >> 3)] & (0x80 >> (x & 7));
}

void LedMatrix7::clear(bool on) { memset(_draw, on ? 0xFF : 0x00, ROWS * _bpr); }

void LedMatrix7::setPixel(int x, int y, bool on) {
  if (y < 0 || y >= ROWS || x < 0 || x >= _w) return;
  int xf = _cfg.flipX ? (_w - 1 - x) : x;
  bufSet(_draw, _bpr, xf, y, on);
}
bool LedMatrix7::getPixel(int x, int y) const {
  if (y < 0 || y >= ROWS || x < 0 || x >= _w) return false;
  int xf = _cfg.flipX ? (_w - 1 - x) : x;
  return bufGet(_draw, _bpr, xf, y);
}

void LedMatrix7::fillRect(int x, int y, int w, int h, bool on) {
  for (int j = 0; j < h; j++)
    for (int i = 0; i < w; i++) setPixel(x + i, y + j, on);
}

void LedMatrix7::drawChar(int x, int y, char c, bool on) {
  if (c < FONT5x7_FIRST || c > FONT5x7_LAST) return;
  const uint8_t* g = FONT5x7[c - FONT5x7_FIRST];
  for (int col = 0; col < 5; col++) {
    uint8_t cb = pgm_read_byte(&g[col]);
    for (int row = 0; row < ROWS; row++)
      if (cb & (1 << row)) setPixel(x + col, y + row, on);
  }
}

int LedMatrix7::drawText(int x, int y, const char* s, bool on) {
  int cx = x;
  for (const char* p = s; *p; ++p) { drawChar(cx, y, *p, on); cx += 6; }
  return cx - x;
}

int LedMatrix7::textWidth(const char* s) const { return (int)strlen(s) * 6; }

void LedMatrix7::show() {
  if (!_running) { uint8_t* t = _active; _active = _draw; _draw = t; return; }
  _swapPending = true;
  uint32_t t0 = millis();
  while (_swapPending && (millis() - t0) < 50) delay(1);
}

// ---------------------------------------------------------------- animation

void LedMatrix7::drawCharBuf(uint8_t* b, int bpr, int x, int y, char c) {
  if (c < FONT5x7_FIRST || c > FONT5x7_LAST) return;
  const uint8_t* g = FONT5x7[c - FONT5x7_FIRST];
  for (int col = 0; col < 5; col++) {
    uint8_t cb = pgm_read_byte(&g[col]);
    for (int row = 0; row < ROWS; row++)
      if (cb & (1 << row)) {
        int xx = x + col;
        if (xx >= 0 && xx < (bpr << 3) && y + row < ROWS && y + row >= 0)
          bufSet(b, bpr, xx, y + row, true);
      }
  }
}

void LedMatrix7::blit(const uint8_t* scene, int sBpr, int sW, int xoff, int yoff) {
  for (int sy = 0; sy < ROWS; sy++) {
    int dy = sy + yoff;
    if (dy < 0 || dy >= ROWS) continue;
    for (int sx = 0; sx < sW; sx++) {
      int dx = sx + xoff;
      if (dx < 0 || dx >= _w) continue;
      if (bufGet(scene, sBpr, sx, sy)) setPixel(dx, dy, true);
    }
  }
}

void LedMatrix7::scrollText(const char* s, int speedPxPerSec,
                            bool leftToRight, int gapPx, int loops) {
  int n = (int)strlen(s);
  if (n == 0) return;
  int sW   = n * 6 + (gapPx > 0 ? gapPx : 0);
  int sBpr = (sW + 7) / 8;
  uint8_t* scene = (uint8_t*)calloc(ROWS * sBpr, 1);
  if (!scene) return;
  int cx = 0;
  for (int i = 0; i < n; i++) { drawCharBuf(scene, sBpr, cx, 0, s[i]); cx += 6; }

  int stepMs = 1000 / (speedPxPerSec > 0 ? speedPxPerSec : 1);
  if (stepMs < 1) stepMs = 1;

  for (int l = 0; l < loops; l++) {
    if (!leftToRight) {
      for (int off = _w; off >= -sW; off--) {
        clear(false); blit(scene, sBpr, sW, off, 0); show(); delay(stepMs);
      }
    } else {
      for (int off = -sW; off <= _w; off++) {
        clear(false); blit(scene, sBpr, sW, off, 0); show(); delay(stepMs);
      }
    }
  }
  free(scene);
}

void LedMatrix7::slideInText(const char* s, SlideDir dir,
                             int speedPxPerSec, int holdMs) {
  int n = (int)strlen(s);
  int textW = n * 6;
  uint8_t* scene = (uint8_t*)calloc(ROWS * _bpr, 1);
  if (!scene) return;
  int x0 = (textW < _w) ? (_w - textW) / 2 : 0;
  int cx = x0;
  for (int i = 0; i < n; i++) { drawCharBuf(scene, _bpr, cx, 0, s[i]); cx += 6; }

  int stepMs = 1000 / (speedPxPerSec > 0 ? speedPxPerSec : 1);
  if (stepMs < 1) stepMs = 1;

  switch (dir) {
    case FROM_LEFT:
      for (int off = -_w; off <= 0; off++) { clear(); blit(scene, _bpr, _w, off, 0); show(); delay(stepMs); }
      break;
    case FROM_RIGHT:
      for (int off = _w; off >= 0; off--) { clear(); blit(scene, _bpr, _w, off, 0); show(); delay(stepMs); }
      break;
    case FROM_TOP:
      for (int yo = -ROWS; yo <= 0; yo++) { clear(); blit(scene, _bpr, _w, 0, yo); show(); delay(stepMs); }
      break;
    case FROM_BOTTOM:
      for (int yo = ROWS; yo >= 0; yo--) { clear(); blit(scene, _bpr, _w, 0, yo); show(); delay(stepMs); }
      break;
  }
  clear(); blit(scene, _bpr, _w, 0, 0); show();
  if (holdMs > 0) delay(holdMs);
  free(scene);
}

// ---------------------------------------------------------------- static / clear

void LedMatrix7::writeText(const char* s, bool center) {
  clear(false);
  int textW = (int)strlen(s) * 6;
  int x0 = (center && textW < _w) ? (_w - textW) / 2 : 0;
  drawText(x0, 0, s, true);
  show();
}

void LedMatrix7::clearNow(bool on) {
  clear(on);
  show();
}

// Read the currently displayed buffer back into a logical (un-flipped) scene.
void LedMatrix7::captureLogical(uint8_t* scene, int sBpr) {
  memset(scene, 0, ROWS * sBpr);
  for (int y = 0; y < ROWS; y++)
    for (int x = 0; x < _w; x++) {
      int xf = _cfg.flipX ? (_w - 1 - x) : x;
      if (bufGet(_active, _bpr, xf, y)) bufSet(scene, sBpr, x, y, true);
    }
}

void LedMatrix7::clearOut(SlideDir dir, int speedPxPerSec) {
  uint8_t* scene = (uint8_t*)calloc(ROWS * _bpr, 1);
  if (!scene) { clearNow(); return; }
  captureLogical(scene, _bpr);

  int stepMs = 1000 / (speedPxPerSec > 0 ? speedPxPerSec : 1);
  if (stepMs < 1) stepMs = 1;

  switch (dir) {
    case FROM_LEFT:   // departs toward the left edge
      for (int off = 0; off >= -_w; off--)   { clear(); blit(scene, _bpr, _w, off, 0); show(); delay(stepMs); }
      break;
    case FROM_RIGHT:
      for (int off = 0; off <= _w; off++)    { clear(); blit(scene, _bpr, _w, off, 0); show(); delay(stepMs); }
      break;
    case FROM_TOP:
      for (int yo = 0; yo >= -ROWS; yo--)    { clear(); blit(scene, _bpr, _w, 0, yo); show(); delay(stepMs); }
      break;
    case FROM_BOTTOM:
      for (int yo = 0; yo <= ROWS; yo++)     { clear(); blit(scene, _bpr, _w, 0, yo); show(); delay(stepMs); }
      break;
  }
  clearNow();
  free(scene);
}
