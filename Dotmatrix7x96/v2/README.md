# LedMatrix7

ESP32 driver for old LED matrix panels with **7 GPIO-selected rows** and
**96 columns fed by 12 daisy-chained 8-bit shift registers** (single serial
input). Supports horizontal daisy-chaining of multiple panels, a
double-buffered framebuffer, a 5x7 font, marquee scrolling and slide-in
transitions.

## Why it works the way it does

The column shift-register **storage latch is not controllable** (transparent /
hardwired enabled), so there is no "shift in the dark, then latch" trick
available. Instead:

- **`/OE` is used as a global blank.** Each row period: blank → drop the old
  row → stream the new column bits over SPI → (optional latch pulse) → enable
  the new row → unblank. The blank window hides the shifting artifacts that
  would otherwise be visible and prevents row-to-row ghosting.
- **Software multiplexing from a RAM framebuffer.** A periodic `esp_timer`
  (task-dispatch, so SPI is safe inside it and the CPU is free between ticks)
  scans one row each tick. Whole-panel rate defaults to 120 Hz → ~1.19 ms per
  row.
- **Double buffer.** You draw into the back buffer; `show()` requests a swap
  that the scanner performs only at a frame boundary, so animations never tear.
- **Brightness** is global on-time modulation: a one-shot timer re-blanks part
  way through each row slot (`255` = full on, no extra blank).

## Wiring (defaults)

| Signal | Default pin | Notes |
|---|---|---|
| SPI data (MOSI) | 23 | → register serial in |
| SPI clock (SCLK) | 18 | → register clock |
| /OE | 4 | blank; active-low by default |
| Strobe/latch | -1 | set to a GPIO only if your latch is controllable |
| Rows 0..6 | 32,33,25,26,27,14,13 | top → bottom |

No SPI chip-select is used. Avoid strapping pins (0, 2, 12, 15) and the
input-only pins (34–39) for outputs.

### Daisy-chaining
Chain the **column serial out → serial in** of each panel left to right and set
`numModules`. Wire the **7 row-enable lines in parallel** across all panels so
one set of 7 GPIOs drives the whole wide display. Width becomes `96 * numModules`.

## Bring-up (set the orientation flags once)

Polarity and wiring order can't be guessed, so tune four booleans with a test
pattern (see the commented block in `examples/Basic`):

1. `setPixel(0,0)` should light **only the top-left** LED.
   - Wrong corner horizontally → toggle `flipX`.
   - Wrong corner vertically → toggle `flipY`.
2. `drawText(0,0,"01")` should read **left to right**.
   - Columns mirrored within a byte → switch `bitOrder` (MSBFIRST/LSBFIRST).
   - 8-pixel groups in the wrong order within a panel → toggle `byteReverse`.
   - For `numModules > 1`, panels in the wrong order → toggle `moduleReverse`.
3. Row appears but display stays dark / always on → `oeActiveHigh`.
4. Rows inverted/no select → `rowActiveHigh`.

If your panel actually *does* expose a usable latch, set `strobePin` and the
driver will pulse it after each row load (it still uses `/OE` for ghost-free
row transitions).

## API

```cpp
LedMatrix7 matrix;
LedMatrix7::Config cfg;            // edit fields, then:
matrix.begin(cfg);

matrix.clear();
matrix.setPixel(x, y, true);
matrix.fillRect(x, y, w, h, true);
matrix.drawText(0, 0, "HELLO");
matrix.show();                     // push the back buffer

matrix.writeText("HELLO");         // static: clear + center + show, no animation
matrix.writeText("LEFT", false);   // left-aligned instead of centered
matrix.clearNow();                 // instant blank (clearNow(true) fills)

matrix.setBrightness(180);         // 0..255

matrix.scrollText("MARQUEE  ", 40);                          // R->L
matrix.scrollText("MARQUEE  ", 40, /*leftToRight=*/true);    // L->R
matrix.slideInText("HI", LedMatrix7::FROM_LEFT,   60, 800);  // + FROM_RIGHT
matrix.slideInText("HI", LedMatrix7::FROM_BOTTOM, 30, 800);  // + FROM_TOP

// animated clear: slide whatever is on screen off toward an edge, end blank
matrix.clearOut(LedMatrix7::FROM_LEFT, 60);                  // + RIGHT/TOP/BOTTOM
```

`scrollText` / `slideInText` / `clearOut` are blocking; `writeText` / `clearNow`
return immediately. For `clearOut`, `SlideDir` is the edge the content departs
toward (so `slideInText(FROM_LEFT, ...)` then `clearOut(FROM_RIGHT, ...)` reads
as enter-from-left, exit-to-the-right).

## Notes / limits

- Monochrome, 1 bit/pixel. Framebuffer is `7 * 12 * numModules` bytes, doubled.
- The 7-row height matches a 5x7 font exactly — text fills the panel cleanly.
- Built-in font covers ASCII 0x20–0x7E; swap `font5x7.h` for any same-layout
  table (column-major, bit0 = top row).
- Targets the arduino-esp32 core on the original ESP32 (WROOM / DevKit V1).
  `VSPI`/`HSPI` macros are ESP32-classic; adjust `spiHost` on other variants.
- SPI default is 8 MHz; lower it if older registers (or long chains) glitch.
