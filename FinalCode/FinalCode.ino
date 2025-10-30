#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

// ------------------- Wi-Fi SETUP -------------------
const char* ssid = "pramyas iphone (3)";L
const char* password = "a1b2c3d4e5";

WebServer server(80);

// ------------------- LED SETUP -------------------
#define LED_PIN 5
#define LED_COUNT 300
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// ------------------- VARIABLES -------------------
bool redOn = false, greenOn = false, blueOn = false;
bool raceActive = false;

unsigned long startTime = 0;
float redSpeed = 0, greenSpeed = 0, blueSpeed = 0; // LEDs per ms
float raceDuration = 0; // in ms

// Distance setup
float trackLength = 5.0; // meters per loop
float selectedDistance = 100.0; // default 100m

// ------------------- HTML FRONTEND -------------------
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Duke Track Pace Control</title>
  <style>
    :root { --duke-blue: #001a57; --duke-light-blue: #00539b; --duke-gray: #e0e0e0; }
    body { margin:0; padding:0; background:var(--duke-blue); color:white; font-family:Poppins, sans-serif; display:flex; flex-direction:column; align-items:center; justify-content:center; min-height:100vh; }
    .track-container { position:relative; width:500px; height:300px; background:var(--duke-light-blue); border-radius:150px/90px; box-shadow:inset 0 0 0 15px #fff,inset 0 0 0 30px var(--duke-blue); display:flex; justify-content:center; align-items:center; margin-bottom:40px; }
    .start-point { position:absolute; bottom:5px; left:50%; transform:translateX(-50%); width:50px; height:50px; background-color:#ff0000; border-radius:50%; border:3px solid white; box-shadow:0 0 10px red; display:flex; align-items:center; justify-content:center; font-weight:700; font-size:12px; color:white; text-shadow:0 0 5px #000; }
    .control-panel { background:white; color:#333; border-radius:15px; box-shadow:0 4px 20px rgba(0,0,0,0.2); padding:20px 30px; width:460px; }
    h2 { text-align:center; margin-bottom:20px; color:var(--duke-blue); font-weight:700; }
    .control-row { display:flex; align-items:center; justify-content:space-between; margin-bottom:15px; transition:opacity .3s ease; }
    .control-row.off { opacity:.5; }
    .color-label { display:flex; align-items:center; gap:10px; font-weight:600; }
    .color-dot { width:20px; height:20px; border-radius:50%; border:2px solid #555; }
    .color-dot.red{background:red;} .color-dot.green{background:green;} .color-dot.blue{background:blue;}
    .switch { position:relative; display:inline-block; width:50px; height:25px; }
    .switch input{opacity:0;width:0;height:0;}
    .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;transition:.4s;border-radius:34px;}
    .slider:before{position:absolute;content:"";height:18px;width:18px;left:4px;bottom:3.5px;background:white;transition:.4s;border-radius:50%;}
    input:checked + .slider{background-color:var(--duke-light-blue);}
    input:checked + .slider:before{transform:translateX(24px);}
    input[type=number]{width:70px;padding:4px;border:1px solid #aaa;border-radius:6px;text-align:right;}
    select{padding:5px 10px;border-radius:6px;border:1px solid #aaa;font-size:1rem;color:#333;}
    button { background:var(--duke-light-blue); color:white; border:none; padding:10px 20px; border-radius:10px; font-size:1rem; font-weight:600; cursor:pointer; width:100%; margin-top:15px; transition:0.3s; }
    button:hover { background:#003f7f; }
    hr{border:0;height:1px;background:var(--duke-gray);margin:15px 0;}
    .footer-text{margin-top:20px;color:#ccc;font-size:.9rem;text-align:center;}
  </style>
</head>
<body>
  <div class="track-container"><div class="start-point">START</div></div>
  <div class="control-panel">
    <h2>Track Light Controls</h2>
    <div class="control-row" id="row-red">
      <div class="color-label"><div class="color-dot red"></div>Pace 1</div>
      <label class="switch"><input type="checkbox" id="redToggle"/><span class="slider"></span></label>
      <input type="number" id="redPace" placeholder="sec"/>
    </div>
    <div class="control-row" id="row-green">
      <div class="color-label"><div class="color-dot green"></div>Pace 2</div>
      <label class="switch"><input type="checkbox" id="greenToggle"/><span class="slider"></span></label>
      <input type="number" id="greenPace" placeholder="sec"/>
    </div>
    <div class="control-row" id="row-blue">
      <div class="color-label"><div class="color-dot blue"></div>Pace 3</div>
      <label class="switch"><input type="checkbox" id="blueToggle"/><span class="slider"></span></label>
      <input type="number" id="bluePace" placeholder="sec"/>
    </div>
    <hr/>
    <div class="control-row">
      <label for="distanceSelect"><strong>Event Distance:</strong></label>
      <select id="distanceSelect">
        <option value="60">60m</option><option value="100">100m</option><option value="200">200m</option>
        <option value="400">400m</option><option value="800">800m</option><option value="1500">1500m</option>
        <option value="1600">1600m (mile)</option><option value="3000">3000m</option>
        <option value="5000">5000m</option><option value="10000">10000m</option>
      </select>
    </div>
    <button id="startRace">Start Race</button>
  </div>
  <div class="footer-text">© Ace the Pace</div>

  <script>
    const lights = ["red", "green", "blue"];
    const toggles = {};
    const paces = {};

    lights.forEach((color) => {
      toggles[color] = document.getElementById(color + "Toggle");
      paces[color] = document.getElementById(color + "Pace");

      toggles[color].addEventListener("change", (e) => {
        fetch(`/${color}?state=${e.target.checked ? 1 : 0}`);
      });

      paces[color].addEventListener("change", (e) => {
        fetch(`/${color}Pace?value=${e.target.value}`);
      });
    });

    document.getElementById("distanceSelect").addEventListener("change", (e) => {
      fetch(`/distance?value=${e.target.value}`);
    });

    document.getElementById("startRace").addEventListener("click", () => {
      fetch(`/start`);
    });
  </script>
</body>
</html>
)rawliteral";

// ------------------- SERVER HANDLERS -------------------
void handleRoot() { server.send(200, "text/html", htmlPage); }

void handleColorControl(String color, bool state) {
  if (color == "red") redOn = state;
  if (color == "green") greenOn = state;
  if (color == "blue") blueOn = state;
}

void handleColorPace(String color, float paceSec) {
  // paceSec = time to finish selected distance in seconds
  // Convert to LEDs per millisecond
  if (selectedDistance <= 0) selectedDistance = 100.0;
  float laps = selectedDistance / trackLength;
  float totalLEDTravel = laps * LED_COUNT;

  float speed = totalLEDTravel / (paceSec * 1000.0); // LEDs per ms

  if (color == "red") redSpeed = speed;
  if (color == "green") greenSpeed = speed;
  if (color == "blue") blueSpeed = speed;
}

void handleDistance(float value) { selectedDistance = value; }

void startRace() {
  raceActive = true;
  startTime = millis();
  float laps = selectedDistance / trackLength;
  raceDuration = (max({redSpeed, greenSpeed, blueSpeed}) > 0)
                     ? (laps * LED_COUNT) / max({redSpeed, greenSpeed, blueSpeed})
                     : 0;
  Serial.println("Race started!");
}

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(255);
  strip.show();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/red", []() { handleColorControl("red", server.arg("state") == "1"); server.send(200, "text/plain", "OK"); });
  server.on("/green", []() { handleColorControl("green", server.arg("state") == "1"); server.send(200, "text/plain", "OK"); });
  server.on("/blue", []() { handleColorControl("blue", server.arg("state") == "1"); server.send(200, "text/plain", "OK"); });

  server.on("/redPace", []() { handleColorPace("red", server.arg("value").toFloat()); server.send(200, "text/plain", "OK"); });
  server.on("/greenPace", []() { handleColorPace("green", server.arg("value").toFloat()); server.send(200, "text/plain", "OK"); });
  server.on("/bluePace", []() { handleColorPace("blue", server.arg("value").toFloat()); server.send(200, "text/plain", "OK"); });

  server.on("/distance", []() { handleDistance(server.arg("value").toFloat()); server.send(200, "text/plain", "OK"); });
  server.on("/start", []() { startRace(); server.send(200, "text/plain", "Race Started"); });

  server.begin();
}

// ------------------- LOOP -------------------
void loop() {
  server.handleClient();

  if (!raceActive) return;

  unsigned long now = millis();
  if (now - startTime > raceDuration) {
    raceActive = false;
    strip.clear();
    strip.show();
    Serial.println("Race finished!");
    return;
  }

  strip.clear();
  const int clusterSize = 3;
  const int halfCluster = clusterSize / 2;

  if (redOn && redSpeed > 0) {
    int redIndex = int(fmod((now - startTime) * redSpeed, LED_COUNT));
    for (int i = -halfCluster; i <= halfCluster; i++) {
      int idx = (redIndex + i + LED_COUNT) % LED_COUNT;
      strip.setPixelColor(idx, strip.Color(255, 0, 0));
    }
  }

  if (greenOn && greenSpeed > 0) {
    int greenIndex = int(fmod((now - startTime) * greenSpeed, LED_COUNT));
    for (int i = -halfCluster; i <= halfCluster; i++) {
      int idx = (greenIndex + i + LED_COUNT) % LED_COUNT;
      strip.setPixelColor(idx, strip.Color(0, 255, 0));
    }
  }

  if (blueOn && blueSpeed > 0) {
    int blueIndex = int(fmod((now - startTime) * blueSpeed, LED_COUNT));
    for (int i = -halfCluster; i <= halfCluster; i++) {
      int idx = (blueIndex + i + LED_COUNT) % LED_COUNT;
      strip.setPixelColor(idx, strip.Color(0, 0, 255));
    }
  }

  strip.show();
}
