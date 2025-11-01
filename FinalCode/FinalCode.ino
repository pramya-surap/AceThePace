#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

// ------------------- Wi-Fi SETUP -------------------
const char* ssid = "DukeTrack";
const char* password = "duke123";

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
float redPace = 0, greenPace = 0, bluePace = 0;    // seconds for distance
float raceDuration = 0; // ms

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

  <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;600;700&display=swap" rel="stylesheet">
  <style>
    :root {
      --duke-blue: #001a57;
      --duke-light-blue: #00539b;
      --duke-gray: #e0e0e0;
    }
    body {
      margin: 0;
      padding: 50px;
      background: linear-gradient(145deg, #001a57 0%, #003f91 50%, #00539b 100%);
      color: white;
      font-family: 'Poppins', sans-serif;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
    }
    .track-container {
      position: relative;
      width: 550px;
      height: 330px;
      background: radial-gradient(circle at center, #00539b 30%, #003b73 100%);
      border-radius: 160px/100px;
      box-shadow: inset 0 0 0 10px white,
                  inset 0 0 0 25px rgba(0,0,0,0.2),
                  0 8px 25px rgba(0,0,0,0.5);
      display: flex;
      justify-content: center;
      align-items: center;
      margin-bottom: 50px;
      overflow: hidden;
      transition: all 0.4s ease;
    }
    .track-container.glow {
      box-shadow: 0 0 25px rgba(0, 115, 255, 0.8),
                  inset 0 0 0 10px white,
                  0 8px 30px rgba(0,0,0,0.6);
    }
    .start-point {
      position: absolute;
      bottom: 5px;
      left: 50%;
      transform: translateX(-50%);
      width: 55px;
      height: 55px;
      background-color: #ff4b4b;
      border-radius: 50%;
      border: 3px solid white;
      box-shadow: 0 0 20px rgba(255,0,0,0.8);
      display: flex;
      align-items: center;
      justify-content: center;
      font-weight: 700;
      font-size: 12px;
      color: white;
      letter-spacing: 1px;
    }
    .control-panel {
      background: linear-gradient(180deg, #ffffff 0%, #f8f9ff 100%);
      color: #333;
      border-radius: 20px;
      box-shadow: 0 6px 25px rgba(0, 0, 0, 0.25);
      padding: 30px 35px;
      width: 480px;
      transition: transform 0.3s ease;
    }
    .control-panel:hover { transform: translateY(-5px); }
    h2 { text-align: center; margin-bottom: 25px; color: var(--duke-blue); font-weight: 700; font-size: 1.6rem; }
    .control-row { display: flex; align-items: center; justify-content: space-between; margin-bottom: 18px; }
    .color-label { display: flex; align-items: center; gap: 10px; font-weight: 600; font-size: 1.05rem; }
    .color-dot { width: 20px; height: 20px; border-radius: 50%; border: 2px solid #555; box-shadow: 0 0 5px rgba(0,0,0,0.2); }
    .color-dot.red { background: red; box-shadow: 0 0 8px rgba(255,0,0,0.8); }
    .color-dot.green { background: limegreen; box-shadow: 0 0 8px rgba(0,255,0,0.8); }
    .color-dot.blue { background: dodgerblue; box-shadow: 0 0 8px rgba(0,0,255,0.8); }
    .switch { position: relative; display: inline-block; width: 50px; height: 25px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 18px; width: 18px; left: 4px; bottom: 3.5px; background: white; transition: .4s; border-radius: 50%; }
    input:checked + .slider { background-color: var(--duke-light-blue); box-shadow: 0 0 10px #0073ff; }
    input:checked + .slider:before { transform: translateX(24px); }
    input[type=number] { width: 80px; padding: 6px; border: 1px solid #bbb; border-radius: 8px; text-align: right; background: #f5f5f5; font-weight: 500; }
    select { padding: 8px 14px; border-radius: 8px; border: 1px solid #bbb; font-size: 1rem; color: #333; background: #f5f5f5; }
    button { background: linear-gradient(135deg, #00539b, #003f7f); color: white; border: none; padding: 12px 20px; border-radius: 12px; font-size: 1.1rem; font-weight: 600; cursor: pointer; width: 100%; margin-top: 20px; transition: all 0.3s ease; box-shadow: 0 4px 15px rgba(0, 83, 155, 0.3); }
    button:hover { background: linear-gradient(135deg, #006fd6, #004b96); transform: scale(1.02); box-shadow: 0 6px 20px rgba(0, 83, 155, 0.45); }
    hr { border: 0; height: 1px; background: var(--duke-gray); margin: 15px 0; }
    .footer-text { margin-top: 20px; color: #ccc; font-size: .9rem; text-align: center; }
    @media (max-width: 600px) { .track-container { width: 90%; height: 200px; } .control-panel { width: 90%; padding: 20px; } }
  </style>
</head>
<body>
  <div class="track-container" id="trackContainer">
    <div class="start-point">START</div>
  </div>
  <div class="control-panel">
    <h2>Track Light Controls</h2>
    <div class="control-row"><div class="color-label"><div class="color-dot red"></div>Pace 1</div><label class="switch"><input type="checkbox" id="redToggle"/><span class="slider"></span></label><input type="number" id="redPace" placeholder="sec"/></div>
    <div class="control-row"><div class="color-label"><div class="color-dot green"></div>Pace 2</div><label class="switch"><input type="checkbox" id="greenToggle"/><span class="slider"></span></label><input type="number" id="greenPace" placeholder="sec"/></div>
    <div class="control-row"><div class="color-label"><div class="color-dot blue"></div>Pace 3</div><label class="switch"><input type="checkbox" id="blueToggle"/><span class="slider"></span></label><input type="number" id="bluePace" placeholder="sec"/></div>
    <hr/>
    <div class="control-row"><label><strong>Event Distance:</strong></label><select id="distanceSelect"><option value="60">60m</option><option value="100" selected>100m</option><option value="200">200m</option><option value="400">400m</option><option value="800">800m</option><option value="1500">1500m</option><option value="1600">1600m</option><option value="3000">3000m</option><option value="5000">5000m</option><option value="10000">10000m</option></select></div>
    <button id="startRace">Start Race</button>
  </div>
  <div class="footer-text">© Ace the Pace</div>
  <script>
    const colors = ["red","green","blue"];
    colors.forEach(c=>{
      document.getElementById(c+"Toggle").addEventListener("change",e=>fetch(`/${c}?state=${e.target.checked?1:0}`));
      document.getElementById(c+"Pace").addEventListener("change",e=>fetch(`/${c}Pace?value=${e.target.value}`));
    });
    document.getElementById("distanceSelect").addEventListener("change",e=>fetch(`/distance?value=${e.target.value}`));
    const track=document.getElementById("trackContainer");
    document.getElementById("startRace").addEventListener("click",()=>{fetch(`/start`);track.classList.add("glow");setTimeout(()=>track.classList.remove("glow"),2000);});
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
  if (paceSec <= 0) return;
  float laps = selectedDistance / trackLength;
  float totalLEDTravel = laps * LED_COUNT; // total LEDs to cover full distance
  float speed = totalLEDTravel / (paceSec * 1000.0); // LEDs per ms
  if (color == "red") { redSpeed = speed; redPace = paceSec; }
  if (color == "green") { greenSpeed = speed; greenPace = paceSec; }
  if (color == "blue") { blueSpeed = speed; bluePace = paceSec; }
}

void handleDistance(float value) { selectedDistance = value; }

void startRace() {
  raceActive = true;
  startTime = millis();

  // Race ends when the *longest* pace finishes
  float maxPace = max(redPace, max(greenPace, bluePace));
  raceDuration = maxPace * 1000.0;
  Serial.printf("Race started: %.0fm, max duration %.2fs\n", selectedDistance, raceDuration/1000.0);
}

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(255);
  strip.show();

  Serial.println("Setting up Access Point...");
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/red", [](){ handleColorControl("red", server.arg("state")=="1"); server.send(200,"text/plain","OK"); });
  server.on("/green", [](){ handleColorControl("green", server.arg("state")=="1"); server.send(200,"text/plain","OK"); });
  server.on("/blue", [](){ handleColorControl("blue", server.arg("state")=="1"); server.send(200,"text/plain","OK"); });
  server.on("/redPace", [](){ handleColorPace("red", server.arg("value").toFloat()); server.send(200,"text/plain","OK"); });
  server.on("/greenPace", [](){ handleColorPace("green", server.arg("value").toFloat()); server.send(200,"text/plain","OK"); });
  server.on("/bluePace", [](){ handleColorPace("blue", server.arg("value").toFloat()); server.send(200,"text/plain","OK"); });
  server.on("/distance", [](){ handleDistance(server.arg("value").toFloat()); server.send(200,"text/plain","OK"); });
  server.on("/start", [](){ startRace(); server.send(200,"text/plain","Race Started"); });
  server.begin();
  Serial.println("Server started!");
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
  const int half = clusterSize / 2;

  if (redOn && redSpeed > 0) {
    int pos = int(fmod((now - startTime) * redSpeed, LED_COUNT));
    for (int i = -half; i <= half; i++) strip.setPixelColor((pos+i+LED_COUNT)%LED_COUNT, strip.Color(255,0,0));
  }
  if (greenOn && greenSpeed > 0) {
    int pos = int(fmod((now - startTime) * greenSpeed, LED_COUNT));
    for (int i = -half; i <= half; i++) strip.setPixelColor((pos+i+LED_COUNT)%LED_COUNT, strip.Color(0,255,0));
  }
  if (blueOn && blueSpeed > 0) {
    int pos = int(fmod((now - startTime) * blueSpeed, LED_COUNT));
    for (int i = -half; i <= half; i++) strip.setPixelColor((pos+i+LED_COUNT)%LED_COUNT, strip.Color(0,0,255));
  }

  strip.show();
}
