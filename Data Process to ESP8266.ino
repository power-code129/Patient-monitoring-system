#include <SoftwareSerial.h>

// Define pins for Software Serial
#define RX_PIN D1 // NodeMCU D1 for RX
#define TX_PIN D2 // NodeMCU D2 for TX

// Create a SoftwareSerial object
SoftwareSerial ArduinoSerial(RX_PIN, TX_PIN);

void setup() {
  // Begin communication
  Serial.begin(115200);         // Debug communication (NodeMCU <-> PC)
  ArduinoSerial.begin(9600);    // Communication with Arduino Uno

  // Debugging message
  Serial.println("NodeMCU Initialized and Ready to Receive Data...");
}

void loop() {
  // Check if data is available from Arduino
  if (ArduinoSerial.available()) {
    String sensorData = ArduinoSerial.readStringUntil('\n'); // Read data until newline
    Serial.println("Received Data: " + sensorData);          // Print received data to Serial Monitor

    // Split the data if needed
    String temperature = getValue(sensorData, ',', 0);
    String pulseRate = getValue(sensorData, ',', 1);
    String ecgSignal = getValue(sensorData, ',', 2);

    // Debugging - display individual values
    Serial.println("Temperature: " + temperature + "°C");
    Serial.println("Pulse Rate: " + pulseRate + " BPM");
    Serial.println("ECG Signal: " + ecgSignal);

    // Further actions can be taken, such as:
    // - Sending data to a server
    // - Processing and storing locally
  }

  delay(1000); // Delay for stability
}

// Function to split string by delimiter
String getValue(String data, char separator, int index) {
  int found = 0;
  int start = 0;
  int end = -1;

  for (int i = 0; i <= index; i++) {
    start = end + 1;
    end = data.indexOf(separator, start);

    if (end == -1) {
      end = data.length();
    }

    if (i == index) {
      return data.substring(start, end);
    }
  }

  return "";
}
