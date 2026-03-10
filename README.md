# Smart Light Control

A comprehensive IoT solution for controlling RGB lights (NeoPixel) or high-voltage lights (via Relay) using an ESP32. This project features control via WiFi (Flask Web App & ESP32 Web Server), Bluetooth, and physical touch.

## 🚀 Features

- **WiFi Control**: Control your lights from any device on your local network using a sleek web interface.
- **REST API**: Flask-based backend communicating with ESP32-hosted endpoints.
- **NeoPixel Support**: Full RGB color selection, brightness control, and smooth transition animations.
- **Timer Functionality**: Set a countdown to turn off your lights automatically.
- **Bluetooth Control**: Alternative control method using Serial Bluetooth (Python scripts provided).
- **Physical Interaction**: Support for capacitive touch sensors to toggle light states.
- **Persistent State**: Saves your last color and brightness settings to ESP32 Flash memory (Preferences) so they persist after a reboot.

## 📁 Repository Structure

- `app.py`: Flask web application that acts as a bridge/proxy for the ESP32.
- `sketch_feb10a/`: Primary Arduino sketch for ESP32 with NeoPixel support.
- `Light/`: Additional variations of the smart light firmware (Relay-based, SPIFFS-based UI).
- `bluetooth_control.py`: Python script for Bluetooth-based commands.
- `static/`, `templates/`, `public/`: Assets for the web interface.

## 🛠️ Setup Instructions

### 1. ESP32 Firmware
1. Open any of the `.ino` files in the Arduino IDE.
2. Install necessary libraries:
   - `Adafruit_NeoPixel`
   - `Preferences`
   - `ArduinoJson` (for some versions)
3. **CRITICAL**: Update the following lines with your network details:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
4. Upload the code to your ESP32.
5. Note the IP address printed to the Serial Monitor (e.g., `192.168.1.105`).

### 2. Flask Application
1. Install Python dependencies:
   ```bash
   pip install -r requirements.txt
   ```
2. Update the `ESP32_IP` in `app.py`:
   ```python
   ESP32_IP = "http://YOUR_ESP32_IP"
   ```
3. Run the application:
   ```bash
   python app.py
   ```
4. Access the dashboard at `http://localhost:5000`.

### 3. Bluetooth Control (Optional)
1. Pair your ESP32 with your PC via Bluetooth.
2. Find the COM port assigned to the Bluetooth connection.
3. Update `COM_PORT` in `bluetooth_control.py` and run it.

## 📦 Requirements

- Python 3.x
- Flask
- Requests
- Arduino IDE with ESP32 board support

## 📝 License

This project is open-source and available for all to use and modify.
