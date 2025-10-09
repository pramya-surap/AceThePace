//ideally errors go away when connected to board

void setup() {
  Serial.begin(9600);
  Serial.println("Hello from Arduino!");
}

void loop() {
  Serial.println("Loop running...");
  delay(1000);
}