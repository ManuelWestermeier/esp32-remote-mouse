#include <WiFi.h>
#include <BleMouse.h>
#include <Arduino.h>

// WiFi access point credentials
const char* ssid = "ESP32_Mouse_AP";
const char* password = "12345678";

// Create a WiFi server on port 80
WiFiServer server(80);

// Create a BLE mouse object with a custom device name and manufacturer
BleMouse bleMouse("ESP32 Mouse", "THE ALFA SIGMA GIGACHAD GmbH", 100);

void setup() {
  Serial.begin(115200);
  
  // Initialize BLE Mouse
  Serial.println("Starting BLE Mouse...");
  bleMouse.begin();
  
  // Setup ESP32 as a WiFi access point
  Serial.println("Setting up WiFi Access Point...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Start the server
  server.begin();
}

void processRequest(String request) {
  int actionIndex = request.indexOf("action=");
  if (actionIndex == -1) return;
  
  int actionEnd = request.indexOf('&', actionIndex);
  if (actionEnd == -1) actionEnd = request.indexOf(' ', actionIndex);
  String action = request.substring(actionIndex + 7, actionEnd);
  Serial.println("Action: " + action);
  
  if (action == "move") {
    int dx = 0, dy = 0;
    int dxIndex = request.indexOf("dx=");
    if (dxIndex != -1) {
      int dxEnd = request.indexOf('&', dxIndex);
      if (dxEnd == -1) dxEnd = request.indexOf(' ', dxIndex);
      dx = request.substring(dxIndex + 3, dxEnd).toInt();
    }
    int dyIndex = request.indexOf("dy=");
    if (dyIndex != -1) {
      int dyEnd = request.indexOf('&', dyIndex);
      if (dyEnd == -1) dyEnd = request.indexOf(' ', dyIndex);
      dy = request.substring(dyIndex + 3, dyEnd).toInt();
    }
    if (bleMouse.isConnected()) {
      Serial.printf("Moving mouse by dx=%d, dy=%d\n", dx, dy);
      bleMouse.move(dx, dy);
    }
  }
  else if (action == "click") {
    if (bleMouse.isConnected()) {
      Serial.println("Mouse click triggered");
      bleMouse.click(MOUSE_LEFT);
    }
  }
  else if (action == "scroll") {
    int scroll = 0;
    int scrollIndex = request.indexOf("dy=");
    if (scrollIndex != -1) {
      int scrollEnd = request.indexOf(' ', scrollIndex);
      scroll = request.substring(scrollIndex + 3, scrollEnd).toInt();
    }
    if (bleMouse.isConnected()) {
      Serial.printf("Scrolling: %d\n", scroll);
      bleMouse.move(0, 0, scroll);
    }
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    
    // Process the request to trigger BLE mouse actions
    processRequest(request);
    
    // Build and send the HTML page with slider controls and improved design
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>ESP32 BLE Mouse Control</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; background: #f0f0f0; margin: 0; padding: 20px; }";
    html += ".container { background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1); max-width: 500px; margin: auto; }";
    html += "h1 { text-align: center; color: #333; }";
    html += ".slider-group { margin: 20px 0; }";
    html += ".slider-label { margin-bottom: 5px; color: #555; }";
    html += ".slider { width: 100%; }";
    html += ".btn { display: block; width: 100%; padding: 10px; margin-top: 10px; background: #007BFF; color: #fff; border: none; border-radius: 4px; font-size: 16px; cursor: pointer; }";
    html += ".btn:hover { background: #0056b3; }";
    html += "</style>";
    html += "</head><body>";
    html += "<div class='container'>";
    html += "<h1>ESP32 BLE Mouse Control</h1>";
    // Cursor Movement Sliders
    html += "<div class='slider-group'>";
    html += "<div class='slider-label'>Cursor Horizontal (dx): <span id='dxValue'>0</span></div>";
    html += "<input type='range' min='-50' max='50' value='0' class='slider' id='sliderX' oninput='updateDx(this.value)'>";
    html += "</div>";
    html += "<div class='slider-group'>";
    html += "<div class='slider-label'>Cursor Vertical (dy): <span id='dyValue'>0</span></div>";
    html += "<input type='range' min='-50' max='50' value='0' class='slider' id='sliderY' oninput='updateDy(this.value)'>";
    html += "</div>";
    html += "<button class='btn' onclick=\"sendCommand('move', document.getElementById('sliderX').value, document.getElementById('sliderY').value)\">Move Cursor</button>";
    // Scroll Slider
    html += "<div class='slider-group'>";
    html += "<div class='slider-label'>Scroll (dy): <span id='scrollValue'>0</span></div>";
    html += "<input type='range' min='-10' max='10' value='0' class='slider' id='sliderScroll' oninput='updateScroll(this.value)'>";
    html += "</div>";
    html += "<button class='btn' onclick=\"sendCommand('scroll', 0, document.getElementById('sliderScroll').value)\">Scroll</button>";
    // Click Button
    html += "<button class='btn' onclick=\"sendCommand('click')\">Click</button>";
    html += "</div>"; // End container
    html += "<script>";
    html += "function sendCommand(action, dx, dy) {";
    html += "  let url = '/?action=' + action;";
    html += "  if(dx !== undefined) { url += '&dx=' + dx; }";
    html += "  if(dy !== undefined) { url += '&dy=' + dy; }";
    html += "  fetch(url).then(response => response.text()).then(data => console.log(data));";
    html += "}";
    html += "function updateDx(val) { document.getElementById('dxValue').innerText = val; }";
    html += "function updateDy(val) { document.getElementById('dyValue').innerText = val; }";
    html += "function updateScroll(val) { document.getElementById('scrollValue').innerText = val; }";
    html += "</script>";
    html += "</body></html>";
    
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    client.print(html);
    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}
