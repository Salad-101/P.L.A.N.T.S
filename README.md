# 🌱 IoT Smart Plant Monitoring System

An IoT-based plant monitoring system developed as a **senior-year high school capstone project**. The system monitors environmental conditions around a plant using an ESP32 and automatically provides alerts when temperature, humidity, or soil moisture falls outside predefined ranges.

Sensor readings are collected periodically and uploaded to cloud services for remote monitoring and historical data tracking.

## 📌 Project Overview

The system combines an **ESP32 microcontroller**, environmental sensors, cloud services, and notification mechanisms to create a simple smart plant monitoring solution.

It continuously measures:

* 🌡️ Temperature
* 💧 Air humidity
* 🌱 Soil moisture

Based on configurable thresholds, the system can also activate:

* 💡 An LED when the temperature is outside the desired range
* 🔊 A buzzer when the humidity is outside the desired range
* 📱 Telegram notifications when soil moisture becomes too low or too high

The collected data is additionally sent to **ThingSpeak** and a **Google Spreadsheet**, allowing the measurements to be monitored and stored remotely.

---

## ✨ Features

* Real-time temperature monitoring using a **DHT22**
* Air humidity monitoring
* Analog soil moisture measurement
* Configurable environmental thresholds
* LED-based temperature warning
* Buzzer-based humidity warning
* Telegram alerts for abnormal soil moisture
* ThingSpeak data logging
* Google Spreadsheet data logging
* Automatic NTP time synchronization
* Wi-Fi connectivity through the ESP32
* Serial monitor logging for debugging and monitoring

---

## 🧰 Hardware

| Component            | Purpose                                     |
| -------------------- | ------------------------------------------- |
| ESP32                | Main microcontroller and Wi-Fi connectivity |
| DHT22                | Temperature and humidity measurement        |
| Soil Moisture Sensor | Measures soil moisture level                |
| LED                  | Temperature warning indicator               |
| Buzzer               | Humidity warning indicator                  |

### Pin Configuration

| Component            | ESP32 Pin |
| -------------------- | --------: |
| DHT22                |   GPIO 33 |
| LED                  |    GPIO 4 |
| Buzzer               |    GPIO 2 |
| Soil Moisture Sensor |   GPIO 34 |

---

## 💻 Software & Technologies

* **C++**
* **Arduino Framework**
* **ESP32**
* **DHT Sensor Library**
* **WiFi**
* **HTTPClient**
* **ThingSpeak**
* **Google Apps Script**
* **Telegram Bot API**
* **NTP**

---

## ⚙️ How It Works

The ESP32 follows a continuous monitoring cycle.

```text
             ┌─────────────────┐
             │     ESP32       │
             └────────┬────────┘
                      │
          ┌───────────┼───────────┐
          │           │           │
          ▼           ▼           ▼
      ┌───────┐   ┌───────┐   ┌───────────┐
      │ DHT22 │   │ Soil  │   │ Threshold │
      │Sensor │   │Sensor │   │  Checks   │
      └───┬───┘   └───┬───┘   └─────┬─────┘
          │           │             │
          └───────────┼─────────────┘
                      │
              ┌───────▼────────┐
              │ Process & Log  │
              │    Readings    │
              └───────┬────────┘
                      │
        ┌─────────────┼──────────────┐
        │             │              │
        ▼             ▼              ▼
   ThingSpeak     Google Sheets   Telegram
        │             │              │
        └─────────────┼──────────────┘
                      │
                      ▼
                 Next Cycle
```

### Monitoring Cycle

Every approximately **30 seconds**, the ESP32:

1. Reads the soil moisture sensor.
2. Converts the analog reading into an estimated percentage.
3. Reads temperature and humidity from the DHT22.
4. Sends the measurements to ThingSpeak.
5. Sends the measurements to a Google Spreadsheet through a Google Apps Script endpoint.
6. Synchronizes and displays the current date and time.
7. Checks the soil moisture against its configured thresholds.
8. Sends a Telegram warning if the soil moisture is outside the acceptable range.
9. Checks the temperature and controls the LED accordingly.
10. Checks the humidity and controls the buzzer accordingly.
11. Prints a summary of the readings to the Serial Monitor.
12. Waits until the next monitoring cycle.

---

## 📊 Default Thresholds

The project uses the following default values:

| Measurement   | Minimum | Maximum |
| ------------- | ------: | ------: |
| Temperature   |    20°C |    25°C |
| Humidity      |     40% |     70% |
| Soil Moisture |     50% |     80% |

These values can be modified directly in the source code.

---

## ☁️ Data Collection

### ThingSpeak

Sensor measurements are uploaded to ThingSpeak using HTTP requests.

The following fields are used:

| Field   | Data          |
| ------- | ------------- |
| Field 1 | Temperature   |
| Field 2 | Humidity      |
| Field 3 | Soil Moisture |
| Field 4 | LED Status    |
| Field 5 | Buzzer Status |

This makes it possible to visualize and track environmental measurements over time.

### Google Spreadsheet

The ESP32 also sends the sensor readings to a Google Apps Script web application.

The request contains:

```text
Temperature
Humidity
Soil Moisture
LED Status
Buzzer Status
```

This provides a simple way to maintain a historical record of the collected data.

---

## 📱 Telegram Notifications

A Telegram bot is used to notify the user when soil moisture falls outside the configured range.

For example:

```text
Warning: Soil moisture is below threshold!
Moisture level is: 32%
```

The ESP32 sends the notification through the Telegram Bot API over Wi-Fi.

---

## 🔔 Local Alerts

The system provides two physical indicators.

### Temperature Alert

The LED turns on when:

```text
Temperature > 25°C
```

or:

```text
Temperature < 20°C
```

The LED turns off when the temperature returns to the acceptable range.

### Humidity Alert

The buzzer turns on when:

```text
Humidity > 70%
```

or:

```text
Humidity < 40%
```

The buzzer turns off when humidity returns to the acceptable range.

---

## 🕐 Time Synchronization

The ESP32 synchronizes its clock using an NTP server:

```text
pool.ntp.org
```

The project is configured for **UTC+2** with no daylight-saving offset.

The synchronized time is displayed through the Serial Monitor and can be used alongside the recorded sensor data.

---

## 🚀 Setup

### 1. Install the Required Libraries

Install the following Arduino libraries:

* DHT sensor library
* ESP32 board support

The `WiFi` and `HTTPClient` functionality is provided by the ESP32 Arduino environment.

### 2. Configure Wi-Fi

Update the following values in the source code:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. Configure Cloud Services

Update the credentials and endpoints for:

* Telegram
* ThingSpeak
* Google Apps Script

### 4. Configure Thresholds

Adjust the environmental thresholds if necessary:

```cpp
float maxtempthreshold = 25.0;
float mintempthreshold = 20.0;

float maxhumthreshold = 70;
float minhumthreshold = 40;

int maxsoilMoistureThreshold = 80;
int minsoilMoistureThreshold = 50;
```

### 5. Upload to the ESP32

Open the project in the Arduino IDE or another compatible ESP32 development environment, select the appropriate ESP32 board and serial port, and upload the firmware.

---

## 🔐 Security Note

**Do not commit real credentials or API tokens to GitHub.**

The original project code contains credentials for services such as Wi-Fi, Telegram, and ThingSpeak. Before publishing this project publicly, replace them with placeholders and revoke/regenerate any credentials that were previously exposed.

For example:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String botToken = "YOUR_TELEGRAM_BOT_TOKEN";
String chatID = "YOUR_TELEGRAM_CHAT_ID";
String apiKey = "YOUR_THINGSPEAK_API_KEY";
```

For a production implementation, these values should ideally be stored outside the source code rather than hard-coded.

---

## 📁 Project Structure

A simple version of the project can be organized as:

```text
smart-plant-monitor/
├── README.md
└── smart_plant_monitor.ino
```

---

## 🎓 Project Background

This project was developed as my **senior-year high school capstone project**.

The goal was to combine embedded systems, sensors, networking, cloud services, and automation into a single practical IoT application.

Although the project was created as an educational project, it introduced several concepts that are useful in larger software and IoT systems:

* Embedded programming
* Sensor data acquisition
* Analog-to-digital conversion
* Network communication
* REST/HTTP APIs
* Cloud data logging
* Automated alerts
* Threshold-based decision making
* Hardware/software integration

---

## 🔮 Possible Future Improvements

Some possible improvements to the original implementation include:

* Add a web dashboard for real-time monitoring
* Replace hard-coded credentials with secure configuration
* Store configuration and thresholds remotely
* Add automatic plant watering using a water pump
* Add multiple soil moisture sensors
* Add support for multiple plants
* Implement persistent local data storage
* Improve sensor calibration
* Prevent repeated Telegram notifications for the same condition
* Add a proper error-recovery mechanism for Wi-Fi disconnections
* Replace blocking `delay()` calls with a non-blocking scheduling approach
* Add OTA firmware updates
* Build a dedicated mobile or web interface

---

## 📜 License

This project was created as an educational capstone project. Feel free to use the code for learning and experimentation.
