#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define pin 9
#define count 11

Adafruit_NeoPixel strip(count, pin, NEO_GRB + NEO_KHZ800);

// Variables to store the last update times for each color
unsigned long prevRed = 0;
unsigned long prevGreen = 0;
unsigned long prevBlue = 0;

// Delay times (in ms) — to be set by user via serial
int redDelay;
int greenDelay;
int blueDelay;

// Index positions of the active LED for each runner
int red = 0;
int green = 0;
int blue = 0;

const int MIN_DELAY = 100;
const int MAX_DELAY = 999;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);
  strip.begin();
  strip.setBrightness(255);
  strip.show();
  
  Serial.println("Ready. Send three delay values (100–999 ms) separated by spaces:");
}

void tryParseAndSetDelays(const String &line) {
  int r, g, b;
  char buf[64];
  line.toCharArray(buf, sizeof(buf));
  
  int n = sscanf(buf, "%d %d %d", &r, &g, &b);
  
  if (n == 3) {
    if (r >= MIN_DELAY && r <= MAX_DELAY &&
        g >= MIN_DELAY && g <= MAX_DELAY &&
        b >= MIN_DELAY && b <= MAX_DELAY) {
      redDelay = r;
      greenDelay = g;
      blueDelay = b;
      Serial.print("Set delays to: ");
      Serial.print(redDelay); Serial.print(" ");
      Serial.print(greenDelay); Serial.print(" ");
      Serial.println(blueDelay);
    } else {
      Serial.println("Error: each value must be between 100 and 999 ms.");
    }
  } else {
    Serial.println("Error: send exactly 3 integers separated by spaces (e.g. '300 500 700').");
  }
}

bool delaysSet() {
  return (redDelay >= MIN_DELAY && redDelay <= MAX_DELAY &&
          greenDelay >= MIN_DELAY && greenDelay <= MAX_DELAY &&
          blueDelay >= MIN_DELAY && blueDelay <= MAX_DELAY);
}

void loop() {
  // Wait for serial input
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      tryParseAndSetDelays(line);
    }
  }

  // If delays aren’t set yet, don’t animate LEDs
  if (!delaysSet()) {
    return;
  }

  unsigned long currentMillis = millis();

  // --- Update Red position ---
  if (currentMillis - prevRed >= (unsigned long)redDelay) {
    prevRed = currentMillis;
    red++;
    if (red >= count) red = 0;
  }

  // --- Update Green position ---
  if (currentMillis - prevGreen >= (unsigned long)greenDelay) {
    prevGreen = currentMillis;
    green++;
    if (green >= count) green = 0;
  }

  // --- Update Blue position ---
  if (currentMillis - prevBlue >= (unsigned long)blueDelay) {
    prevBlue = currentMillis;
    blue++;
    if (blue >= count) blue = 0;
  }

  strip.clear();

  for (int i = 0; i < count; i++) {
    if (i == red) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    } else if (i == green) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    } else if (i == blue) {
      strip.setPixelColor(i, strip.Color(0, 0, 255));
    }
  }

  strip.show();
}
