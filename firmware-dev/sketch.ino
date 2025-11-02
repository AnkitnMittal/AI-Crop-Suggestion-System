#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

#define DHT_PIN 15
#define LDR_PIN 34
#define SOIL_PIN 35

const char* WIFI_SSID = "CHENAB";
const char* WIFI_PASS = "44zMf3QqdU&KC3Mv";

// ---- Supabase Configuration ----
const char* SUPABASE_URL = "https://fwnsxlojxqvdwaseomhs.supabase.co/rest/v1/environment_data";
const char* SUPABASE_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImZ3bnN4bG9qeHF2ZHdhc2VvbWhzIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NjE5MjQ4MjgsImV4cCI6MjA3NzUwMDgyOH0.ooP4Vp_m08uGv1xuqOxE4q4KqhwEzOb1XpforRRDUbA";

DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool wifiConnected = false;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  lcd.setCursor(0, 0);
  lcd.print("WiFi...");
  Serial.print("Connecting to WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    lcd.print(".");
    Serial.print(".");
    attempts++;
  }

  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    lcd.print("WiFi OK!");
    Serial.println("\nWiFi Connected!");
    Serial.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    lcd.print("WiFi Failed!");
    Serial.println("\nWiFi Failed! Skipping...");
  }

  delay(1500);
  lcd.clear();
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  connectWiFi();
  dht.setup(DHT_PIN, DHTesp::DHT11);

  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
}

void sendToSupabase(float temp, float hum, int light, int soil) {
  if (!wifiConnected) {
    Serial.println("WiFi not connected, skipping Supabase POST.");
    return;
  }

  HTTPClient http;
  http.begin(SUPABASE_URL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", SUPABASE_KEY);
  http.addHeader("Authorization", String("Bearer ") + SUPABASE_KEY);

  StaticJsonDocument<256> doc;
  doc["device_id"] = "esp32_01";
  doc["temperature"] = temp;
  doc["humidity"] = hum;
  doc["light"] = light;
  doc["soil"] = soil;

  String json;
  serializeJson(doc, json);

  int httpResponseCode = http.POST(json);

  if (httpResponseCode > 0) {
    Serial.print("Supabase Response Code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error in POST: ");
    Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
}

void loop() {
  TempAndHumidity data = dht.getTempAndHumidity();
  float temperature = data.temperature;
  float humidity = data.humidity;

  int lightValue = analogRead(LDR_PIN);
  int soilValue = analogRead(SOIL_PIN);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  lcd.print("L:");
  lcd.print(lightValue);
  lcd.print(" S:");
  lcd.print(soilValue);

  Serial.printf("Temp: %.1f  Hum: %.1f  Light: %d  Soil: %d\n", temperature, humidity, lightValue, soilValue);
  sendToSupabase(temperature, humidity, lightValue, soilValue);

  delay(10000);
}
