Heart Rate & Blood Oxygen Monitor
Author: Mohak Dua (dua00013@umn.edu)

Project Overview
This project is a personal health monitoring system that displays heart rate and blood oxygen levels in real-time on an OLED display. It is powered by the MAX30102 sensor, which measures both pulse (BPM - Beats Per Minute) and SpO₂ (blood oxygen saturation levels).

The system includes:

A buzzer and LED indicator that activate when pulse spikes, simulating an ECG/Pulse monitor.
Cloud functionality, displaying real-time BPM data on a web page via the Particle Console.
Components Used
MAX30102 Pulse and Blood Oxygen Sensor
CVS-2308 Buzzer/Speaker
LED Indicator
SSD1306 OLED Display
Connecting wires
Blood Oxygen (SpO₂) Monitoring
The MAX30102 sensor measures SpO₂ (oxygen saturation levels) using infrared and red light absorption. The algorithm calculates the percentage of oxygenated hemoglobin in the blood, providing an estimate of oxygen levels. This is essential for detecting hypoxia (low oxygen levels), which can be critical for athletes, patients, and individuals with respiratory conditions.

The SpO₂ readings are displayed on the OLED screen, giving real-time health insights.

Project Structure
📂 lib/

OLED Adafruit Libraries:
Adafruit_BusIO_RK
Adafruit_GFX_RK
Adafruit_SSD1306_RK
SparkFun MAX3010x Library
📂 src/

main.cpp → Code to flash onto Particle Photon 2
📂 web/

BPM_Viewer.html → Web-based interface for cloud monitoring
