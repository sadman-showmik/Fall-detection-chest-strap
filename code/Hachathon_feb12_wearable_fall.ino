#define BLYNK_TEMPLATE_ID "TMPL4S7XRlP1z"
#define BLYNK_TEMPLATE_NAME "Fall detector"
#define BLYNK_AUTH_TOKEN "qNbAnRFwCXbBAn_D3CU-wBo4Vc3pf3Qy"

#include <Wire.h>
#include "DFRobot_BloodOxygen_S.h"
#include <SPI.h>
#include <MPU6050.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>

// WiFi credentials
const char* ssid = "OnePlus Nord 3 5G";
const char* password = "e6espipi";

// Create an instance of the MAX30102 sensor (for heart rate)
#define I2C_ADDRESS 0x57
DFRobot_BloodOxygen_S_I2C MAX30102(&Wire, I2C_ADDRESS);

// Create an instance of the MPU6050 class
MPU6050 mpu;

// Variables for moving average filter
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;

// Variables for fall detection
const float fallThreshold = 1.2;
bool fallDetected = false;

// Heart rate thresholds
const int lowHeartRateThreshold = 50;
const int highHeartRateThreshold = 90;

// Buzzer pin
const int buzzerPin = 6;

void setup() {
  delay(8000); // Allow board and sensors to initialize properly
  
  WiFi.begin(ssid, password);
  // Initialize WiFi connection
  connectToWiFi();

  // Initialize Blynk without blocking
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  // Initialize the MAX30102 sensor (for heart rate)
  if (MAX30102.begin() == false) {
    Serial.println("Failed to initialize the MAX30102 sensor!");
    while (1);
  }
  MAX30102.sensorStartCollect();

  // Initialize the MPU6050 sensor
  Wire.begin();
  delay(500);
  mpu.initialize();
  if (!mpu.testConnection()) {
    while (1); // Stop if sensor is not available
  }

  // Initialize readings array
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  // Initialize buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}
bool eventTrigger = false;  
void loop() {
  Blynk.run();

  // Ensure WiFi stays connected
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // Read heart rate from the MAX30102 sensor
  int heartRate = readHeartRate();

  // Moving average filter for heart rate
  total -= readings[readIndex];
  readings[readIndex] = heartRate;
  total += readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  average = total / numReadings;

  // Send heart rate to Blynk
  Blynk.virtualWrite(V0, average);

  // Check for abnormal heart rate
  if (average < lowHeartRateThreshold) {
    Blynk.virtualWrite(V2, 1);
    eventTrigger=true;
    Blynk.logEvent("bad_heart_rate", "Low heart rate detected!");
  } else if (average > highHeartRateThreshold) {
    Blynk.virtualWrite(V2, 1);
    Blynk.logEvent("bad_heart_rate", "High heart rate detected!");
  } else {
    Blynk.virtualWrite(V2, 0);
  }

  // Read acceleration from MPU6050 for fall detection
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;

  // Calculate acceleration magnitude
  float magnitude = sqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);

  // Fall detection
  if (magnitude > fallThreshold) {
    if (!fallDetected) {
      Blynk.virtualWrite(V1, 1);
      eventTrigger=true;
      Blynk.logEvent("fall_detected", "Fall detected!");
      fallDetected = true;
      tone(buzzerPin, 1000);
    }
  } else {
    fallDetected = false;
    Blynk.virtualWrite(V1, 0);
    noTone(buzzerPin);
  }

  delay(4000);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  
  WiFi.disconnect(); // Ensure previous connections are cleared
  delay(1000);
  
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
  } else {
    Serial.println("\nFailed to connect to WiFi. Restarting...");
    NVIC_SystemReset(); // Reset the board if Wi-Fi fails
  }
}

int readHeartRate() {
  // Read heart rate from the MAX30102 sensor
  MAX30102.getHeartbeatSPO2();
  return MAX30102._sHeartbeatSPO2.Heartbeat;
}