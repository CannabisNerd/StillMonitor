#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi credentials
const char* ssid = "notyournetwork";
const char* password = "Jailbreak";

// Server details
const char* serverAddress = "192.168.2.190";  // IP address of your server
const int serverPort = 80;  // Port number of your server

// Data wire is connected to D5
#define ONE_WIRE_BUS D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables to hold sensor data
float sensor1Value = 0.0;
float sensor2Value = 0.0;
float sensor3Value = 0.0;
float sensor4Value = 0.0;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Connect to WiFi with static IP configuration
  connectWiFi();

  // Initialize DS18B20 sensors
  sensors.begin();
}

void loop() {
  // Read sensor values
  sensor1Value = readTemperature(1);
  sensor2Value = readTemperature(2);
  sensor3Value = readTemperature(3);
  sensor4Value = readTemperature(4);

  // Send data to server via HTTP GET request
  sendDataToServer();

  // Delay to allow ESP8266 to handle other tasks
  delay(10000);  // 10 seconds delay
}

void connectWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

float readTemperature(int sensorNumber) {
  sensors.requestTemperatures();
  float temperature = sensors.getTempFByIndex(sensorNumber - 1);

  Serial.print("Sensor ");
  Serial.print(sensorNumber);
  Serial.print(" Temperature: ");
  Serial.println(temperature);
  
  return temperature;
}

void sendDataToServer() {
  // Construct URL with sensor data
  String url = "/?sensor1=" + String(sensor1Value, 2) +
               "&sensor2=" + String(sensor2Value, 2) +
               "&sensor3=" + String(sensor3Value, 2) +
               "&sensor4=" + String(sensor4Value, 2);

  // Establish WiFi client connection
  WiFiClient client;
  if (!client.connect(serverAddress, serverPort)) {
    Serial.println("Failed to connect to server");
    return;
  }

  // Make HTTP GET request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + serverAddress + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Data sent to server");

  // Wait for server response
  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }

  // Close connection
  client.stop();
}
