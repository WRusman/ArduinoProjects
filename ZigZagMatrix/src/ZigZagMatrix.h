/*
 * ZigZagMatrix - text and animation engine for serpentine WS2812 matrices.
 *
 * Target hardware for the defaults: 40 LEDs/row x 10 rows, zig-zag wired.
 *
 * Buffering model
 * ---------------
 * The CRGB array you hand to FastLED *is* the frame buffer. FastLED.show()
 * is synchronous (it blocks until the last bit is clocked out), so nothing
 * can tear mid-frame and a second buffer would only cost RAM. Every
 * animation frame is composed from scratch: clear -> draw -> optional mask
 * pass -> show. At 400 pixels that is a few hundred microseconds.
 *
 * The renderer is non-blocking: call update() as often as you like, it
 * rate-limits itself and returns true when it actually redrew.
 */

#pragma once
#include <Arduino.h>
#include <FastLED.h>

#ifndef ZZM_MAX_TEXT
#define ZZM_MAX_TEXT 96          // bytes reserved for the active string
#endif

// ---------------------------------------------------------------- font

struct ZZMFont {
    const uint8_t *data;   // column-major, 1 byte per column, bit0 = top row
    uint8_t width;         // glyph width in columns
    uint8_t height;        // glyph height in rows
    uint8_t first;         // ASCII code of data[0]
    uint8_t count;         // number of glyphs
    uint8_t advance;       // pen movement per char (width + inter-glyph gap)
};

// ---------------------------------------------------------------- enums

enum ZZMOrigin : uint8_t {   // where physical LED #0 sits
    ZZM_TOP_LEFT,
    ZZM_TOP_RIGHT,
    ZZM_BOTTOM_LEFT,
    ZZM_BOTTOM_RIGHT
};

enum ZZMDir : uint8_t {
    ZZM_LEFT, ZZM_RIGHT, ZZM_UP, ZZM_DOWN,
    ZZM_TOP    = ZZM_UP,      // readable aliases for swipe/wipe edge args
    ZZM_BOTTOM = ZZM_DOWN
};

enum ZZMColorMode : uint8_t {
    ZZM_SOLID,        // single colour
    ZZM_RAINBOW_X,    // hue sweeps across columns
    ZZM_RAINBOW_Y,    // hue sweeps down rows
    ZZM_RAINBOW_TIME, // whole glyph cycles hue over time
    ZZM_PALETTE_X     // user palette across columns
};

enum ZZMEase : uint8_t { ZZM_LINEAR, ZZM_EASE_OUT, ZZM_EASE_IN_OUT, ZZM_BACK_OUT };

enum ZZMAnim : uint8_t {
    ZZM_NONE,
    ZZM_STATIC,
    ZZM_SCROLL,       // continuous, any direction, optional looping
    ZZM_SWIPE_IN,     // text slides in from an edge and stops centred
    ZZM_SWIPE_OUT,    // text slides from centre off an edge
    ZZM_WIPE_IN,      // text revealed edge-to-edge, glyphs never move
    ZZM_WIPE_OUT,
    ZZM_FADE_IN,
    ZZM_FADE_OUT,
    ZZM_DISSOLVE_IN,  // pseudo-random per-pixel reveal
    ZZM_DISSOLVE_OUT,
    ZZM_TYPEWRITER,
    ZZM_BLINK,
    ZZM_BOUNCE        // ping-pong for text wider than the panel
};

// ---------------------------------------------------------------- class

class ZigZagMatrix {
public:
    ZigZagMatrix(CRGB *leds, uint8_t width, uint8_t height,
                 bool serpentine = true, ZZMOrigin origin = ZZM_TOP_LEFT);

    // ---- geometry -------------------------------------------------
    uint8_t  width()  const { return _w; }
    uint8_t  height() const { return _h; }
    uint16_t numLeds() const { return (uint16_t)_w * _h; }
    /* Logical (x,y) -> strip index. x right, y down, (0,0) top-left,
       independent of how the strip is physically wired. Returns 0xFFFF
       when off-panel. */
    uint16_t xy(int16_t x, int16_t y) const;

    // ---- raw drawing ----------------------------------------------
    void clear(const CRGB &c = CRGB::Black);
    void setPixel(int16_t x, int16_t y, const CRGB &c);
    CRGB getPixel(int16_t x, int16_t y) const;
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, const CRGB &c);
    void fadeAll(uint8_t amount);          // 0..255, 255 = to black
    void scaleAll(uint8_t scale);          // 0..255 brightness multiplier

    // ---- text -----------------------------------------------------
    void   setFont(const ZZMFont &f)   { _font = &f; }
    void   setTracking(int8_t px)      { _tracking = px; }   // extra px between glyphs
    int16_t textWidth(const char *s) const;
    /* Draws with the current colour mode. y is the top row of the glyph. */
    void   drawText(const char *s, int16_t x, int16_t y);
    void   drawChar(char c, int16_t x, int16_t y);

    // ---- colour ---------------------------------------------------
    void setColor(const CRGB &c)          { _fg = c; }
    void setBackground(const CRGB &c)     { _bg = c; }
    void setColorMode(ZZMColorMode m)     { _mode = m; }
    void setPalette(const CRGBPalette16 &p) { _pal = p; }
    void setHueSpread(uint8_t d)          { _hueSpread = d; }  // hue delta/pixel
    void setHueSpeed(uint8_t s)           { _hueSpeed = s; }   // hue delta/frame

    // ---- engine ---------------------------------------------------
    void setFrameRate(uint8_t fps)        { _framePeriod = fps ? 1000 / fps : 0; }
    void setAutoShow(bool on)             { _autoShow = on; }
    void setEasing(ZZMEase e)             { _ease = e; }
    /* Render the next frame if it is due. Returns true if the buffer
       changed (and was shown, when autoShow is on). */
    bool update();
    bool busy() const { return _anim != ZZM_NONE && !_done; }
    bool finished() const { return _done; }   // latches until next start*()
    void stop();
    void show();

    // ---- animations (all non-blocking) ----------------------------
    void showText(const char *s);                                   // static, centred
    void showText(const char *s, int16_t x, int16_t y);             // static, placed
    void scroll(const char *s, ZZMDir dir = ZZM_LEFT,
                uint16_t pxPerSec = 20, bool loop = true, uint8_t gap = 6);
    void swipeIn(const char *s, ZZMDir from, uint16_t ms = 600);
    void swipeOut(ZZMDir to, uint16_t ms = 600);
    void wipeIn(const char *s, ZZMDir from, uint16_t ms = 600);
    void wipeOut(ZZMDir to, uint16_t ms = 600);
    void fadeIn(const char *s, uint16_t ms = 600);
    void fadeOut(uint16_t ms = 600);
    void dissolveIn(const char *s, uint16_t ms = 800);
    void dissolveOut(uint16_t ms = 800);
    void typewriter(const char *s, uint16_t msPerChar = 120);
    void blink(const char *s, uint16_t onMs = 400, uint16_t offMs = 250,
               uint8_t times = 3);
    void bounce(const char *s, uint16_t pxPerSec = 15);

private:
    void     setText(const char *s);
    void     startAnim(ZZMAnim a, uint32_t ms);
    uint16_t progress() const;                    // 0..1000, eased
    static uint16_t applyEase(uint16_t p, ZZMEase e);
    CRGB     pixelColor(int16_t x, int16_t y, uint8_t glyphCol) const;
    void     renderStatic(int16_t x, int16_t y);
    void     maskDirectional(ZZMDir d, uint16_t p, bool keepLeading);
    void     maskDissolve(uint16_t p, bool reveal);
    int16_t  centerX() const;
    int16_t  centerY() const;
    static uint8_t hash8(uint8_t x, uint8_t y);

    CRGB       *_leds;
    uint8_t     _w, _h;
    bool        _serpentine;
    bool        _flipX, _flipY;

    const ZZMFont *_font;
    int8_t      _tracking = 0;

    CRGB        _fg = CRGB::White;
    CRGB        _bg = CRGB::Black;
    ZZMColorMode _mode = ZZM_SOLID;
    CRGBPalette16 _pal = RainbowColors_p;
    uint8_t     _hueSpread = 8;
    uint8_t     _hueSpeed  = 2;
    uint8_t     _hueBase   = 0;

    char        _text[ZZM_MAX_TEXT] = {0};
    int16_t     _textW = 0;

    ZZMAnim     _anim = ZZM_NONE;
    ZZMDir      _dir  = ZZM_LEFT;
    ZZMEase     _ease = ZZM_EASE_OUT;
    uint32_t    _t0   = 0;
    uint32_t    _duration = 0;   // ms
    bool        _loop = false;
    bool        _done = true;
    uint8_t     _gap = 6;
    uint16_t    _pxPerSec = 20;
    uint16_t    _onMs = 0, _offMs = 0;
    uint8_t     _times = 0;
    int16_t     _sx = 0, _sy = 0;      // static placement

    uint32_t    _lastFrame = 0;
    uint8_t     _framePeriod = 16;     // ~60 fps
    bool        _autoShow = true;
};

#include "ZigZagFonts.h"
