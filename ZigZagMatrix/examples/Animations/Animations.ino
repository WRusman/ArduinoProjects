/*
 * ZigZagMatrix - cycles through every transition.
 *
 * Shows the intended chaining pattern: the library runs one animation at a
 * time and latches finished(); the sketch owns the sequence.
 */
#include <FastLED.h>
#include <ZigZagMatrix.h>

#define DATA_PIN 5
#define MATRIX_W 40
#define MATRIX_H 10
#define NUM_LEDS (MATRIX_W * MATRIX_H)

CRGB leds[NUM_LEDS];
ZigZagMatrix matrix(leds, MATRIX_W, MATRIX_H, true, ZZM_TOP_LEFT);

uint8_t  step     = 0;
uint32_t holdUntil = 0;

void nextStep() {
    switch (step) {
        case 0:  matrix.setColorMode(ZZM_SOLID);
                 matrix.setColor(CRGB::White);
                 matrix.swipeIn("LEFT", ZZM_LEFT, 500);            break;
        case 1:  matrix.swipeOut(ZZM_RIGHT, 400);                  break;
        case 2:  matrix.swipeIn("TOP", ZZM_TOP, 500);              break;
        case 3:  matrix.swipeOut(ZZM_BOTTOM, 400);                 break;
        case 4:  matrix.setEasing(ZZM_BACK_OUT);
                 matrix.swipeIn("BOING", ZZM_BOTTOM, 700);         break;
        case 5:  matrix.setEasing(ZZM_EASE_OUT);
                 matrix.wipeOut(ZZM_LEFT, 400);                    break;
        case 6:  matrix.setColorMode(ZZM_RAINBOW_X);
                 matrix.wipeIn("WIPE", ZZM_RIGHT, 500);            break;
        case 7:  matrix.dissolveOut(600);                          break;
        case 8:  matrix.setColorMode(ZZM_SOLID);
                 matrix.setColor(CRGB::Orange);
                 matrix.typewriter("TYPE.", 140);                  break;
        case 9:  matrix.fadeOut(500);                              break;
        case 10: matrix.setColor(CRGB::Red);
                 matrix.blink("ALERT", 300, 200, 4);               break;
        case 11: matrix.setColorMode(ZZM_RAINBOW_TIME);
                 matrix.scroll("SCROLLING MARQUEE", ZZM_LEFT, 30,
                               false, MATRIX_W);                   break;
        case 12: matrix.setColorMode(ZZM_SOLID);
                 matrix.setColor(CRGB::Green);
                 matrix.dissolveIn("DONE", 700);                   break;
    }
    step = (step + 1) % 13;
}

void setup() {
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
    FastLED.setBrightness(60);
    matrix.setFrameRate(60);
    nextStep();
}

void loop() {
    matrix.update();

    // Advance when the current animation has ended and its hold time is up.
    if (matrix.finished()) {
        if (!holdUntil) holdUntil = millis() + 500;
        else if (millis() >= holdUntil) { holdUntil = 0; nextStep(); }
    }
}
