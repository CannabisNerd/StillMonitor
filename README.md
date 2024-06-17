# StillMonitor
# Project Tutorial: Temperature Monitoring and Logging System

## Overview

This project involves creating a temperature monitoring and logging system using two ESP8266 devices: **Still_Server** (acting as a web server and data logger) and **Still_Client** (collecting temperature data from DS18B20 sensors and sending it to Still_Server). Still_Server will host a web interface to display real-time sensor data, log it to a CSV file stored in SPIFFS, and allow users to set and log ABV (Alcohol by Volume) values.

## Step 1: Setting Up Hardware - Still_Server (Web Server and Logging Device)

### Connecting OLED Display (Optional):

- If using an OLED display with Still_Server for visual output:
  - Connect VCC and GND of the OLED display to 3.3V and GND, respectively.
  - Connect SDA (data) and SCL (clock) pins of the OLED display to the corresponding GPIO pins on the ESP8266 (e.g., GPIO 4 for SDA and GPIO 5 for SCL).

### SSD1306 OLED Display Wiring Diagram:

![SSD1306 OLED Wiring Diagram](https://example.com/ssd1306_wiring_diagram.png)

For detailed OLED display wiring instructions, refer to this [SSD1306 OLED display wiring guide](https://example.com/ssd1306_wiring_guide).

### Verify Connections:

- Double-check all connections to ensure there are no loose wires or incorrect connections.

## Step 2: Software Setup - Still_Server (Web Server and Logging Device)

### Install Libraries and Setup Arduino IDE:

- Open Arduino IDE and ensure you have the necessary libraries installed:
  - ESP8266WiFi
  - ESP8266WebServer
  - FS (for SPIFFS)
  - Wire (for OLED display, if used)
  - Adafruit_GFX (for OLED display, if used)
  - Adafruit_SSD1306 (for OLED display, if used)
- Set up the Arduino IDE for ESP8266 boards if not already done.

### Copy and Upload Still_Server Code:

- Copy the Still_Server code provided earlier into a new sketch in Arduino IDE.
- Replace the Wi-Fi credentials (`ssid` and `password`) with your own network details.
- Verify and upload Still_Server code to one of your ESP8266 devices.

### Accessing the Web Interface:

- Once uploaded, open a web browser on any device connected to the same network as Still_Server.
- Enter the IP address of Still_Server in the browser’s address bar (e.g., `http://192.168.2.190`).
- The web interface should display real-time sensor data, provide options for viewing logged data, renaming sensors, and setting ABV values.

## Step 3: Setting Up Hardware - Still_Client (Temperature Monitoring Device)

### Connecting DS18B20 Temperature Sensors:

- Connect the DS18B20 temperature sensors to GPIO D5 (or any compatible data pin) on Still_Client.
- Each DS18B20 sensor has three pins: VCC, GND, and Data. Connect:
  - VCC to 3.3V or 5V (depending on your sensor's specifications).
  - GND to GND.
  - Data to GPIO D5 (or your chosen data pin).
- **Important:** DS18B20 sensors require a 4.7kΩ pull-up resistor connected between the data (DQ) line and the VCC line to function correctly. Ensure this resistor is in place to pull the data line high. For guidance on wiring DS18B20 sensors, refer to this [guide for DS18B20 temperature sensor with Arduino](https://randomnerdtutorials.com/guide-for-ds18b20-temperature-sensor-with-arduino/).

### Understanding OneWire Communication:

- DS18B20 sensors use the OneWire protocol to communicate with the ESP8266.
- OneWire allows multiple DS18B20 sensors to share the same data pin (D5) using unique addresses.

### Verify Connections:

- Double-check all connections to ensure there are no loose wires or incorrect connections.

## Step 4: Software Setup - Still_Client (Temperature Monitoring Device)

### Install Libraries and Setup Arduino IDE:

- Open a new Arduino IDE window for Still_Client.
- Ensure the necessary libraries are installed:
  - ESP8266WiFi
  - OneWire
  - DallasTemperature

### Copy and Upload Still_Client Code:

- Copy the Still_Client code provided earlier into the Arduino IDE window for Still_Client.
- Replace the Wi-Fi credentials (`ssid` and `password`) with your own network details.
- Replace the `serverAddress` with the IP address of Still_Server.
- Verify and upload Still_Client code to the second ESP8266 device.

### Monitoring and Sending Data:

- Still_Client will read temperature data from DS18B20 sensors connected to GPIO D5.
- It will periodically send this data to Still_Server using HTTP GET requests.
- Ensure both devices are powered and connected to the same Wi-Fi network.

## Conclusion

By following these steps, you have set up a temperature monitoring and logging system using ESP8266 devices. Still_Server acts as a web server, displaying real-time temperature data, allowing users to set and log ABV values, and logging data to a CSV file accessible through a web interface. Still_Client collects temperature data from DS18B20 sensors and sends it to Still_Server for display and logging. This system provides a flexible framework for monitoring and logging temperature data in various applications.
