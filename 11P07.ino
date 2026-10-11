
#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   9
#define PIN_TRIG  12
#define PIN_ECHO  13
#define PIN_SERVO 10

// Sonar parameters
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 180.0
#define _DIST_MAX 360.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.3

// Original servo settings (not used for angle control below)
#define _DUTY_MIN 1000
#define _DUTY_NEU 1500
#define _DUTY_MAX 2000

// Global variables
float dist_ema = _DIST_MIN;
float dist_prev = _DIST_MIN;

unsigned long last_sampling_time = 0;

Servo myservo;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);  // LED off (active-low)

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO);
  myservo.write(0);

  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;

  // Sampling interval
  if (millis() < last_sampling_time + INTERVAL) {
    return;
  }

  // Measure distance
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // Range filter: keep the previous valid reading
  if ((dist_raw == 0.0) ||
      (dist_raw > _DIST_MAX) ||
      (dist_raw < _DIST_MIN)) {
    dist_filtered = dist_prev;
  } else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  }

  // EMA smoothing
  dist_ema = _EMA_ALPHA * dist_filtered
           + (1.0 - _EMA_ALPHA) * dist_ema;

  // Servo angle: 0 to 180 degrees across 18–36 cm
  int angle;

  if (dist_ema <= _DIST_MIN) {
    angle = 0;
  } else if (dist_ema >= _DIST_MAX) {
    angle = 180;
  } else {
    angle = (int)(
      (dist_ema - _DIST_MIN) * 180.0
      / (_DIST_MAX - _DIST_MIN)
    );
  }

  myservo.write(angle);

  // LED on inside the range, off outside
  // Active-low logic, matching the observed wiring behavior
 if (dist_raw >= _DIST_MIN &&
    dist_raw <= _DIST_MAX &&
    dist_raw != 0.0) {
  digitalWrite(PIN_LED, LOW);
} else {
  digitalWrite(PIN_LED, HIGH);
}

  // Serial Monitor / Serial Plotter
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",dist:");
  Serial.print(dist_raw);

  Serial.print(",ema:");
  Serial.print(dist_ema);

  Serial.print(",Servo:");
  Serial.print(myservo.read());

  Serial.print(",Max:");
  Serial.println(_DIST_MAX);

  last_sampling_time += INTERVAL;
}

// Get distance in millimeters
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
