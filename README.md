# Fall-detection-chest-strap
IoT-enabled wearable device for fall detection and heart rate monitoring using Arduino MKR WiFi 1010, MAX30102, and MPU6050 with Blynk integration.

## 📌 Project Overview  
This project is a **wearable health monitoring system** designed to detect falls and measure heart rate (bpm) & blood oxygen (SpO₂) in real time.  
It uses **IoT connectivity (Blynk platform)** to send alerts and display live data remotely, making it suitable for elderly care, health monitoring, and personal safety applications.  

## ✨ Key Features  
- ✅ Fall Detection using **MPU6050 accelerometer & gyroscope**  
- ❤️ Heart Rate & SpO₂ Monitoring with **MAX30102 sensor**  
- 📡 IoT Integration with **Blynk** to send live data and trigger notifications  
- 🔔 Real-time Alerts:  
  - Fall event → Alarm, Buzzer & Blynk notification  
  - Low/High heart rate → IoT alert & buzzer  
- 📊 Data Visualization on **Blynk mobile app dashboard**  

## 🛠️ Hardware Components  
- Arduino MKR WiFi 1010 (WiFi-enabled microcontroller)  
- MPU6050 – Accelerometer & Gyroscope (fall detection)  
- MAX30102 – Heart rate & SpO₂ sensor  
- Buzzer & LED – Alert system  
- WiFi Network – IoT connectivity  

## 🔗 Software & Tools  
- Embedded C/C++ (Arduino IDE)  
- Blynk IoT Platform for visualization & notifications  
- WiFiNINA library for connectivity  
- DFRobot Blood Oxygen Sensor library for MAX30102  
- MPU6050 library for motion sensing  

## 🧮 How It Works  
### Fall Detection  
- MPU6050 reads acceleration values  
- Magnitude of acceleration vector is calculated  
- If it exceeds the threshold → a fall event is triggered  

### Heart Rate Monitoring  
- MAX30102 continuously measures heartbeats and SpO₂  
- Moving average filter smoothens heart rate readings  
- If bpm < 50 or > 90 → alert triggered  

### IoT Data Display  
- Data sent to **Blynk dashboard** via WiFi  
- V0 → Heart Rate  
- V1 → Fall Detection  
- V2 → Alerts  

### Alert System  
- Buzzer + LED alarm when abnormal condition detected  
- Remote push notifications via Blynk  

## 🚨 Applications  
- Elderly care & fall prevention systems  
- Remote patient health monitoring  
- Sports & fitness wearable devices  
- IoT-based healthcare research  

## 📸 System Demo (to be added)  
- Circuit schematic / Fritzing diagram  
- Screenshots of Blynk dashboard  
