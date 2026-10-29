#include "../src/ZigZagMatrix.h"
#include <cstdio>
#include <string>

uint32_t g_millis = 0;
FastLEDStub FastLED;

static const uint8_t W = 40, H = 10;
CRGB leds[W * H];
ZigZagMatrix m(leds, W, H);

static void dump(const char *label) {
    printf("--- %s (t=%ums) ---\n", label, g_millis);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) putchar(m.getPixel(x, y).lit() ? '#' : '.');
        putchar('\n');
    }
}

static void run(uint32_t ms, uint32_t step, const char *label, uint32_t every) {
    uint32_t start = g_millis, next = 0;
    while (g_millis - start <= ms) {
        m.update();
        if (g_millis - start >= next) { dump(label); next += every; }
        g_millis += step;
    }
}

int main() {
    // --- wiring sanity: index 0..3 and the row-1 reversal --------------
    printf("xy(0,0)=%u  xy(39,0)=%u  xy(0,1)=%u  xy(39,1)=%u  xy(0,9)=%u\n",
           m.xy(0, 0), m.xy(39, 0), m.xy(0, 1), m.xy(39, 1), m.xy(0, 9));
    printf("off-panel xy(40,0)=%u\n\n", m.xy(40, 0));

    // every strip index must be hit exactly once
    bool seen[W * H] = {false};
    bool ok = true;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) {
            uint16_t i = m.xy(x, y);
            if (i >= W * H || seen[i]) ok = false;
            seen[i] = true;
        }
    for (int i = 0; i < W * H; i++) if (!seen[i]) ok = false;
    printf("mapping is a bijection over 0..399: %s\n\n", ok ? "YES" : "NO");

    printf("textWidth(\"HELLO\") = %d\n", m.textWidth("HELLO"));
    printf("textWidth(\"A\")     = %d\n\n", m.textWidth("A"));

    m.setColor(CRGB::White);

    m.showText("HELLO");
    m.update();
    dump("static centred");

    m.swipeIn("ABC", ZZM_LEFT, 600);
    run(600, 50, "swipeIn from LEFT", 200);

    m.swipeIn("ABC", ZZM_DOWN, 600);
    run(600, 50, "swipeIn from BOTTOM", 300);

    m.swipeOut(ZZM_UP, 400);
    run(400, 50, "swipeOut to TOP", 200);

    m.wipeIn("WIPE", ZZM_LEFT, 500);
    run(500, 50, "wipeIn from LEFT", 250);

    m.scroll("SCROLLING TEXT", ZZM_LEFT, 40, true, 6);
    run(2000, 40, "scroll left (loop)", 900);

    m.typewriter("TYPE", 120);
    run(500, 50, "typewriter", 240);

    m.dissolveIn("DISS", 500);
    run(500, 50, "dissolveIn", 240);

    m.setFont(ZZM_FONT_3X5);
    m.showText("TINY 3X5", 0, 0);
    m.update();
    dump("3x5 font, two-line capable");

    return 0;
}
