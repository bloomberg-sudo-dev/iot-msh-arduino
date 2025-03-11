#include "SparkFun_AS7265X.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <TimeLib.h> // Include Time library

AS7265X sensor;

const char* ssid = "";
const char* password = "";
const String googleScriptUrl = "https://script.google.com/macros/s/AKfycbyBnsHuufUGtkibwzRF7hOM_zUY7yJx6hAhRcrah87anM6Ona4xOKqD_H3Tsheim12e/exec";

// NTP Server Settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600; // Change this to your time zone offset in seconds
const int   daylightOffset_sec = 3600; // Daylight saving time offset in seconds

void setup() {
    delay(1000);
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Initialize NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("Waiting for time");
    while (time(nullptr) < 1000) {
        delay(10);
    }
    Serial.println("Time set");

    // Initialize AS7265x sensor
    if (sensor.begin() == false) {
        Serial.println("Sensor does not appear to be connected. Please check wiring. Freezing...");
        while (1);
    }
    Serial.println("Sensor Initialized");
}

void loop() {
    delay(10000);
    sensor.takeMeasurements();

    // Prepare data to send
    float data[18] = {
        sensor.getCalibratedA(), sensor.getCalibratedB(), sensor.getCalibratedC(),
        sensor.getCalibratedD(), sensor.getCalibratedE(), sensor.getCalibratedF(),
        sensor.getCalibratedG(), sensor.getCalibratedH(), sensor.getCalibratedR(),
        sensor.getCalibratedI(), sensor.getCalibratedS(), sensor.getCalibratedJ(),
        sensor.getCalibratedT(), sensor.getCalibratedU(), sensor.getCalibratedV(),
        sensor.getCalibratedW(), sensor.getCalibratedK(), sensor.getCalibratedL()
    };

    // Get current time
    time_t now = time(nullptr);
    struct tm* ptm = localtime(&now);
    char timeStringBuff[30];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", ptm);
    String currentTime = String(timeStringBuff);

    // Create JSON object
    String jsonData = "{\"time\": \"" + currentTime + "\", \"data\": [";
    for (int i = 0; i < 18; i++) {
        jsonData += String(data[i]);
        if (i < 17) {
            jsonData += ",";
        }
    }
    jsonData += "]}";

    // Send HTTP POST request to Google Sheets
    HTTPClient http;
    http.begin(googleScriptUrl);
    http.addHeader("Content-Type", "Content-Type: application/json");
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.println("Data successfully sent to Google Sheets!");
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        Serial.println("Error in sending data");
    }
    http.end();

    delay(30000); // Wait for 30 seconds
}
