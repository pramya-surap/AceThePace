//Upload this code to the arduino first(Can do through vscode)


const int ledPin = 9; // use a PWM-capable pin (e.g. 3, 5, 6, 9, 10, 11)

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  Serial.println("Arduino ready — send brightness (0-255)");
}

void loop() {
  if (Serial.available()) {
    int brightness = Serial.parseInt(); // read number sent from Python
    if (brightness >= 0 && brightness <= 255) {
      analogWrite(ledPin, brightness);
      Serial.print("Set brightness to: ");
      Serial.println(brightness);
    }
    // clear input buffer if user typed text
    while (Serial.available()) Serial.read();
  }
}