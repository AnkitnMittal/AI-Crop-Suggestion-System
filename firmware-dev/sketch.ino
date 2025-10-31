#include <WiFi.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

#define DHT_PIN 15   // DHT data pin
#define LDR_PIN 34   // LDR analog pin
#define SOIL_PIN 35  // Potentiometer / soil sensor pin

const char* WIFI_SSID = "YourWiFiName";  // <-- Replace with your WiFi name
const char* WIFI_PASS = "YourPassword";  // <-- Replace with your WiFi password

DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool wifiConnected = false;  // Track WiFi state

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

  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print("  Hum: "); Serial.print(humidity);
  Serial.print("  LDR: "); Serial.print(lightValue);
  Serial.print("  Soil: "); Serial.println(soilValue);

  delay(2000);
}
