#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

// WiFi credentials - CHANGE THESE TO YOUR NETWORK
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// LED strip configuration
#define LED_PIN 9
#define LED_COUNT 11

// Create web server on port 80
WebServer server(80);

// Create NeoPixel strip object
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Variables to store the last update times for each color
unsigned long prevRed = 0;
unsigned long prevGreen = 0;
unsigned long prevBlue = 0;

// Delay times (in ms) - default values
int redDelay = 300;
int greenDelay = 500;
int blueDelay = 700;

// Index positions of the active LED for each runner
int red = 0;
int green = 0;
int blue = 0;

// HTML page for the web interface
const char* htmlPage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>LED Runner Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 600px;
            margin: 0 auto;
            padding: 20px;
            background-color: #f0f0f0;
        }
        .container {
            background-color: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            text-align: center;
            color: #333;
            margin-bottom: 30px;
        }
        .control-group {
            margin-bottom: 20px;
            padding: 15px;
            border-radius: 5px;
        }
        .red-control { background-color: #ffebee; }
        .green-control { background-color: #e8f5e8; }
        .blue-control { background-color: #e3f2fd; }
        
        label {
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
        }
        input[type="range"] {
            width: 100%;
            margin: 10px 0;
        }
        input[type="number"] {
            width: 80px;
            padding: 5px;
            border: 1px solid #ddd;
            border-radius: 3px;
        }
        .value-display {
            font-size: 18px;
            font-weight: bold;
            color: #666;
        }
        button {
            background-color: #4CAF50;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            margin-top: 20px;
        }
        button:hover {
            background-color: #45a049;
        }
        .status {
            margin-top: 20px;
            padding: 10px;
            background-color: #e8f5e8;
            border-radius: 5px;
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🏃‍♂️ LED Runner Controller</h1>
        
        <div class="control-group red-control">
            <label for="redDelay">🔴 Red Runner Speed</label>
            <input type="range" id="redDelay" min="100" max="999" value="300" oninput="updateRed(this.value)">
            <div class="value-display">Delay: <span id="redValue">300</span> ms</div>
        </div>
        
        <div class="control-group green-control">
            <label for="greenDelay">🟢 Green Runner Speed</label>
            <input type="range" id="greenDelay" min="100" max="999" value="500" oninput="updateGreen(this.value)">
            <div class="value-display">Delay: <span id="greenValue">500</span> ms</div>
        </div>
        
        <div class="control-group blue-control">
            <label for="blueDelay">🔵 Blue Runner Speed</label>
            <input type="range" id="blueDelay" min="100" max="999" value="700" oninput="updateBlue(this.value)">
            <div class="value-display">Delay: <span id="blueValue">700</span> ms</div>
        </div>
        
        <button onclick="updateAll()">Update All Runners</button>
        
        <div class="status" id="status">Ready to control LED runners!</div>
    </div>

    <script>
        function updateRed(value) {
            document.getElementById('redValue').textContent = value;
        }
        
        function updateGreen(value) {
            document.getElementById('greenValue').textContent = value;
        }
        
        function updateBlue(value) {
            document.getElementById('blueValue').textContent = value;
        }
        
        function updateAll() {
            const redDelay = document.getElementById('redDelay').value;
            const greenDelay = document.getElementById('greenDelay').value;
            const blueDelay = document.getElementById('blueDelay').value;
            
            const status = document.getElementById('status');
            status.textContent = 'Updating runners...';
            
            fetch('/update', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: 'red=' + redDelay + '&green=' + greenDelay + '&blue=' + blueDelay
            })
            .then(response => response.text())
            .then(data => {
                status.textContent = 'Runners updated successfully!';
                setTimeout(() => {
                    status.textContent = 'Ready to control LED runners!';
                }, 2000);
            })
            .catch(error => {
                status.textContent = 'Error updating runners: ' + error;
            });
        }
        
        // Load current values on page load
        window.onload = function() {
            fetch('/status')
            .then(response => response.json())
            .then(data => {
                document.getElementById('redDelay').value = data.red;
                document.getElementById('greenDelay').value = data.green;
                document.getElementById('blueDelay').value = data.blue;
                document.getElementById('redValue').textContent = data.red;
                document.getElementById('greenValue').textContent = data.green;
                document.getElementById('blueValue').textContent = data.blue;
            });
        };
    </script>
</body>
</html>
)";

void setup() {
  Serial.begin(115200);
  
  // Initialize LED strip
  strip.begin();
  strip.setBrightness(255);
  strip.show();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/status", handleStatus);
  
  // Start the server
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
  
  // LED animation logic (same as original)
  unsigned long currentMillis = millis();
  
  // Update Red position
  if (currentMillis - prevRed >= redDelay) {
    prevRed = currentMillis;
    red++;
    if (red >= LED_COUNT) red = 0;
  }
  
  // Update Green position
  if (currentMillis - prevGreen >= greenDelay) {
    prevGreen = currentMillis;
    green++;
    if (green >= LED_COUNT) green = 0;
  }
  
  // Update Blue position
  if (currentMillis - prevBlue >= blueDelay) {
    prevBlue = currentMillis;
    blue++;
    if (blue >= LED_COUNT) blue = 0;
  }
  
  // Update LED strip
  strip.clear();
  
  for (int i = 0; i < LED_COUNT; i++) {
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

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleUpdate() {
  if (server.hasArg("red") && server.hasArg("green") && server.hasArg("blue")) {
    redDelay = server.arg("red").toInt();
    greenDelay = server.arg("green").toInt();
    blueDelay = server.arg("blue").toInt();
    
    // Validate ranges
    if (redDelay < 100) redDelay = 100;
    if (redDelay > 999) redDelay = 999;
    if (greenDelay < 100) greenDelay = 100;
    if (greenDelay > 999) greenDelay = 999;
    if (blueDelay < 100) blueDelay = 100;
    if (blueDelay > 999) blueDelay = 999;
    
    Serial.print("Updated delays - Red: ");
    Serial.print(redDelay);
    Serial.print(", Green: ");
    Serial.print(greenDelay);
    Serial.print(", Blue: ");
    Serial.println(blueDelay);
    
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void handleStatus() {
  String json = "{";
  json += "\"red\":" + String(redDelay) + ",";
  json += "\"green\":" + String(greenDelay) + ",";
  json += "\"blue\":" + String(blueDelay);
  json += "}";
  
  server.send(200, "application/json", json);
}
