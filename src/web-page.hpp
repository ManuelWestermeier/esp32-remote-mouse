#pragma once

#include <Arduino.h>

// Build and send the HTML page with slider controls and improved design
String html = R"(<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 BLE Mouse Control without Libraries</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        background: #f0f0f0;
        margin: 0;
        padding: 20px;
      }
      .container {
        background: #fff;
        padding: 20px;
        border-radius: 8px;
        box-shadow: 0 0 10px rgba(0,0,0,0.1);
        max-width: 500px;
        margin: auto;
        text-align: center;
      }
      h1 {
        color: #333;
      }
      /* Joystick zone styles */
      #joystick-zone {
        width: 300px;
        height: 300px;
        margin: 20px auto;
        background: #eee;
        border-radius: 50%;
        position: relative;
        touch-action: none; /* prevent default touch actions */
      }
      /* Joystick handle styles */
      #joystick-handle {
        width: 50px;
        height: 50px;
        background: blue;
        border-radius: 50%;
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        pointer-events: none; /* allow events to pass through */
      }
      .slider-group {
        margin: 20px 0;
      }
      .slider-label {
        margin-bottom: 5px;
        color: #555;
      }
      .slider {
        width: 100%;
      }
      .btn {
        display: block;
        width: 100%;
        padding: 10px;
        margin-top: 10px;
        background: #007BFF;
        color: #fff;
        border: none;
        border-radius: 4px;
        font-size: 16px;
        cursor: pointer;
      }
      .btn:hover {
        background: #0056b3;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>ESP32 BLE Mouse Control</h1>
      <div id="joystick-zone">
        <div id="joystick-handle"></div>
      </div>
      <div class="slider-group">
        <div class="slider-label">
          Scroll (dy): <span id="scrollValue">0</span>
        </div>
        <input type="range" min="-10" max="10" value="0" class="slider" id="sliderScroll" oninput="updateScroll(this.value) ">
      </div>
      <button class="btn" onclick="sendCommand('scroll', 0, document.getElementById('sliderScroll').value) ">Scroll</button>
      <button class="btn" onclick="sendCommand('click') ">Click</button>
    </div>

    <script>
      // Get elements
      const joystickZone = document.getElementById('joystick-zone');
      const joystickHandle = document.getElementById('joystick-handle');
      const centerX = joystickZone.clientWidth / 2;
      const centerY = joystickZone.clientHeight / 2;
      const maxDistance = centerX; // Maximum distance from center (radius)
      let isDragging = false;

      // Function to send command to the ESP32
      function sendCommand(action, dx, dy) {
        let url = '/?action=' + action;
        if (dx !== undefined) url += '&dx=' + dx;
        if (dy !== undefined) url += '&dy=' + dy;
        fetch(url)
          .then(response => response.text())
          .then(data => console.log(data));
      }

      // Update scroll display value
      function updateScroll(val) {
        document.getElementById('scrollValue').innerText = val;
      }

      // Update joystick position and send movement command
      function updateJoystick(clientX, clientY) {
        const rect = joystickZone.getBoundingClientRect();
        // Calculate pointer coordinates relative to the joystick zone
        const x = clientX - rect.left;
        const y = clientY - rect.top;
        // Calculate offset from center
        let dx = x - centerX;
        let dy = y - centerY;
        // Compute distance from center
        const distance = Math.sqrt(dx * dx + dy * dy);
        // Limit the handle movement within the joystick zone
        if (distance > maxDistance) {
          const angle = Math.atan2(dy, dx);
          dx = Math.cos(angle) * maxDistance;
          dy = Math.sin(angle) * maxDistance;
        }
        // Move the handle (adjusting so its center aligns)
        joystickHandle.style.left = (centerX + dx) + 'px';
        joystickHandle.style.top = (centerY + dy) + 'px';
        // Send the move command (adjust multiplier for sensitivity if needed)
        const multiplier = 1;
        sendCommand('move', Math.round(dx * multiplier), Math.round(dy * multiplier));
      }

      // Reset joystick handle to center and send stop command
      function resetJoystick() {
        joystickHandle.style.left = centerX + 'px';
        joystickHandle.style.top = centerY + 'px';
        sendCommand('move', 0, 0);
      }

      // Mouse event handlers
      joystickZone.addEventListener('mousedown', function(e) {
        isDragging = true;
        updateJoystick(e.clientX, e.clientY);
      });
      document.addEventListener('mousemove', function(e) {
        if (isDragging) {
          updateJoystick(e.clientX, e.clientY);
        }
      });
      document.addEventListener('mouseup', function() {
        if (isDragging) {
          isDragging = false;
          resetJoystick();
        }
      });

      // Touch event handlers
      joystickZone.addEventListener('touchstart', function(e) {
        e.preventDefault();
        isDragging = true;
        const touch = e.touches[0];
        updateJoystick(touch.clientX, touch.clientY);
      });
      joystickZone.addEventListener('touchmove', function(e) {
        e.preventDefault();
        if (isDragging) {
          const touch = e.touches[0];
          updateJoystick(touch.clientX, touch.clientY);
        }
      });
      joystickZone.addEventListener('touchend', function(e) {
        e.preventDefault();
        if (isDragging) {
          isDragging = false;
          resetJoystick();
        }
      });
    </script>
  </body>
</html>
)";