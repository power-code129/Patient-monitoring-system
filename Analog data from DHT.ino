#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <DHT.h>

// **Pin Definitions**
#define FP_RX 6  // Fingerprint RX
#define FP_TX 7  // Fingerprint TX
#define DHTPIN 2         // DHT11 data pin
#define DHTTYPE DHT11    // Sensor type
#define KY039_PIN A1     // KY-039 Pulse Sensor analog pin
#define AD8232_PIN A0    // AD8232 ECG Sensor analog pin
#define GREEN_LED_PIN 3  // Green LED
#define RED_LED_PIN 4    // Red LED
#define BUZZER_PIN 5     // Buzzer

// **Initialize Components**
SoftwareSerial mySerial(FP_TX, FP_RX);
Adafruit_Fingerprint finger(&mySerial);
DHT dht(DHTPIN, DHTTYPE);

bool sensorActive = false; // Control flag for sensor readings
unsigned long previousMillis = 0; // Timer for delay
const long interval = 5000; // 5-second delay
int currentID = 1; // ID counter for continuous enrollment

void setup() {
  Serial.begin(9600);
  mySerial.begin(57600);
  finger.begin(57600);
  dht.begin();

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  if (finger.verifyPassword()) {
    Serial.println("✅ Fingerprint sensor detected!");
  } else {
    Serial.println("❌ Fingerprint sensor NOT detected! Check wiring.");
    while (1);
  }

  Serial.println("\nCommands:");
  Serial.println("1 - Start Continuous Fingerprint Enrollment");
  Serial.println("2 - Start Sensor Readings");
  Serial.println("3 - Stop Sensor Readings");
  Serial.println("4 - Clear All Fingerprints");
}

void loop() {
  if (Serial.available()) {
    char choice = Serial.read();
    switch (choice) {
      case '1': enrollFingerprintContinuously(); break;
      case '2': sensorActive = true; Serial.println("✅ Sensor Readings Started"); break;
      case '3': stopSensorReadings(); break;
      case '4': clearFingerprintDatabase(); break;
      default: Serial.println("Invalid choice. Enter 1, 2, 3, or 4.");
    }
  }

  if (sensorActive) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      displaySensorData();
    }
  }
}

// ✅ **Continuously Enroll Fingerprints**
void enrollFingerprintContinuously() {
  Serial.print("Enrolling ID #"); Serial.println(currentID);

  delay(1000);
  Serial.println("👉 Place your finger on the sensor...");

  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("❌ Failed to process image.");
    return;
  }

  Serial.println("✅ First scan complete. Remove your finger.");
  delay(2000);

  Serial.println("👉 Place the same finger again...");
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("❌ Second scan failed.");
    return;
  }

  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println("❌ Failed to create model.");
    return;
  }

  if (finger.storeModel(currentID) == FINGERPRINT_OK) {
    Serial.println("🎉 Fingerprint enrolled successfully!");
    currentID++;
  } else {
    Serial.println("❌ Failed to store fingerprint.");
  }
}

// ✅ **Stop Sensor Readings**
void stopSensorReadings() {
  Serial.println("\n🛑 Stopping sensor readings.");
  sensorActive = false;
}

// ✅ **Clear All Fingerprints**
void clearFingerprintDatabase() {
  Serial.println("\n⚠️ Clearing all fingerprints...");
  if (finger.emptyDatabase() == FINGERPRINT_OK) {
    Serial.println("✅ All fingerprints deleted.");
    currentID = 1; // Reset ID counter
  } else {
    Serial.println("❌ Failed to delete fingerprints.");
  }
}

// ✅ **Display Sensor Data**
void displaySensorData() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) return;

  int pulseRate = map(analogRead(KY039_PIN), 0, 1023, 60, 100);
  int ecgValue = analogRead(AD8232_PIN);
  int systolicBP = map(ecgValue, 0, 1023, 90, 120);
  int diastolicBP = map(ecgValue, 0, 1023, 60, 80);

  Serial.print(temperature, 2);
  Serial.print(",");
  Serial.print(pulseRate);
  Serial.print(",");
  Serial.print(systolicBP);
  Serial.print("/");
  Serial.println(diastolicBP);

  bool isNormal = (temperature >= 22.0 && temperature <= 37.5 &&
                   pulseRate >= 60 && pulseRate <= 100 &&
                   systolicBP >= 90 && systolicBP <= 120 &&
                   diastolicBP >= 60 && diastolicBP <= 80);

  digitalWrite(GREEN_LED_PIN, isNormal ? HIGH : LOW);
  digitalWrite(RED_LED_PIN, isNormal ? LOW : HIGH);
  digitalWrite(BUZZER_PIN, isNormal ? LOW : HIGH);
}
