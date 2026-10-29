/*
 * ZigZagMatrix - basic example for a 40 x 10 serpentine WS2812 panel.
 */
#include <FastLED.h>
#include <ZigZagMatrix.h>

#define DATA_PIN   5
#define MATRIX_W   40
#define MATRIX_H   10
#define NUM_LEDS   (MATRIX_W * MATRIX_H)

CRGB leds[NUM_LEDS];

// If your first LED is not the top-left one, change ZZM_TOP_LEFT.
ZigZagMatrix matrix(leds, MATRIX_W, MATRIX_H, true, ZZM_TOP_LEFT);

void setup() {
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

    // 400 pixels at full white is ~24 A. Let FastLED police the budget:
    // it scales global brightness down instead of browning out your supply.
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
    FastLED.setBrightness(60);

    matrix.setFont(ZZM_FONT_5X7);
    matrix.setColor(CRGB::Cyan);
    matrix.setFrameRate(60);

    matrix.scroll("HELLO FROM THE ZIG-ZAG PANEL  ", ZZM_LEFT, 25);
}

void loop() {
    matrix.update();   // non-blocking; calls FastLED.show() when it redraws
}
