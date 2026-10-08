// Included libraries
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Pin and sensor configuration
#define DHTPIN 33
#define LEDPIN 4
#define BUZZER 2
#define DHTTYPE DHT22

// Soil moisture sensor pin
int soilMoisturePin = 34;
int soilMoistureValue = 0;
int Moisturelevel;

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Time configuration
const long gmtOffset_sec = 2 * 3600;
const int daylightOffset_sec = 0;

// Threshold values
float maxtempthreshold = 25.0;
float mintempthreshold = 20.0;
float maxhumthreshold = 70;
float minhumthreshold = 40;
int maxsoilMoistureThreshold = 80;
int minsoilMoistureThreshold = 50;

// IoT and cloud platform configuration
String botToken = "YOUR_BOT_TOKEN";
String chatID = "YOUR_CHAT_ID";
String apiKey = "YOUR_API_KEY";
String server = "http://api.thingspeak.com/update";
String Web_App_URL = "YOUR_SCRIPT_URL";

// Device status
String LEDStatus = "OFF";
String BuzzerStatus = "OFF";

DHT dht(DHTPIN, DHTTYPE);

// Setup function to initialize hardware and connections
void setup() {
  Serial.begin(9600);
  dht.begin();

  Serial.println("\n======= INITIALIZING =======");
  
  // Initialize output pins
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi.");

  // Synchronize time with NTP server
  Serial.println("Synchronizing time with NTP server...");
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for time sync...");
    delay(5000);
  }
  Serial.println("Time synchronized!");
}

// Function to send message to Telegram bot
void sendMessage(String message) {
  Serial.println("\n======= SENDING TELEGRAM MESSAGE =======");
  Serial.print("Message: ");
  Serial.println(message);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + message;

    Serial.print("Connecting to Telegram API: ");
    Serial.println(url);

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("Message sent. HTTP Response Code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Failed to send message. Error Code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected. Cannot send message.");
  }
}

// Function to send data to ThingSpeak
void sendToThingSpeak(int Moisturelevel, float temperature, float humidity, String LEDStatus, String BuzzerStatus) {
  Serial.println("\n======= SENDING DATA TO THINGSPEAK =======");
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Soil Moisture Level: ");
  Serial.println(Moisturelevel);
  Serial.print("LED Status: ");
  Serial.println(LEDStatus);
  Serial.print("Buzzer Status: ");
  Serial.println(BuzzerStatus);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String ledStatusValue = (LEDStatus == "ON") ? "1" : "0";
    String buzzerStatusValue = (BuzzerStatus == "ON") ? "1" : "0";

    String url = server + "?api_key=" + apiKey + "&field1=" + String(temperature) + "&field2=" + String(humidity) + "&field3=" + String(Moisturelevel) + "&field4=" + ledStatusValue + "&field5=" + buzzerStatusValue;

    Serial.print("Connecting to ThingSpeak: ");
    Serial.println(url);

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("Data sent successfully. HTTP Response Code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Failed to send data. Error Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected. Cannot send data to ThingSpeak.");
  }
}

// Loop function to run the code in a loop manner
void loop() {
  Serial.println("\n======= STARTING LOOP =======");

  unsigned long startTime = millis();

  // Reading soil moisture
  soilMoistureValue = analogRead(soilMoisturePin);
  Moisturelevel = map(soilMoistureValue, 0, 4095, 100, 0);
  Serial.print("Soil Moisture Raw Value: ");
  Serial.println(soilMoistureValue);
  Serial.print("Soil Moisture Level (%): ");
  Serial.println(Moisturelevel);

  // Reading temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(30000);
    return;
  }
  Serial.print("Temperature (°C): ");
  Serial.println(temperature);
  Serial.print("Humidity (%): ");
  Serial.println(humidity);

  // Sending data to ThingSpeak
  sendToThingSpeak(Moisturelevel, temperature, humidity, LEDStatus, BuzzerStatus);

  // Sending data to Google Spreadsheet
  if (WiFi.status() == WL_CONNECTED) {
    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL += "&temp=" + String(temperature);
    Send_Data_URL += "&humd=" + String(humidity);
    Send_Data_URL += "&soil=" + String(Moisturelevel);
    Send_Data_URL += "&led=" + LEDStatus;
    Send_Data_URL += "&buzz=" + BuzzerStatus;

    Serial.println("\n======= SENDING DATA TO GOOGLE SPREADSHEET =======");
    HTTPClient http;
    http.begin(Send_Data_URL.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int httpCode = http.GET();
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.println("Failed to send data to Google Spreadsheet.");
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected. Cannot send data to Google Spreadsheet.");
  }

  // Displaying the current time
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.printf("\nCurrent Date and Time: %02d/%02d/%04d %02d:%02d:%02d\n", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  } else {
    Serial.println("Failed to obtain time.");
  }

  // Checking soil moisture thresholds
  if (Moisturelevel < minsoilMoistureThreshold) {
    sendMessage("Warning: Soil moisture is below threshold! Moisture level is: " + String(Moisturelevel) + "%");
    Serial.println("Soil moisture is below threshold!");
  } else if (Moisturelevel > maxsoilMoistureThreshold) {
    sendMessage("Warning: Soil moisture is above threshold! Moisture level is: " + String(Moisturelevel) + "%");
    Serial.println("Soil moisture is above threshold!");
  }

  // Checking temperature thresholds
  if (temperature > maxtempthreshold) {
    digitalWrite(LEDPIN, HIGH);
    LEDStatus = "ON";
    Serial.println("Temperature is above maximum threshold!");
  } else if (temperature < mintempthreshold) {
    digitalWrite(LEDPIN, HIGH);
    LEDStatus = "ON";
    Serial.println("Temperature is below minimum threshold!");
  } else {
    digitalWrite(LEDPIN, LOW);
    LEDStatus = "OFF";
  }

  // Checking humidity thresholds
  if (humidity > maxhumthreshold) {
    digitalWrite(BUZZER, HIGH);
    BuzzerStatus = "ON";
    Serial.println("Humidity is above maximum threshold!");
  } else if (humidity < minhumthreshold) {
    digitalWrite(BUZZER, HIGH);
    BuzzerStatus = "ON";
    Serial.println("Humidity is below minimum threshold!");
  } else {
    digitalWrite(BUZZER, LOW);
    BuzzerStatus = "OFF";
  }

  // Summary log
  Serial.println("\n======= SUMMARY =======");
  Serial.printf("Soil Moisture Level: %d%%\n", Moisturelevel);
  Serial.printf("Temperature: %.2f°C\n", temperature);
  Serial.printf("Humidity: %.2f%%\n", humidity);
  Serial.printf("LED Status: %s\n", LEDStatus.c_str());
  Serial.printf("Buzzer Status: %s\n", BuzzerStatus.c_str());
  Serial.println("-----------------------");

  // Calculating loop duration and delay adjustment
  unsigned long endTime = millis();
  unsigned long elapsedTime = endTime - startTime;

  if (elapsedTime < 30000) {
    delay(30000 - elapsedTime);
  } else {
    Serial.println("Warning: Loop execution time exceeded the delay interval!");
  }

  Serial.println("\n======= END OF LOOP =======");
}