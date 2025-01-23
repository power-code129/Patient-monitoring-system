#include <DHT.h>

// Pin Definitions
#define DHTPIN 2         // DHT11 data pin
#define DHTTYPE DHT11    // Sensor type
#define KY039_PIN A1     // KY-039 Pulse Sensor analog pin
#define AD8232_PIN A0    // AD8232 ECG Sensor analog pin
#define GREEN_LED_PIN 3  // Green LED pin
#define RED_LED_PIN 4    // Red LED pin
#define BUZZER_PIN 5     // Buzzer pin

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Begin serial communication
  Serial.begin(9600);

  // Initialize DHT sensor
  dht.begin();

  // Set pin modes
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize LEDs and buzzer to OFF
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  // Read temperature from DHT11
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    return; // Skip the rest of the loop if the temperature reading fails
  }

  // Read pulse rate from KY-039 sensor
  int pulseSignal = analogRead(KY039_PIN);
  int pulseRate = map(pulseSignal, 0, 1023, 60, 100); // Map raw value to BPM range

  // Read ECG signal from AD8232 sensor
  int ecgSignal = analogRead(AD8232_PIN);
  int systolicBP = map(ecgSignal, 0, 1023, 90, 120); // Map raw value to systolic BP range
  int diastolicBP = map(ecgSignal, 0, 1023, 60, 80); // Map raw value to diastolic BP range

  // Output only values in the format: temperature,pulseRate,systolicBP/diastolicBP
  Serial.print(temperature, 2);       // Print temperature with 2 decimal places
  Serial.print(",");
  Serial.print(pulseRate);            // Print pulse rate
  Serial.print(",");
  Serial.print(systolicBP);           // Print systolic BP
  Serial.print("/");
  Serial.println(diastolicBP);        // Print diastolic BP with newline

  // LED and Buzzer Control
  bool isNormal = true;

  // Check temperature range (e.g., normal range: 36.0°C - 37.5°C)
  if (temperature < 22.0 || temperature > 37.5) {
    isNormal = false;
  }

  // Check pulse rate range (e.g., normal range: 60 - 100 BPM)
  if (pulseRate < 60 || pulseRate > 100) {
    isNormal = false;
  }

  // Check blood pressure range (e.g., systolic: 90-120, diastolic: 60-80)
  if (systolicBP < 90 || systolicBP > 120 || diastolicBP < 60 || diastolicBP > 80) {
    isNormal = false;
  }

  // Activate LEDs and buzzer based on the conditions
  if (isNormal) {
    digitalWrite(GREEN_LED_PIN, HIGH);  // Turn on green LED
    digitalWrite(RED_LED_PIN, LOW);    // Turn off red LED
    digitalWrite(BUZZER_PIN, LOW);     // Turn off buzzer
  } else {
    digitalWrite(GREEN_LED_PIN, LOW);  // Turn off green LED
    digitalWrite(RED_LED_PIN, HIGH);   // Turn on red LED
    digitalWrite(BUZZER_PIN, HIGH);    // Turn on buzzer
    delay(500);                        // Beep for 500ms
    digitalWrite(BUZZER_PIN, LOW);     // Turn off buzzer
    delay(500);                        // Wait for 500ms
  }

  delay(1000); // Wait for 1 second
}
