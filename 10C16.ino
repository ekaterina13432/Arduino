
#include <Servo.h>
#include <math.h>

#define PIN_SERVO 10
#define PIN_TRIG  12
#define PIN_ECHO  13

// 1 = Sigmoid, 2 = Smoothstep
#define MOVEMENT_FUNCTION 1

const int CLOSED_ANGLE = 0;
const int OPEN_ANGLE = 90;

const float OPEN_DISTANCE = 20.0;
const float CLOSE_DISTANCE = 25.0;

const unsigned long MOVING_TIME = 2000;
const unsigned long INTERVAL = 20;

Servo myServo;

int currentAngle = CLOSED_ANGLE;
int startAngle = CLOSED_ANGLE;
int targetAngle = CLOSED_ANGLE;

bool barrierOpen = false;

unsigned long moveStartTime = 0;
unsigned long lastUpdateTime = 0;
unsigned long lastMeasureTime = 0;

float distance = -1.0;

// Measure distance using the ultrasonic sensor
float measureDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_ECHO, HIGH, 30000);

  if (duration == 0) {
    return -1.0;
  }

  return duration * 0.0343 / 2.0;
}

// Sigmoid function
float sigmoid(float t) {
  float s = 1.0 / (1.0 + exp(-12.0 * (t - 0.5)));
  float s0 = 1.0 / (1.0 + exp(6.0));
  float s1 = 1.0 / (1.0 + exp(-6.0));

  return (s - s0) / (s1 - s0);
}

// Smoothstep function
float smoothstep(float t) {
  return t * t * (3.0 - 2.0 * t);
}

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myServo.attach(PIN_SERVO);
  myServo.write(CLOSED_ANGLE);

  Serial.begin(57600);

  moveStartTime = millis();
}

void loop() {
  unsigned long now = millis();

  // Measure distance every 50 ms
  if (now - lastMeasureTime >= 50) {
    lastMeasureTime = now;

    float measuredDistance = measureDistance();

    // Ignore failed measurements
    if (measuredDistance >= 0) {
      distance = measuredDistance;

      // Detect a vehicle
      if (distance < OPEN_DISTANCE) {
        barrierOpen = true;
      }
      else if (distance > CLOSE_DISTANCE) {
        barrierOpen = false;
      }
    }
  }

  // Determine the target angle
  int newTarget = barrierOpen ? OPEN_ANGLE : CLOSED_ANGLE;

  // Start moving when the target changes
  if (newTarget != targetAngle) {
    startAngle = currentAngle;
    targetAngle = newTarget;
    moveStartTime = now;
  }

  // Update servo position every 20 ms
  if (now - lastUpdateTime >= INTERVAL) {
    lastUpdateTime = now;

    float t = (float)(now - moveStartTime)
              / MOVING_TIME;

    if (t > 1.0) {
      t = 1.0;
    }

    float progress;

    #if MOVEMENT_FUNCTION == 1
      progress = sigmoid(t);
    #else
      progress = smoothstep(t);
    #endif

    currentAngle = startAngle
        + (int)((targetAngle - startAngle) * progress);

    myServo.write(currentAngle);

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm, Angle: ");
    Serial.println(currentAngle);
  }
}
