// Blynk Credentials (Add your actual credentials)
#define BLYNK_TEMPLATE_ID "TMPL3itox_Glh"
#define BLYNK_TEMPLATE_NAME "Patient Monitoring System"
#define BLYNK_AUTH_TOKEN "eKn4tB7JjuAjsGZF0jekQPEpbn5vNGSV"

// Include necessary libraries
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoJson.h>

// WiFi Credentials
const char* ssid = "Airtel_arok_3180";
const char* password = "0723192827";

// OpenWeatherMap API details for AQI
const char* aqiApiKey = "0c728c09a543e1a42984f1e9a73d53d3"; // Get your API key from OpenWeatherMap
const char* aqiApiUrl = "http://api.openweathermap.org/data/2.5/air_pollution?lat=11.040825711135978&lon=77.00228451015636&appid=0c728c09a543e1a42984f1e9a73d53d3";

// OpenStreetMap Nominatim API for nearby medical shops (No API key required)
const char* nearbyApiUrl = "https://nominatim.openstreetmap.org/search?format=json&limit=5&q=pharmacy+near+11.040825711135978,77.00228451015636";

// Software Serial for Arduino communication
#define RX_PIN D1 // NodeMCU D1 for RX
#define TX_PIN D2 // NodeMCU D2 for TX
SoftwareSerial ArduinoSerial(RX_PIN, TX_PIN); // RX, TX

WiFiClient client;

// Blynk Virtual Pins
#define VPIN_TEMP V0
#define VPIN_PULSE V1
#define VPIN_BP V2
#define VPIN_AQI V4
#define VPIN_MEDICAL_SHOPS V5
#define VPIN_REMINDER V6
#define VPIN_TIPS V7
#define VPIN_CHATBOT V8

BlynkTimer timer;

// Function declarations
bool validateData(String data);
String getValue(String data, char separator, int index);
int parseAQI(String payload);
String parseMedicalShops(String payload);
void fetchAQI();
void fetchMedicalShops();
void sendTips();

void setup() {
  Serial.begin(115200);
  ArduinoSerial.begin(9600);
  WiFi.begin(ssid, password);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // Timers for periodic tasks
  timer.setInterval(60000L, fetchAQI);            // Fetch AQI every 60 seconds
  timer.setInterval(120000L, fetchMedicalShops);  // Fetch medical shops every 2 minutes
  timer.setInterval(300000L, sendTips);           // Send health tips every 5 minutes

  Serial.println("Setup Complete. NodeMCU Ready!");
}

void loop() {
  Blynk.run();
  timer.run();

  // Read data from Arduino
  if (ArduinoSerial.available()) {
    String sensorData = ArduinoSerial.readStringUntil('\n');
    sensorData.trim(); // Remove any leading/trailing whitespace
    if (validateData(sensorData)) {
      // Parse health data
      String temperature = getValue(sensorData, ',', 0);
      String pulseRate = getValue(sensorData, ',', 1);
      String bpValues = getValue(sensorData, ',', 2);

      // Display in Serial Monitor
      Serial.println("Temperature: " + temperature + "°C");
      Serial.println("Pulse Rate: " + pulseRate + " BPM");
      Serial.println("Blood Pressure: " + bpValues);

      // Send data to Blynk
      Blynk.virtualWrite(VPIN_TEMP, temperature.toFloat());
      Blynk.virtualWrite(VPIN_PULSE, pulseRate.toInt());
      Blynk.virtualWrite(VPIN_BP, bpValues);
    }
  }
}

// Function to fetch AQI from OpenWeatherMap
void fetchAQI() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, aqiApiUrl); // Use WiFiClient and URL
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      int aqi = parseAQI(payload);

      if (aqi != -1) {
        Serial.println("AQI: " + String(aqi));
        Blynk.virtualWrite(VPIN_AQI, aqi); // Send AQI data to Blynk
      } else {
        Serial.println("Error: Unable to parse AQI");
      }
    } else {
      Serial.println("HTTP Request Failed");
    }
    http.end();
  }
}

// Function to fetch nearby medical shops using OpenStreetMap Nominatim API
void fetchMedicalShops() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, nearbyApiUrl); // Use WiFiClient and URL
    http.addHeader("User-Agent", "ESP8266PatientMonitoringSystem");
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      String shopNames = parseMedicalShops(payload);

      if (!shopNames.isEmpty()) {
        Serial.println("Nearby Medical Shops: " + shopNames);
        Blynk.virtualWrite(VPIN_MEDICAL_SHOPS, shopNames); // Send medical shops data to Blynk
      } else {
        Serial.println("Error: Unable to parse nearby medical shops");
      }
    } else {
      Serial.println("HTTP Request Failed");
    }
    http.end();
  }
}

// Function to validate sensor data
bool validateData(String data) {
  int count = 0;
  for (uint i = 0; i < data.length(); i++) {
    if (data[i] == ',') count++;
  }
  return (count == 2); // Ensure the data has two commas
}

// Function to retrieve values from CSV
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  for (uint i = 0; i <= data.length() && found <= index; i++) {
    if (data.charAt(i) == separator || i == data.length()) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = i;
    }
  }
  return (found > index) ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Function to parse AQI from JSON payload
int parseAQI(String payload) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error || !doc["list"][0]["main"]["aqi"]) {
    return -1;
  }
  return doc["list"][0]["main"]["aqi"].as<int>();
}

// Function to parse medical shops from JSON payload
String parseMedicalShops(String payload) {
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error || doc.size() == 0) {
    return "No nearby medical shops found.";
  }
  String shops = "";
  for (int i = 0; i < doc.size(); i++) {
    if (doc[i]["display_name"]) {
      shops += doc[i]["display_name"].as<String>();
      if (i < doc.size() - 1) {
        shops += "\n";
      }
    }
  }
  return shops.isEmpty() ? "No nearby medical shops found." : shops;
}

// Function to send health tips
void sendTips() {
  String tips[] = { "Stay hydrated!", "Exercise regularly.", "Eat balanced meals.", "Get enough sleep.", "Manage stress." };
  int index = random(0, 5);
  Blynk.virtualWrite(VPIN_TIPS, tips[index]);
  Serial.println("Health Tip: " + tips[index]);
}

// Function to handle chatbot messages from the app
BLYNK_WRITE(VPIN_CHATBOT) {
  String userMessage = param.asStr();
  String response;

  if (userMessage.equalsIgnoreCase("hello")) {
    response = "Hi! How can I assist you today?";
  } else if (userMessage.equalsIgnoreCase("tips")) {
    response = "Here's a tip: Stay hydrated and exercise regularly!";
  } else if (userMessage.equalsIgnoreCase("medical shops")) {
    response = "Fetching nearby medical shops...";
    fetchMedicalShops();
    response = WiFi.status() == WL_CONNECTED ? "Check your app for the updated list!" : "Error: Unable to fetch medical shops. Please check your internet connection.";
  } else {
    response = "Sorry, I didn't understand that. Try 'hello', 'tips', or 'medical shops'.";
  }

  // Send response to Blynk
  Blynk.virtualWrite(VPIN_CHATBOT, response);
  Serial.println("Chatbot Response: " + response);
}
