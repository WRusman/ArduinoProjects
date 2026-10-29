// Host-side stub of the bits of Arduino.h + FastLED.h that the library uses.
#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>

#define PROGMEM
#define pgm_read_byte(a) (*(const uint8_t *)(a))

extern uint32_t g_millis;
inline uint32_t millis() { return g_millis; }

struct CRGB {
    uint8_t r = 0, g = 0, b = 0;
    enum HTMLColorCode { Black = 0, White = 1, Red = 2, Green = 3, Blue = 4 };
    CRGB() {}
    CRGB(uint8_t rr, uint8_t gg, uint8_t bb) : r(rr), g(gg), b(bb) {}
    CRGB(HTMLColorCode c) {
        switch (c) {
            case White: r = g = b = 255; break;
            case Red:   r = 255; break;
            case Green: g = 255; break;
            case Blue:  b = 255; break;
            default: break;
        }
    }
    bool lit() const { return r || g || b; }
};

struct CHSV {
    uint8_t h, s, v;
    CHSV(uint8_t hh, uint8_t ss, uint8_t vv) : h(hh), s(ss), v(vv) {}
    operator CRGB() const { return CRGB(v, (uint8_t)(v / 2), (uint8_t)(h)); }
};

struct CRGBPalette16 { int dummy = 0; };
static CRGBPalette16 RainbowColors_p;
inline CRGB ColorFromPalette(const CRGBPalette16 &, uint8_t i) {
    return CRGB(i, (uint8_t)(255 - i), 128);
}

inline void fill_solid(CRGB *a, uint16_t n, const CRGB &c) {
    for (uint16_t i = 0; i < n; i++) a[i] = c;
}
inline void fadeToBlackBy(CRGB *a, uint16_t n, uint8_t amt) {
    for (uint16_t i = 0; i < n; i++) {
        a[i].r = (uint8_t)((a[i].r * (255 - amt)) / 255);
        a[i].g = (uint8_t)((a[i].g * (255 - amt)) / 255);
        a[i].b = (uint8_t)((a[i].b * (255 - amt)) / 255);
    }
}
inline void nscale8(CRGB *a, uint16_t n, uint8_t s) {
    for (uint16_t i = 0; i < n; i++) {
        a[i].r = (uint8_t)((a[i].r * s) / 255);
        a[i].g = (uint8_t)((a[i].g * s) / 255);
        a[i].b = (uint8_t)((a[i].b * s) / 255);
    }
}

struct FastLEDStub { int showCount = 0; void show() { showCount++; } };
extern FastLEDStub FastLED;
