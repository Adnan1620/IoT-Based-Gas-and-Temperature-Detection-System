/****************************************************
 🌫 Air Quality Monitor with Blynk IoT + Notifications + Serial Alerts
 Includes DHT11 (Temp + Humidity) and MQ2 Gas Sensor
 Two LEDs:
  - LED_GAS: Air Quality Alert
  - LED_TEMP: Temperature/Humidity Alert
****************************************************/

#define BLYNK_TEMPLATE_ID "TMPL3Iu7csbQS"
#define BLYNK_TEMPLATE_NAME "Air Quality"
#define BLYNK_AUTH_TOKEN "eHQytNAo5tk75GqOw6GY3BmwhpuVYhX7"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// Wi-Fi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Suraj";
char pass[] = "Suraj1234";

// Pin setup
#define DHTPIN D4
#define DHTTYPE DHT11
#define GASPIN A0
#define LED_GAS D1
#define LED_TEMP D2

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Flags to avoid repeated notifications
bool gasAlertSent = false;
bool tempAlertSent = false;

void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasValue = analogRead(GASPIN);

  if (isnan(h) || isnan(t)) {
    Serial.println("❌ DHT Reading Failed!");
    return;
  }

  Serial.println("\n----------------------------------");
  Serial.print("🌡 Temperature: "); Serial.print(t); Serial.println(" °C");
  Serial.print("💧 Humidity: "); Serial.print(h); Serial.println(" %");
  Serial.print("🧪 Gas Sensor: "); Serial.println(gasValue);

  // --- GAS QUALITY ALERT ---
  if (gasValue > 160) {
    digitalWrite(LED_GAS, HIGH);
    if (!gasAlertSent) {
      Serial.println("⚠ ALERT: Gas Detected!");
      Blynk.logEvent("air_alert", "⚠ Poor Air Quality Detected!");
      gasAlertSent = true;
    }
  } else {
    digitalWrite(LED_GAS, LOW);
    if (gasAlertSent) {
      Serial.println("✅ Air Quality Back to Normal!");
      gasAlertSent = false;
    } else {
      Serial.println("🌤 Air Quality: Normal");
    }
  }

  // --- TEMPERATURE / HUMIDITY ALERT ---
  if (t > 35 || h > 50) {
    digitalWrite(LED_TEMP, HIGH);
    if (!tempAlertSent) {
      Serial.println("🔥 ALERT: High Temperature/Humidity Detected!");
      Blynk.logEvent("temp_alert", "🌡 High Temperature/Humidity Detected!");
      tempAlertSent = true;
    }
  } else {
    digitalWrite(LED_TEMP, LOW);
    if (tempAlertSent) {
      Serial.println("✅ Temperature/Humidity Back to Normal!");
      tempAlertSent = false;
    } else {
      Serial.println("🌤 Environment: Normal Range");
    }
  }

  // --- Send data to Blynk ---
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, gasValue);

  Serial.println("----------------------------------\n");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_GAS, OUTPUT);
  pinMode(LED_TEMP, OUTPUT);
  pinMode(GASPIN, INPUT);
  dht.begin();

  Serial.println("\n🌫 Air Quality Monitor Initializing...");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
