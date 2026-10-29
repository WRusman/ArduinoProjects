# ZigZagMatrix

Text and animation engine for serpentine-wired WS2812 matrices, on top of FastLED.
Defaults are tuned for a 40 × 10 panel.

## Design decisions

**No second frame buffer.** The `CRGB` array you pass to FastLED already *is* the
back buffer — nothing reaches the strip until `FastLED.show()`, which is
synchronous and blocks until the last bit is clocked out. A shadow buffer would
cost another 1200 bytes and buy nothing. Every frame is composed from scratch:
`clear → draw text → optional mask pass → show`. For 400 pixels that is a few
hundred microseconds on an ESP32, well inside a 16 ms frame budget.

**Masks are a post-pass, not a clipping mode.** Wipe and dissolve render the text
normally and then clear the pixels that should not be visible yet. One extra pass
over 400 pixels is cheaper than threading mask state through the glyph blitter,
and it composes with any colour mode for free.

**No offscreen text canvas.** `drawText()` clips per column, so scrolling and
swiping are just changing `x`/`y`. A 40-char string never needs a 240 px wide
scratch bitmap.

**Dissolve uses a hash, not a shuffle.** A permutation table for 400 pixels is
800 bytes of RAM. `hash8(x, y) < threshold` gives the same stable random-looking
reveal order with zero state.

**Fixed-point everywhere.** Progress is 0…1000 integer, easings are integer
polynomials, scroll position is `elapsed × pxPerSec / 1000` recomputed from the
start time (no accumulator, so no drift).

RAM cost of the library itself: `ZZM_MAX_TEXT` (96 B default) plus ~40 B of state.
Fonts live in PROGMEM (475 B + 285 B).

## Wiring / orientation

```cpp
ZigZagMatrix matrix(leds, 40, 10, /*serpentine=*/true, ZZM_TOP_LEFT);
```

`xy(x, y)` maps logical coordinates (x right, y down, origin top-left) to a strip
index regardless of wiring. The last two arguments describe the *physical* strip:
`serpentine` = every other row runs backwards, and the origin corner is where LED
#0 sits. If your text comes out mirrored or upside down, change the origin; if
rows alternate direction incorrectly, flip `serpentine`.

## API

### Drawing
```cpp
matrix.clear();
matrix.setPixel(x, y, CRGB::Red);
matrix.fillRect(x, y, w, h, colour);
matrix.fadeAll(40);          // trail effects
matrix.drawText("HI", x, y); // y = top row of the glyph
matrix.textWidth("HI");
```

### Fonts
| Font | Size | Notes |
|---|---|---|
| `ZZM_FONT_5X7` | 5 × 7, 6 px advance | default, 6 chars per 40 px row |
| `ZZM_FONT_3X5` | 3 × 5, 4 px advance | 10 chars/row, two lines fit in 10 rows |

`setTracking(px)` adds or removes inter-glyph space. The 3×5 font folds lowercase
to uppercase.

### Colour
```cpp
matrix.setColor(CRGB::Cyan);
matrix.setColorMode(ZZM_RAINBOW_X);   // SOLID, RAINBOW_X/_Y/_TIME, PALETTE_X
matrix.setHueSpread(8);               // hue delta per pixel
matrix.setHueSpeed(2);                // hue delta per frame
matrix.setPalette(HeatColors_p);
```

### Animations
All are non-blocking. Start one, then call `update()` in `loop()`.

```cpp
matrix.showText("HI");                              // static, centred
matrix.scroll(s, ZZM_LEFT, 25, /*loop=*/true, gap); // any direction
matrix.swipeIn(s, ZZM_LEFT, 600);   // glyphs slide in from an edge
matrix.swipeOut(ZZM_TOP, 400);      // ...and out through an edge
matrix.wipeIn(s, ZZM_LEFT, 500);    // revealed edge-to-edge, glyphs stay put
matrix.wipeOut(ZZM_BOTTOM, 500);
matrix.fadeIn(s, 600);  matrix.fadeOut(600);
matrix.dissolveIn(s, 800);  matrix.dissolveOut(800);
matrix.typewriter(s, 120);          // ms per char, with caret
matrix.blink(s, 400, 250, 3);       // onMs, offMs, repeats
matrix.bounce(s, 15);               // ping-pong for text wider than the panel
```

Direction constants: `ZZM_LEFT`, `ZZM_RIGHT`, `ZZM_TOP` (= `ZZM_UP`),
`ZZM_BOTTOM` (= `ZZM_DOWN`). For swipe/wipe the argument names the **edge**:
`swipeIn(s, ZZM_LEFT)` enters from the left, `swipeOut(ZZM_TOP)` leaves upward.

Easing applies to swipe/wipe/fade/dissolve: `ZZM_LINEAR`, `ZZM_EASE_OUT`
(default), `ZZM_EASE_IN_OUT`, `ZZM_BACK_OUT` (overshoots ~10 %, then settles).

### Engine
```cpp
matrix.setFrameRate(60);   // update() rate-limits itself
matrix.setAutoShow(true);  // false if you drive FastLED.show() yourself
matrix.update();           // true when it actually redrew
matrix.finished();         // latches when a one-shot ends -> chain the next one
matrix.busy();
```

`update()` stops redrawing once a one-shot animation finishes, so an idle static
message costs nothing per loop (unless the colour mode is `ZZM_RAINBOW_TIME`).

## Scroll gap semantics

`gap` is the blank distance between the end of one repetition and the start of
the next. With the default 6 px, a long string is already re-entering on the left
before its tail has left on the right — a tight marquee. For a clean, fully blank
frame between passes, pass `gap >= width()`.

## Power

400 WS2812s at full white draw about 24 A. The examples cap it with
`FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000)`, which makes FastLED scale
global brightness to stay inside the budget instead of browning out the supply
mid-frame. Inject 5 V at both ends of a 40-LED row; a single feed at one end will
show visible red shift by the far end.

## Regenerating the fonts

`tools/gen_font.py` holds the glyphs as ASCII art and emits
`src/ZigZagFonts.h` (column-major, bit 0 = top row). Edit the art, rerun it.

## Host-side testing

`test/` contains stubs for `Arduino.h` and the FastLED calls used here, plus a
simulator that prints frames as ASCII:

```
g++ -std=c++11 -I test -o sim test/sim.cpp src/ZigZagMatrix.cpp && ./sim
```

It verifies that `xy()` is a bijection over 0…399 and dumps frames from every
animation — useful for checking a new effect without reflashing.
