#define LED_PIN 7

int period = 100;   
int duty = 0;

void set_period(int p) {
  period = p;
}

void set_duty(int d) {
  duty = d;
}

void pwm() {
  if (duty == 0) {
    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(period);
  }
  else if (duty == 100) {
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(period);
  }
  else {
    int onTime = period * duty / 100;
    int offTime = period - onTime;

    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(onTime);

    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(offTime);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {

  set_period(100);

  for (int d = 0; d <= 100; d++) {
    set_duty(d);

    unsigned long start = micros();

    while (micros() - start < 5000) {
      pwm();
    }
  }

  for (int d = 100; d >= 0; d--) {
    set_duty(d);

    unsigned long start = micros();

    while (micros() - start < 5000) {
      pwm();
    }
  }
}
