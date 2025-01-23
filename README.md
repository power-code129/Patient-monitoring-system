# Patient-monitoring-system


## **Overview**
This program reads and processes data from three health-monitoring sensors (DHT11, KY-039, and AD8232) connected to an ESP8266 or Arduino. Based on the readings, it performs health checks and activates LEDs and a buzzer to indicate whether the readings are within normal ranges.

## **Components and Purpose**
1. **DHT11 Sensor**: Measures temperature.
2. **KY-039 Sensor**: Measures pulse rate (heartbeats per minute).
3. **AD8232 Sensor**: Measures ECG signals, which are used to estimate blood pressure (systolic and diastolic).
4. **Green LED**: Indicates normal health parameters.
5. **Red LED and Buzzer**: Indicate abnormal health parameters.

## **Code Breakdown**

### **1. Include Libraries and Pin Definitions**
- The `DHT.h` library is included to interact with the DHT11 sensor.
- The pins for the sensors and output components (LEDs and buzzer) are defined:
  - **DHTPIN (2)**: DHT11 data pin.
  - **KY039_PIN (A1)**: KY-039 Pulse Sensor connected to analog pin A1.
  - **AD8232_PIN (A0)**: AD8232 ECG Sensor connected to analog pin A0.
  - **LED and Buzzer Pins**: Control green LED, red LED, and buzzer.

### **2. Setup Function**
- **Serial Communication**: Initialized at `9600 baud` to send data to the serial monitor.
- **Sensor Initialization**: The DHT sensor is started using `dht.begin()`.
- **Pin Modes**: All LEDs and the buzzer are set as output, and their initial states are set to OFF.

### **3. Loop Function**
The main logic runs continuously in the `loop()` function:

#### **a. Read Temperature**
- The temperature is read using the `dht.readTemperature()` function.
- If the reading fails (`isnan` returns true), the program skips the current loop cycle to avoid invalid data processing.

#### **b. Read Pulse Rate**
- The raw signal from the KY-039 sensor is read using `analogRead(KY039_PIN)`.
- The raw value (0-1023) is mapped to a heart rate range (60-100 BPM).

#### **c. Read ECG and Estimate Blood Pressure**
- The ECG signal is read from the AD8232 sensor using `analogRead(AD8232_PIN)`.
- The raw ECG signal (0-1023) is mapped to approximate systolic (90-120 mmHg) and diastolic (60-80 mmHg) blood pressure ranges.


### **4. Output the Data**
- Data is formatted as `temperature,pulseRate,systolicBP/diastolicBP` and sent to the serial monitor:
  - Example output: `36.5,72,110/75`

### **5. Health Check Conditions**
- The program checks if the sensor readings fall within normal health ranges:
  - **Temperature**: 36.0°C to 37.5°C.
  - **Pulse Rate**: 60 to 100 BPM.
  - **Blood Pressure**:
    - Systolic: 90 to 120 mmHg.
    - Diastolic: 60 to 80 mmHg.

- If all values are normal:
  - **Green LED**: Turns ON.
  - **Red LED and Buzzer**: Turn OFF.

- If any value is abnormal:
  - **Red LED**: Turns ON.
  - **Buzzer**: Beeps (ON for 500ms, OFF for 500ms).
  - **Green LED**: Turns OFF.

### **6. Timing**
- The loop includes a `delay(1000)` to wait for 1 second before the next iteration. This ensures that the sensors have enough time to stabilize and provide accurate readings.


## **Demonstration Plan**
1. **Explain the Purpose**: Monitor and display health parameters using sensors.
2. **Show Hardware Setup**: Point out the DHT11, KY-039, AD8232 sensors, LEDs, and buzzer.
3. **Run the Program**:
   - Display real-time data in the serial monitor.
   - Simulate abnormal conditions (e.g., change temperature or pulse inputs) to show LED and buzzer activation.
4. **Highlight Features**:
   - Real-time health monitoring.
   - Immediate feedback through LEDs and buzzer.

## **Key Takeaways**
- This program is a simple health-monitoring system.
- It collects real-time data, evaluates it against normal ranges, and provides visual and auditory feedback.
- The modular design makes it easy to expand with additional sensors or features in the future. 

