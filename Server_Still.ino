#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h> // For SPIFFS
#include <Wire.h> 
#include <Adafruit_GFX.h> // Make LED look good
#include <Adafruit_SSD1306.h> // OLED Library

// Function prototypes
void handleLogFile();
void handleRoot();
void connectWiFi();
String getLoggedDataAsTable();
void updateOLED();


const char* ssid = "SSID"; // Enter your SSID
const char* password = "WIFI Password"; // Password


// Your client needs to know where to send the data so a Static IP is needed. 

IPAddress staticIP(192, 168, 1, 190); // Static IP address configuration
IPAddress gateway(192, 168, 1, 1);    // Gateway IP address
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

ESP8266WebServer server(80); // Starts webserver on port 80

#define OLED_RESET 0
Adafruit_SSD1306 display(OLED_RESET);

unsigned long lastLogTime = 0;
unsigned long startTimeMillis = 0;
const unsigned long logInterval = 120000; // Interval for how often Temp data is logged to file, default 2 minutes

// Global variables to hold sensor data
String sensor1Value = "69"; // Initial value to simulate data
String sensor2Value = "169"; // Initial value simulate data
String sensor3Value = "420"; // Initial value simulate data
String sensor4Value = "710"; // Initial value simulate data
String sensor1Name = "Sensor 1"; // Default sensor names
String sensor2Name = "Sensor 2";
String sensor3Name = "Sensor 3";
String sensor4Name = "Sensor 4";
float abvPercentage = 0.0; // Initial ABV percentage

void handleLogFile() { 
    File logFile = SPIFFS.open("/log.csv", "r"); // Opens log file
    if (!logFile) {
        server.send(500, "text/plain", "Failed to open log.csv"); // Prints error to serial if unable to open the file
        return;
    }

    String logData = "";
    while (logFile.available()) {
        logData += logFile.readStringUntil('\n'); // Logs new Temperature data
    }
    logFile.close();

    server.send(200, "text/csv", logData);
}

void setup() {
    Serial.begin(115200);

    // Initialize SPIFFS
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }

    // Backup existing log.csv if it exists
    if (SPIFFS.exists("/log.csv")) {
        if (SPIFFS.exists("/log_old.csv")) {
            SPIFFS.remove("/log_old.csv");
        }
        SPIFFS.rename("/log.csv", "/log_old.csv");
        Serial.println("Backed up existing log.csv to log_old.csv");
    }

    // Create a new log.csv file if it doesn't exist
    File logFile = SPIFFS.open("/log.csv", "w");
    if (!logFile) {
        Serial.println("Failed to create log.csv");
    } else {
        logFile.println("Time,Sensor 1,Sensor 2,Sensor 3,Sensor 4,ABV%"); // Add header to the new file
        logFile.close();
        Serial.println("Created new log.csv");
    }

    // Initialize OLED display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();

    // Connect to WiFi with static IP configuration
    connectWiFi();

    // Set initial start time
    startTimeMillis = millis();

    // Set initial last log time
    lastLogTime = millis();
    
    // Handle root URL
    server.on("/", HTTP_GET, []() {
        handleRoot();
    });

    // Handle log file URL
    server.on("/log", HTTP_GET, []() {
        handleLogFile();
    });

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();

    // Check if it's time to log data based on logInterval
    unsigned long currentMillis = millis();
    if (currentMillis - lastLogTime >= logInterval) {
        // Update last log time to current time
        lastLogTime = currentMillis;
        
        // Log data
        logData(sensor1Value, sensor2Value, sensor3Value, sensor4Value);
    }

}

// The HTML Stuff, handles both what is displayed and receiving data via HTTP GET
void handleRoot() {
    bool dataReceived = false;

    // Check if sensor data is provided in the request
    if (server.hasArg("sensor1")) {
        sensor1Value = server.arg("sensor1");
        dataReceived = true;
    }
    if (server.hasArg("sensor2")) {
        sensor2Value = server.arg("sensor2");
        dataReceived = true;
    }
    if (server.hasArg("sensor3")) {
        sensor3Value = server.arg("sensor3");
        dataReceived = true;
    }
    if (server.hasArg("sensor4")) {
        sensor4Value = server.arg("sensor4");
        dataReceived = true;
    }

    // Check if ABV percentage is provided in the request, even though we are providing this manually it seems to help the logging work better
    if (server.hasArg("abv")) {
        abvPercentage = server.arg("abv").toFloat();
        logABV(abvPercentage); // Log ABV percentage
    }

    // Allows the use of the rename buttons
    if (server.hasArg("sensor1name")) {
        sensor1Name = server.arg("sensor1name");
    }
    if (server.hasArg("sensor2name")) {
        sensor2Name = server.arg("sensor2name");
    }
    if (server.hasArg("sensor3name")) {
        sensor3Name = server.arg("sensor3name");
    }
    if (server.hasArg("sensor4name")) {
        sensor4Name = server.arg("sensor4name");
    }

    if (dataReceived) {
       
    }

    // Prepare HTML response with updated sensor data and controls
    String html = "<html><head>";
    html += "<meta http-equiv='refresh' content='30; url=/' />"; // Refresh every 30 seconds
    html += "<style>body { font-family: Arial, Helvetica, sans-serif; }</style>"; // Basic styling for body
    html += "</head><body>";
    html += "<h1 style='text-align: center;'>Distillation System Dashboard</h1>";

    // Create a 3x2 grid for sensor data display and renaming form
    html += "<div style='display: grid; grid-template-columns: 1fr 1fr 1fr; grid-gap: 20px; text-align: center;'>";

    // Sensor 1
    html += "<div>";
    html += "<div style='font-size: 24px;'><strong>" + sensor1Name + "</strong><br>" + sensor1Value + "F</div>";
    html += "<form action='/' method='get'>";
    html += "<input type='text' id='sensor1name' name='sensor1name' value='" + sensor1Name + "'><input type='submit' name='rename1' value='Rename'>";
    html += "</form>";
    html += "</div>";

    // Sensor 2
    html += "<div>";
    html += "<div style='font-size: 24px;'><strong>" + sensor2Name + "</strong><br>" + sensor2Value + "F</div>";
    html += "<form action='/' method='get'>";
    html += "<input type='text' id='sensor2name' name='sensor2name' value='" + sensor2Name + "'><input type='submit' name='rename2' value='Rename'>";
    html += "</form>";
    html += "</div>";

    // Sensor 3
    html += "<div>";
    html += "<div style='font-size: 24px;'><strong>" + sensor3Name + "</strong><br>" + sensor3Value + "F</div>";
    html += "<form action='/' method='get'>";
    html += "<input type='text' id='sensor3name' name='sensor3name' value='" + sensor3Name + "'><input type='submit' name='rename3' value='Rename'>";
    html += "</form>";
    html += "</div>";

    // Sensor 4
    html += "<div>";
    html += "<div style='font-size: 24px;'><strong>" + sensor4Name + "</strong><br>" + sensor4Value + "F</div>";
    html += "<form action='/' method='get'>";
    html += "<input type='text' id='sensor4name' name='sensor4name' value='" + sensor4Name + "'><input type='submit' name='rename4' value='Rename'>";
    html += "</form>";
    html += "</div>";

    // ABV percentage input form
    html += "<div>";
    html += "<h2>ABV %:</h2>";
    html += "<form action='/' method='get'>";
    html += "<input type='text' id='abv' name='abv' value='" + String(abvPercentage, 2) + "'><br>";
    html += "<input type='submit' value='Submit'>";
    html += "</form>";
    html += "</div>";

    // Logged data table
    html += "<div>";
    html += "<h2>Logged Data:</h2>";
    html += "<table border='1' style='width: 100%; text-align: center;'>";
    html += "<tr><th>Time (mins)</th><th>" + sensor1Name + "</th><th>" + sensor2Name + "</th><th>" + sensor3Name + "</th><th>" + sensor4Name + "</th><th>ABV %</th></tr>";
    html += getLoggedDataAsTable(); // Call the function to generate the table
    html += "</table>";
    html += "</div>";

    html += "</div>"; // Close grid

    // Download CSV button
    html += "<div style='margin-top: 20px; text-align: center;'>";
    html += "<br><a href='/log'><button style='padding: 10px; background-color: #4CAF50; color: white; border: none; cursor: pointer;'>Download Log</button></a>";
    html += "</div>";

    html += "</body></html>";

    server.send(200, "text/html", html);

    // Update OLED display with new sensor data
    updateOLED();
}


void connectWiFi() {
    WiFi.config(staticIP, gateway, subnet); // Set static IP configuration
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void logData(String sensor1Value, String sensor2Value, String sensor3Value, String sensor4Value) {
    // Get current time in milliseconds since start
    unsigned long currentTime = millis();

    // Open log.csv file in append mode
    File logFile = SPIFFS.open("/log.csv", "a");
    if (!logFile) {
        Serial.println("Failed to open log.csv for appending");
        return;
    }

    // Format the data string to be logged (without ABV)
    String data = String(currentTime) + "," + sensor1Value + "," + sensor2Value + "," + sensor3Value + "," + sensor4Value + ",\n";

    // Write the data to log.csv
    logFile.print(data);
    logFile.close();
}

String getLoggedDataAsTable() {
    String tableData = "";

    // Open log.csv file
    File logFile = SPIFFS.open("/log.csv", "r");
    if (!logFile) {
        Serial.println("Failed to open log.csv for reading");
        return tableData;
    }

    // Skip header line
    logFile.readStringUntil('\n');

    // Read each line from log.csv and create table rows
    while (logFile.available()) {
        String line = logFile.readStringUntil('\n');
        if (line.length() > 0) {
            // Parse CSV line
            String timestamp = getValue(line, ',', 0);
            String sensor1 = getValue(line, ',', 1);
            String sensor2 = getValue(line, ',', 2);
            String sensor3 = getValue(line, ',', 3);
            String sensor4 = getValue(line, ',', 4);
            String abv = getValue(line, ',', 5);

            // Convert timestamp to minutes relative to device startup
            unsigned long currentTime = millis();
            unsigned long logTime = timestamp.toInt();
            unsigned long minutes = (logTime - startTimeMillis) / (1000 * 60); // Convert milliseconds to minutes

            // Create table row
            tableData += "<tr><td>" + String(minutes) + "</td><td>" + sensor1 + "</td><td>" + sensor2 + "</td><td>" + sensor3 + "</td><td>" + sensor4 + "</td><td>" + abv + "</td></tr>";
        }
    }
    logFile.close();

    return tableData;
}

void logABV(float abv) {
    File logFile = SPIFFS.open("/log.csv", "r+");
    if (!logFile) {
        Serial.println("Failed to open log.csv for updating ABV");
        return;
    }

    // Skip header line
    String header = logFile.readStringUntil('\n');

    // Find the last line
    String lastLine;
    long lastLinePosition = -1;
    while (logFile.available()) {
        long currentPosition = logFile.position();
        lastLine = logFile.readStringUntil('\n');
        if (lastLine.length() > 0) {
            lastLinePosition = currentPosition;
        }
    }

    // Update ABV value in the last line
    if (lastLinePosition != -1) {
        int lastComma = lastLine.lastIndexOf(',');
        if (lastComma != -1) {
            String updatedLine = lastLine.substring(0, lastComma + 1) + String(abv, 2) + "\n"; // Update ABV field
            logFile.seek(lastLinePosition);
            logFile.print(updatedLine); // Overwrite the line with updated ABV value
        }
    }

    logFile.close();
}

void updateOLED() {
    // Clear the display buffer
    display.clearDisplay();

    // Display header
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Sensor Readings");

    // Display sensor 1
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.print(sensor1Name + ": ");
    display.println(sensor1Value + "°F");

    // Display sensor 2
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print(sensor2Name + ": ");
    display.println(sensor2Value + "°F");

    // Display sensor 3
    display.setTextSize(1);
    display.setCursor(0, 30);
    display.print(sensor3Name + ": ");
    display.println(sensor3Value + "°F");

    // Display sensor 4
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.print(sensor4Name + ": ");
    display.println(sensor4Value + "°F");

    // Update the OLED display with all changes above
    display.display();
}

String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
