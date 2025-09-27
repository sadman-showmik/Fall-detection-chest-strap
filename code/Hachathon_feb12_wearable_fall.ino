#define BLYNK_TEMPLATE_ID "TMPL4S7XRlP1z"
#define BLYNK_TEMPLATE_NAME "Fall detector"
#define BLYNK_AUTH_TOKEN "qNbAnRFwCXbBAn_D3CU-wBo4Vc3pf3Qy"

#include <Wire.h>
#include "DFRobot_BloodOxygen_S.h"
#include <MPU6050.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>

// WiFi credentials
const char* ssid = "AIoT-5GmmWave_IoT";
const char* password = "AIoT-2024";

// Create an instance of the derived class for the blood oxygen sensor
#define I2C_ADDRESS 0x57
DFRobot_BloodOxygen_S_I2C MAX30102(&Wire, I2C_ADDRESS);

// Create an instance of the MPU6050 class
MPU6050 mpu;

// Variables for the moving average filter
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;

// Variables for fall detection
const float fallThreshold = 1.2; // Lower threshold for more sensitivity
bool fallDetected = false;

// Variables for heart rate alert
const int lowHeartRateThreshold = 50; // Lower threshold for heart rate
const int highHeartRateThreshold = 90; // Upper threshold for heart rate

// Buzzer pin
const int buzzerPin = 6;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize WiFi
  connectToWiFi();
  delay(5000); // Small delay to ensure WiFi connection is stable

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Initialize the blood oxygen sensor
  if (MAX30102.begin() == false) {
    Serial.println("Failed to initialize the blood oxygen sensor!");
    while (1);
  }
  Serial.println("Blood oxygen sensor initialized successfully.");
  MAX30102.sensorStartCollect();

  // Initialize the MPU6050 sensor
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("Failed to initialize the MPU6050 sensor!");
    while (1);
  }
  Serial.println("MPU6050 sensor initialized successfully.");

  // Initialize all readings to 0
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  // Initialize the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // Ensure the buzzer is off initially
}

void loop() {
  Blynk.run();

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // Read heart rate and SpO2
  MAX30102.getHeartbeatSPO2();

  // Subtract the last reading
  total = total - readings[readIndex];
  // Read the current heart rate
  readings[readIndex] = MAX30102._sHeartbeatSPO2.Heartbeat;
  // Add the current reading to the total
  total = total + readings[readIndex];
  // Advance to the next position in the array
  readIndex = readIndex + 1;

  // If we're at the end of the array, wrap around to the beginning
  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  // Calculate the average
  average = total / numReadings;

  // Print the heart rate and SpO2 values to the Serial Monitor
  Serial.print("Heart Rate: ");
  Serial.print(average);
  Serial.print(" bpm, SpO2: ");
  Serial.print(MAX30102._sHeartbeatSPO2.SPO2);
  Serial.println(" %");

  // Publish heart rate to Blynk
  Serial.println("Sending heart rate to Blynk...");
  Blynk.virtualWrite(V0, average); // Using V0 for heart rate

  // Check for low or high heart rate
  if (average < lowHeartRateThreshold) {
    Serial.println("Low heart rate detected!");
    Blynk.virtualWrite(V2, 1); // Send alarm to V2 for low heart rate
    Blynk.logEvent("low_heart_rate", "Low heart rate detected!");
  } else if (average > highHeartRateThreshold) {
    Serial.println("High heart rate detected!");
    Blynk.virtualWrite(V2, 1); // Send alarm to V2 for high heart rate
    Blynk.logEvent("high_heart_rate", "High heart rate detected!");
  } else {
    Blynk.virtualWrite(V2, 0); // Reset heart rate alarm
  }

  // Read acceleration values from the MPU6050
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;

  // Calculate the magnitude of the acceleration vector
  float magnitude = sqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);

  // Check if the magnitude exceeds the fall threshold
  if (magnitude > fallThreshold) {
    if (!fallDetected) {
      Serial.println("Fall detected!");
      Blynk.virtualWrite(V1, 1); // Using V1 for fall detection notification
      Blynk.logEvent("fall_detected", "Fall detected!");
      fallDetected = true;
      tone(buzzerPin, 1000); // Turn on the buzzer with 1KHz tone
      Serial.println("Buzzer ON");
    }
  } else {
    fallDetected = false;
    Blynk.virtualWrite(V1, 0); // Reset fall detection notification
    noTone(buzzerPin); // Turn off the buzzer
    Serial.println("Buzzer OFF");
  }

  // The sensor updates the data every 4 seconds
  delay(4000);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 10) {
    delay(1000);
    Serial.print(".");
    retryCount++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}
