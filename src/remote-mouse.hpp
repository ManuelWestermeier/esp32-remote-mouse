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
  // Check if the request contains an action parameter
  int actionIndex = request.indexOf("action=");
  if (actionIndex == -1) return;
  
  // Extract the action string
  int actionEnd = request.indexOf('&', actionIndex);
  if (actionEnd == -1) actionEnd = request.indexOf(' ', actionIndex);
  String action = request.substring(actionIndex + 7, actionEnd);
  Serial.println("Action: " + action);
  
  // For move and scroll, we optionally extract dx and dy values
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
      bleMouse.move(dx, dy);  // Move cursor (x, y). Third parameter is wheel if needed.
    }
  }
  else if (action == "click") {
    if (bleMouse.isConnected()) {
      Serial.println("Mouse click triggered");
      bleMouse.click(MOUSE_LEFT);  // Left click. (Check your library documentation if MOUSE_LEFT is defined)
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
      // Pass 0 for x and y movement; the third parameter controls the scroll wheel.
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
    
    // Build and send the HTML page
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>ESP32 BLE Mouse Control</title></head><body>";
    html += "<h1>ESP32 BLE Mouse Control</h1>";
    html += "<p><button onclick=\"sendCommand('move',10,0)\">Move Right</button>";
    html += "<button onclick=\"sendCommand('move',-10,0)\">Move Left</button></p>";
    html += "<p><button onclick=\"sendCommand('move',0,-10)\">Move Up</button>";
    html += "<button onclick=\"sendCommand('move',0,10)\">Move Down</button></p>";
    html += "<p><button onclick=\"sendCommand('click')\">Click</button></p>";
    html += "<p><button onclick=\"sendCommand('scroll',0,1)\">Scroll Up</button>";
    html += "<button onclick=\"sendCommand('scroll',0,-1)\">Scroll Down</button></p>";
    html += "<script>";
    html += "function sendCommand(action, dx, dy) {";
    html += "  let url = '/?action=' + action;";
    html += "  if(dx !== undefined) { url += '&dx=' + dx; }";
    html += "  if(dy !== undefined) { url += '&dy=' + dy; }";
    html += "  fetch(url).then(response => response.text()).then(data => console.log(data));";
    html += "}";
    html += "</script>";
    html += "</body></html>";
    
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    client.print(html);
    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}
