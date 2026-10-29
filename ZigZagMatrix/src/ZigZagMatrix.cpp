#include "ZigZagMatrix.h"

// ---------------------------------------------------------------- ctor

ZigZagMatrix::ZigZagMatrix(CRGB *leds, uint8_t w, uint8_t h,
                           bool serpentine, ZZMOrigin origin)
    : _leds(leds), _w(w), _h(h), _serpentine(serpentine) {
    _flipX = (origin == ZZM_TOP_RIGHT || origin == ZZM_BOTTOM_RIGHT);
    _flipY = (origin == ZZM_BOTTOM_LEFT || origin == ZZM_BOTTOM_RIGHT);
    _font  = &ZZM_FONT_5X7;
}

// ---------------------------------------------------------------- geometry

uint16_t ZigZagMatrix::xy(int16_t x, int16_t y) const {
    if (x < 0 || y < 0 || x >= _w || y >= _h) return 0xFFFF;

    uint8_t px = _flipX ? (_w - 1 - x) : x;
    uint8_t py = _flipY ? (_h - 1 - y) : y;

    // On a serpentine panel every odd physical row runs backwards.
    if (_serpentine && (py & 1)) px = _w - 1 - px;

    return (uint16_t)py * _w + px;
}

void ZigZagMatrix::setPixel(int16_t x, int16_t y, const CRGB &c) {
    uint16_t i = xy(x, y);
    if (i != 0xFFFF) _leds[i] = c;
}

CRGB ZigZagMatrix::getPixel(int16_t x, int16_t y) const {
    uint16_t i = xy(x, y);
    return (i == 0xFFFF) ? CRGB(0, 0, 0) : _leds[i];
}

void ZigZagMatrix::clear(const CRGB &c) {
    fill_solid(_leds, numLeds(), c);
}

void ZigZagMatrix::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                            const CRGB &c) {
    for (int16_t j = y; j < y + h; j++)
        for (int16_t i = x; i < x + w; i++) setPixel(i, j, c);
}

void ZigZagMatrix::fadeAll(uint8_t amount) {
    fadeToBlackBy(_leds, numLeds(), amount);
}

void ZigZagMatrix::scaleAll(uint8_t scale) {
    nscale8(_leds, numLeds(), scale);
}

void ZigZagMatrix::show() { FastLED.show(); }

// ---------------------------------------------------------------- colour

CRGB ZigZagMatrix::pixelColor(int16_t x, int16_t y, uint8_t glyphCol) const {
    (void)glyphCol;
    switch (_mode) {
        case ZZM_RAINBOW_X:
            return CHSV(_hueBase + (uint8_t)(x * _hueSpread), 255, 255);
        case ZZM_RAINBOW_Y:
            return CHSV(_hueBase + (uint8_t)(y * _hueSpread), 255, 255);
        case ZZM_RAINBOW_TIME:
            return CHSV(_hueBase, 255, 255);
        case ZZM_PALETTE_X:
            return ColorFromPalette(_pal, _hueBase + (uint8_t)(x * _hueSpread));
        case ZZM_SOLID:
        default:
            return _fg;
    }
}

// ---------------------------------------------------------------- text

int16_t ZigZagMatrix::textWidth(const char *s) const {
    if (!s || !*s) return 0;
    int16_t n    = (int16_t)strlen(s);
    int16_t step = (int16_t)_font->advance + _tracking;
    return n * step - (step - _font->width);
}

void ZigZagMatrix::drawChar(char c, int16_t x, int16_t y) {
    uint8_t code = (uint8_t)c;
    if (code < _font->first || code >= _font->first + _font->count) return;

    const uint8_t *g = _font->data +
                       (uint16_t)(code - _font->first) * _font->width;

    for (uint8_t col = 0; col < _font->width; col++) {
        int16_t px = x + col;
        if (px < 0 || px >= _w) continue;          // cheap horizontal clip
        uint8_t bits = pgm_read_byte(g + col);
        if (!bits) continue;
        for (uint8_t row = 0; row < _font->height; row++) {
            if (bits & (1 << row)) setPixel(px, y + row, pixelColor(px, y + row, col));
        }
    }
}

void ZigZagMatrix::drawText(const char *s, int16_t x, int16_t y) {
    if (!s) return;
    int16_t step = (int16_t)_font->advance + _tracking;
    for (const char *p = s; *p; p++, x += step) {
        if (x >= _w) break;                        // rest is off the right edge
        if (x + _font->width <= 0) continue;       // still off the left edge
        drawChar(*p, x, y);
    }
}

// ---------------------------------------------------------------- helpers

int16_t ZigZagMatrix::centerX() const { return (int16_t)(_w - _textW) / 2; }
int16_t ZigZagMatrix::centerY() const { return (int16_t)(_h - _font->height) / 2; }

void ZigZagMatrix::setText(const char *s) {
    strncpy(_text, s ? s : "", ZZM_MAX_TEXT - 1);
    _text[ZZM_MAX_TEXT - 1] = '\0';
    _textW = textWidth(_text);
}

void ZigZagMatrix::startAnim(ZZMAnim a, uint32_t ms) {
    _anim     = a;
    _duration = ms ? ms : 1;
    _t0       = millis();
    _done     = false;
    _lastFrame = 0;               // force an immediate first frame
}

void ZigZagMatrix::stop() { _anim = ZZM_NONE; _done = true; }

uint16_t ZigZagMatrix::applyEase(uint16_t p, ZZMEase e) {
    // p and result are 0..1000
    uint32_t t = p;
    switch (e) {
        case ZZM_EASE_OUT: {                       // 1-(1-t)^2
            uint32_t inv = 1000 - t;
            return (uint16_t)(1000 - (inv * inv) / 1000);
        }
        case ZZM_EASE_IN_OUT:
            if (t < 500) return (uint16_t)(2 * t * t / 1000);
            else {
                uint32_t inv = 1000 - t;
                return (uint16_t)(1000 - 2 * inv * inv / 1000);
            }
        case ZZM_BACK_OUT: {
            // f(t) = 1 + c3*(t-1)^3 + c1*(t-1)^2, c1=1.702, c3=2.702
            // Overshoots ~10% around t=0.7 then settles exactly on 1.
            int32_t u   = (int32_t)t - 1000;          // -1000..0
            int32_t uu  = (u * u) / 1000;             //     0..1000
            int32_t uuu = (uu * u) / 1000;            // -1000..0
            int32_t v   = 1000 + (2702 * uuu) / 1000 + (1702 * uu) / 1000;
            if (v < 0) v = 0;
            return (uint16_t)v;
        }
        case ZZM_LINEAR:
        default: return p;
    }
}

uint16_t ZigZagMatrix::progress() const {
    uint32_t el = millis() - _t0;
    uint32_t p  = (el >= _duration) ? 1000 : (el * 1000UL) / _duration;
    return applyEase((uint16_t)p, _ease);
}

uint8_t ZigZagMatrix::hash8(uint8_t x, uint8_t y) {
    uint16_t h = (uint16_t)(x * 37 + y * 151);
    h ^= h >> 4;
    h *= 0x2B9D;
    h ^= h >> 7;
    return (uint8_t)(h & 0xFF);
}

// Clears everything on one side of a moving edge.
// reveal = true  -> the leading region stays lit (wipe-in)
// reveal = false -> the leading region is cleared (wipe-out)
void ZigZagMatrix::maskDirectional(ZZMDir d, uint16_t p, bool reveal) {
    if (p > 1000) p = 1000;                 // easings may overshoot
    int16_t thrX = (int16_t)(((uint32_t)_w * p) / 1000);
    int16_t thrY = (int16_t)(((uint32_t)_h * p) / 1000);

    for (int16_t y = 0; y < _h; y++) {
        for (int16_t x = 0; x < _w; x++) {
            bool lead;
            switch (d) {
                case ZZM_LEFT:  lead = (x < thrX);            break;
                case ZZM_RIGHT: lead = (x >= _w - thrX);      break;
                case ZZM_UP:    lead = (y < thrY);            break;
                default:        lead = (y >= _h - thrY);      break;   // ZZM_DOWN
            }
            if (reveal ? !lead : lead) setPixel(x, y, _bg);
        }
    }
}

void ZigZagMatrix::maskDissolve(uint16_t p, bool reveal) {
    if (p > 1000) p = 1000;
    uint8_t thr = (uint8_t)(((uint32_t)255 * p) / 1000);
    for (int16_t y = 0; y < _h; y++)
        for (int16_t x = 0; x < _w; x++) {
            bool lead = (hash8((uint8_t)x, (uint8_t)y) < thr);
            if (reveal ? !lead : lead) setPixel(x, y, _bg);
        }
}

void ZigZagMatrix::renderStatic(int16_t x, int16_t y) {
    clear(_bg);
    drawText(_text, x, y);
}

// ---------------------------------------------------------------- API

void ZigZagMatrix::showText(const char *s) {
    setText(s);
    _sx = centerX(); _sy = centerY();
    startAnim(ZZM_STATIC, 1);
}

void ZigZagMatrix::showText(const char *s, int16_t x, int16_t y) {
    setText(s);
    _sx = x; _sy = y;
    startAnim(ZZM_STATIC, 1);
}

void ZigZagMatrix::scroll(const char *s, ZZMDir dir, uint16_t pxPerSec,
                          bool loop, uint8_t gap) {
    setText(s);
    _dir = dir; _pxPerSec = pxPerSec ? pxPerSec : 1; _loop = loop; _gap = gap;
    startAnim(ZZM_SCROLL, 1);
}

void ZigZagMatrix::swipeIn(const char *s, ZZMDir from, uint16_t ms) {
    setText(s); _dir = from; startAnim(ZZM_SWIPE_IN, ms);
}
void ZigZagMatrix::swipeOut(ZZMDir to, uint16_t ms) {
    _dir = to; startAnim(ZZM_SWIPE_OUT, ms);
}
void ZigZagMatrix::wipeIn(const char *s, ZZMDir from, uint16_t ms) {
    setText(s); _dir = from; startAnim(ZZM_WIPE_IN, ms);
}
void ZigZagMatrix::wipeOut(ZZMDir to, uint16_t ms) {
    _dir = to; startAnim(ZZM_WIPE_OUT, ms);
}
void ZigZagMatrix::fadeIn(const char *s, uint16_t ms) {
    setText(s); startAnim(ZZM_FADE_IN, ms);
}
void ZigZagMatrix::fadeOut(uint16_t ms)      { startAnim(ZZM_FADE_OUT, ms); }
void ZigZagMatrix::dissolveIn(const char *s, uint16_t ms) {
    setText(s); startAnim(ZZM_DISSOLVE_IN, ms);
}
void ZigZagMatrix::dissolveOut(uint16_t ms)  { startAnim(ZZM_DISSOLVE_OUT, ms); }

void ZigZagMatrix::typewriter(const char *s, uint16_t msPerChar) {
    setText(s);
    startAnim(ZZM_TYPEWRITER, (uint16_t)(msPerChar * (strlen(_text) + 1)));
}

void ZigZagMatrix::blink(const char *s, uint16_t onMs, uint16_t offMs,
                         uint8_t times) {
    setText(s);
    _onMs = onMs; _offMs = offMs; _times = times ? times : 1;
    startAnim(ZZM_BLINK, (uint16_t)((onMs + offMs) * _times));
}

void ZigZagMatrix::bounce(const char *s, uint16_t pxPerSec) {
    setText(s);
    _pxPerSec = pxPerSec ? pxPerSec : 1;
    startAnim(ZZM_BOUNCE, 1);
}

// ---------------------------------------------------------------- engine

bool ZigZagMatrix::update() {
    if (_anim == ZZM_NONE) return false;

    uint32_t now = millis();
    if (_lastFrame && (uint32_t)(now - _lastFrame) < _framePeriod) return false;

    // A finished one-shot keeps its last frame on screen; stop redrawing
    // unless the colour mode is time-varying.
    if (_done && _anim != ZZM_SCROLL && _anim != ZZM_BOUNCE &&
        _mode != ZZM_RAINBOW_TIME) return false;

    _lastFrame = now;
    _hueBase += _hueSpeed;

    const int16_t fh   = _font->height;
    const int16_t cx   = centerX();
    const int16_t cy   = centerY();
    const uint32_t el  = now - _t0;

    switch (_anim) {

    case ZZM_STATIC:
        renderStatic(_sx, _sy);
        _done = true;
        break;

    case ZZM_SCROLL: {
        int32_t travel = ((int32_t)el * _pxPerSec) / 1000;
        clear(_bg);
        if (_dir == ZZM_LEFT || _dir == ZZM_RIGHT) {
            int32_t period = _textW + _gap;
            if (period < 1) period = 1;      // guard empty text + zero gap
            if (_loop) {
                int32_t base = (_dir == ZZM_LEFT)
                    ? _w - (travel % period)
                    : -_textW + (travel % period);
                // Tile copies until the panel is covered in both directions.
                for (int32_t x = base; x > -_textW; x -= period)
                    drawText(_text, (int16_t)x, cy);
                for (int32_t x = base + period; x < _w; x += period)
                    drawText(_text, (int16_t)x, cy);
            } else {
                int32_t x = (_dir == ZZM_LEFT) ? _w - travel
                                               : -_textW + travel;
                drawText(_text, (int16_t)x, cy);
                if ((_dir == ZZM_LEFT && x < -_textW) ||
                    (_dir == ZZM_RIGHT && x > _w)) _done = true;
            }
        } else {
            int32_t period = fh + _gap;
            if (period < 1) period = 1;
            if (_loop) {
                int32_t base = (_dir == ZZM_UP)
                    ? _h - (travel % period)
                    : -fh + (travel % period);
                for (int32_t y = base; y > -fh; y -= period)
                    drawText(_text, cx, (int16_t)y);
                for (int32_t y = base + period; y < _h; y += period)
                    drawText(_text, cx, (int16_t)y);
            } else {
                int32_t y = (_dir == ZZM_UP) ? _h - travel : -fh + travel;
                drawText(_text, cx, (int16_t)y);
                if ((_dir == ZZM_UP && y < -fh) ||
                    (_dir == ZZM_DOWN && y > _h)) _done = true;
            }
        }
        break;
    }

    case ZZM_SWIPE_IN:
    case ZZM_SWIPE_OUT: {
        uint16_t p = progress();
        bool in = (_anim == ZZM_SWIPE_IN);
        int16_t offX = cx, offY = cy, fromX = cx, fromY = cy;

        switch (_dir) {
            case ZZM_LEFT:  fromX = -_textW; break;   // enters from / exits to left
            case ZZM_RIGHT: fromX = _w;      break;
            case ZZM_UP:    fromY = -fh;     break;
            default:        fromY = _h;      break;   // ZZM_DOWN
        }
        int16_t a_x = in ? fromX : cx, b_x = in ? cx : fromX;
        int16_t a_y = in ? fromY : cy, b_y = in ? cy : fromY;
        offX = a_x + (int16_t)(((int32_t)(b_x - a_x) * p) / 1000);
        offY = a_y + (int16_t)(((int32_t)(b_y - a_y) * p) / 1000);

        renderStatic(offX, offY);
        if (millis() - _t0 >= _duration) _done = true;
        break;
    }

    case ZZM_WIPE_IN:
    case ZZM_WIPE_OUT: {
        uint16_t p = progress();
        renderStatic(cx, cy);
        maskDirectional(_dir, p, _anim == ZZM_WIPE_IN);
        if (millis() - _t0 >= _duration) _done = true;
        break;
    }

    case ZZM_FADE_IN:
    case ZZM_FADE_OUT: {
        uint16_t p = progress();
        if (p > 1000) p = 1000;
        uint8_t scale = (_anim == ZZM_FADE_IN)
                            ? (uint8_t)((p * 255UL) / 1000)
                            : (uint8_t)(255 - (p * 255UL) / 1000);
        renderStatic(cx, cy);
        scaleAll(scale);
        if (millis() - _t0 >= _duration) _done = true;
        break;
    }

    case ZZM_DISSOLVE_IN:
    case ZZM_DISSOLVE_OUT: {
        uint16_t p = progress();
        renderStatic(cx, cy);
        maskDissolve(p, _anim == ZZM_DISSOLVE_IN);
        if (millis() - _t0 >= _duration) _done = true;
        break;
    }

    case ZZM_TYPEWRITER: {
        uint16_t len   = (uint16_t)strlen(_text);
        uint16_t step  = _duration / (len + 1);
        uint16_t shown = step ? (uint16_t)(el / step) : len;
        if (shown > len) shown = len;

        char tmp[ZZM_MAX_TEXT];
        strncpy(tmp, _text, shown);
        tmp[shown] = '\0';

        clear(_bg);
        int16_t x = cx;                     // fixed left edge: no jitter
        drawText(tmp, x, cy);
        if (shown < len) {                  // caret
            int16_t caret = x + shown * (_font->advance + _tracking);
            fillRect(caret, cy, 1, _font->height, pixelColor(caret, cy, 0));
        }
        if (el >= _duration) _done = true;
        break;
    }

    case ZZM_BLINK: {
        uint16_t cycle = _onMs + _offMs;
        uint16_t phase = cycle ? (uint16_t)(el % cycle) : 0;
        clear(_bg);
        if (phase < _onMs) drawText(_text, cx, cy);
        if (el >= (uint32_t)cycle * _times) _done = true;
        break;
    }

    case ZZM_BOUNCE: {
        int32_t range = _textW - _w;
        clear(_bg);
        if (range <= 0) {
            drawText(_text, cx, cy);
        } else {
            int32_t travel = ((int32_t)el * _pxPerSec) / 1000;
            int32_t pos    = travel % (2 * range);
            if (pos > range) pos = 2 * range - pos;
            drawText(_text, (int16_t)(-pos), cy);
        }
        break;
    }

    default: break;
    }

    if (_autoShow) FastLED.show();
    return true;
}
