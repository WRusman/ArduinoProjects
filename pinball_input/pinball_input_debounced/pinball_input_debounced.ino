#include <Joystick.h>   // https://github.com/MHeironimus/ArduinoJoystickLibrary
#include <Adafruit_MPU6050.h>
#include <Wire.h>

// ---------------- Buttons ----------------
const uint8_t NUM_BUTTONS = 13;
const uint8_t inputPins[NUM_BUTTONS] = {4, 5, 6, 8, 9, 10, 14, 15, 16, 18, 19, 20, 21};
const uint8_t PRESS_LOCKOUT_MS = 5;    // after a reported release, ignore new presses this long

// Release must be stable this long before it is reported (per button).
// Filters contact dropouts while a button is held. Raise the launch button's
// entry if it still drops out (e.g. 30). Flippers: keep low for fast release.
const uint8_t releaseMs[NUM_BUTTONS] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

bool          btnState[NUM_BUTTONS];
unsigned long lastChange[NUM_BUTTONS];      // time of last reported state change
unsigned long lastPressedSeen[NUM_BUTTONS]; // last time the raw pin read "pressed"

// ---------------- Accelerometer ----------------
const uint8_t  MPU_ADDR          = 0x68;
const uint8_t  REG_ACCEL_XOUT_H  = 0x3B;
// Raw counts at +/-2g = 16384/g. 3340 counts ~= 2 m/s^2 (same sensitivity as the original map()).
const int32_t  NUDGE_FULLSCALE   = 3340;
const uint16_t ACCEL_INTERVAL_US = 1000; // MPU output rate is 1 kHz with DLPF enabled

Adafruit_MPU6050 mpu;
Joystick_ Joystick(0x03, 0x04, NUM_BUTTONS, 0,
                   true, true, false, false, false, false,
                   false, false, false, false, false);

int16_t       xBase, yBase;
int16_t       lastXOut = 0, lastYOut = 0;
unsigned long lastAccelRead = 0;

// Read only ACCEL_X and ACCEL_Y (4 bytes) instead of the 14-byte getEvent() burst.
bool readAccel(int16_t &ax, int16_t &ay) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(REG_ACCEL_XOUT_H);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(MPU_ADDR, (uint8_t)4) != 4) return false;
  uint8_t xh = Wire.read();          // separate reads: evaluation order of
  uint8_t xl = Wire.read();          // (read()<<8)|read() is unspecified in C++
  uint8_t yh = Wire.read();
  uint8_t yl = Wire.read();
  ax = (int16_t)((xh << 8) | xl);
  ay = (int16_t)((yh << 8) | yl);
  return true;
}

int16_t scaleAxis(int32_t delta) {
  int32_t v = -delta * 512 / NUDGE_FULLSCALE;   // inverted, as in the original map(..., 512, -512)
  return (int16_t)constrain(v, -512, 512);
}

void setup() {
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
    btnState[i]   = false;
    lastChange[i] = 0;
    lastPressedSeen[i] = 0;
  }

  mpu.begin();
  Wire.setClock(400000);              // after mpu.begin(), which re-inits Wire at 100 kHz
#if defined(WIRE_HAS_TIMEOUT)
  Wire.setWireTimeout(3000, true);    // a glitched I2C bus must never freeze the flippers
#endif
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);

  Joystick.begin(false);              // no auto-send: one HID report per loop, only when changed
  Joystick.setXAxisRange(-512, 512);
  Joystick.setYAxisRange(-512, 512);
  Joystick.setXAxis(0);
  Joystick.setYAxis(0);
  Joystick.sendState();

  // Baseline: let the sensor settle, then average 64 samples.
  delay(100);
  int32_t sx = 0, sy = 0;
  uint8_t n = 0;
  for (uint8_t i = 0; i < 64; i++) {
    int16_t ax, ay;
    if (readAccel(ax, ay)) { sx += ax; sy += ay; n++; }
    delay(2);
  }
  xBase = n ? sx / n : 0;
  yBase = n ? sy / n : 0;
}

void loop() {
  bool dirty = false;

  // Buttons: asymmetric debounce.
  //  - press:   reported on the first edge (zero latency)
  //  - release: reported only after the pin has read "released" continuously
  //             for releaseMs[i], so dropouts while holding are ignored
  unsigned long nowMs = millis();
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    bool raw = !digitalRead(inputPins[i]);
    if (raw) lastPressedSeen[i] = nowMs;

    if (!btnState[i]) {
      if (raw && (nowMs - lastChange[i]) >= PRESS_LOCKOUT_MS) {
        btnState[i] = true;
        lastChange[i] = nowMs;
        Joystick.setButton(i, 1);
        dirty = true;
      }
    } else {
      if (!raw && (nowMs - lastPressedSeen[i]) >= releaseMs[i]) {
        btnState[i] = false;
        lastChange[i] = nowMs;
        Joystick.setButton(i, 0);
        dirty = true;
      }
    }
  }

  // Accelerometer: rate-limited so button polling stays tight.
  unsigned long nowUs = micros();
  if (nowUs - lastAccelRead >= ACCEL_INTERVAL_US) {
    lastAccelRead = nowUs;
    int16_t ax, ay;
    if (readAccel(ax, ay)) {
      int16_t xOut = scaleAxis((int32_t)ay - yBase);   // X axis <- sensor Y (as original)
      int16_t yOut = scaleAxis((int32_t)ax - xBase);   // Y axis <- sensor X
      if (xOut != lastXOut) { Joystick.setXAxis(xOut); lastXOut = xOut; dirty = true; }
      if (yOut != lastYOut) { Joystick.setYAxis(yOut); lastYOut = yOut; dirty = true; }
    }
  }

  if (dirty) Joystick.sendState();
}
