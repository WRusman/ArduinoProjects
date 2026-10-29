#include <Joystick.h>   // https://github.com/MHeironimus/ArduinoJoystickLibrary/wiki
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

//https://raw.githubusercontent.com/sparkfun/Arduino_Boards/main/IDE_Board_Manager/package_sparkfun_index.json

// ---------------- configuration ----------------
const uint8_t  NUM_BUTTONS       = 13;
const uint8_t  inputPins[NUM_BUTTONS] = {4,5,6,8,9,10,14,15,16,18,19,20,21};
const uint8_t  outButton[NUM_BUTTONS] = {0,1,2,3,4,5,6,7,8,9,10,11,12};

const uint8_t  LOCKOUT_MS        = 5;     // after an accepted edge, ignore that input for this long
const uint16_t ACCEL_INTERVAL_US = 2000;  // read MPU every 2 ms (500 Hz) - plenty for 94 Hz DLPF
const float    NUDGE_RANGE       = 2.0f;  // m/s^2 that maps to full axis deflection (+-512)
const uint8_t  BASELINE_SAMPLES  = 50;

// ---------------- globals ----------------
Adafruit_MPU6050 mpu;
bool mpuOk = false;

Joystick_ Joystick(0x03, 0x04, NUM_BUTTONS, 0,
                   true, true, false, false, false, false,
                   false, false, false, false, false);

float xBaseline = 0, yBaseline = 0;
bool          btnState[NUM_BUTTONS];
unsigned long lastChange[NUM_BUTTONS];
int16_t       lastXAxis = 0, lastYAxis = 0;
unsigned long lastAccelRead = 0;

static int16_t scaleAxis(float v) {
  float s = -v * (512.0f / NUDGE_RANGE);          // inverted, same as original map(..., 512, -512)
  return (int16_t)constrain(s, -512.0f, 512.0f);
}

void setup() {
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
    btnState[i]   = false;
    lastChange[i] = 0;
  }

  mpuOk = mpu.begin();
  if (mpuOk) {
    Wire.setClock(400000);                         // after begin(), which resets I2C to 100 kHz
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);
    delay(100);                                    // let the sensor settle

    sensors_event_t a, g, t;
    for (uint8_t i = 0; i < BASELINE_SAMPLES; i++) {
      mpu.getEvent(&a, &g, &t);
      xBaseline += a.acceleration.x;
      yBaseline += a.acceleration.y;
      delay(5);                                    // get fresh samples, not the same one 10x
    }
    xBaseline /= BASELINE_SAMPLES;
    yBaseline /= BASELINE_SAMPLES;
  }

  Joystick.begin(false);                           // false = no auto-send; we call sendState()
  Joystick.setXAxisRange(-512, 512);
  Joystick.setYAxisRange(-512, 512);
  Joystick.setXAxis(0);
  Joystick.setYAxis(0);
  Joystick.sendState();
}

void loop() {
  bool dirty = false;

  // ---- buttons: leading-edge debounce (report first edge instantly, then lock out bounce) ----
  unsigned long now = millis();
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    bool raw = !digitalRead(inputPins[i]);
    if (raw != btnState[i] && (now - lastChange[i]) >= LOCKOUT_MS) {
      btnState[i]   = raw;
      lastChange[i] = now;
      Joystick.setButton(outButton[i], raw);
      dirty = true;
    }
  }

  // ---- accelerometer: fixed rate, only flag changes ----
  unsigned long nowUs = micros();
  if (mpuOk && (nowUs - lastAccelRead) >= ACCEL_INTERVAL_US) {
    lastAccelRead = nowUs;
    sensors_event_t a, g, t;
    mpu.getEvent(&a, &g, &t);

    int16_t xAxis = scaleAxis(a.acceleration.y - yBaseline);   // axes swapped as in original
    int16_t yAxis = scaleAxis(a.acceleration.x - xBaseline);

    if (xAxis != lastXAxis) { Joystick.setXAxis(xAxis); lastXAxis = xAxis; dirty = true; }
    if (yAxis != lastYAxis) { Joystick.setYAxis(yAxis); lastYAxis = yAxis; dirty = true; }
  }

  // ---- one HID report per loop, only when something changed ----
  if (dirty) Joystick.sendState();
}
