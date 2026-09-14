#define PIN_LED 13

unsigned int count, toggle;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);

  while (!Serial) {
  }

  Serial.println("Hello World!");
  count = 0;
  toggle = 0;
  digitalWrite(PIN_LED, toggle); 
}

void loop() {
  Serial.println(count);
  toggle = !toggle;
  digitalWrite(PIN_LED, toggle);
  count++;
  delay(1000);
}

int toggle_state(int toggle) {
  return toggle;
}
