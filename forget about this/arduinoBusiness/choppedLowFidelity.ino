#include <Adafruit_NeoPixel.h>

#define pin 5

#define count 11

Adafruit_NeoPixel strip(count, pin, NEO_GRB + NEO_KHZ800);

// Variables to store the last update times for each color
int prevRed = 0;
int prevGreen = 0;
int prevBlue = 0;

// Delay times (in ms) — smaller = faster runner
// int redDelay = 300;
// int greenDelay = 500;
// int blueDelay = 700;

int redDelay;
int greenDelay;
int blueDelay;

// Index positions of the active LED for each runner
int red = 0;
int green = 0;
int blue = 0;

void setup() {
  strip.begin();            
  strip.setBrightness(255);  
  strip.show();             

  //adding Serial component
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Arduino ready. Send 3 integers separated by spaces.");
}

void loop() {
  // Get current time in ms since Arduino powered on
  
   if (Serial.available()) {
    redDelay = Serial.parseInt();
    blueDelay = Serial.parseInt();
    greenDelay = Serial.parseInt();
   }  

  int currentMillis = millis();

  // --- Update Red position ---
  if (currentMillis - prevRed >= redDelay) {
    prevRed = currentMillis;  // Save current time
    red++;                    // Move red forward one LED
    if (red >= count) red = 0; // Wrap back to 0 if at the end
  }

  // --- Update Green position ---
  if (currentMillis - prevGreen >= greenDelay) {
    prevGreen = currentMillis;
    green++;
    if (green >= count) green = 0;
  }

  // --- Update Blue position ---
  if (currentMillis - prevBlue >= blueDelay) {
    prevBlue = currentMillis;
    blue++;
    if (blue >= count) blue = 0;
  }

  strip.clear();

  // Loop through each pixel and decide which color to show
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
