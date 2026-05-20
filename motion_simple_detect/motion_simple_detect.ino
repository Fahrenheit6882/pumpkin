const int PIR_PIN = 5;
const int LED_PIN = LED_BUILTIN;

void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int state = digitalRead(PIR_PIN);
  Serial.println(state); // Add Serial.begin(9600) to setup()
  if (state == HIGH) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  delay(500);
}
