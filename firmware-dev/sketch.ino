#include <WiFi.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

#define DHT_PIN   15
#define LDR_PIN   34
#define SOIL_PIN  35

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
}

void setup() {
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22); // or DHTesp::DHT11
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  connectWiFi();
  lcd.print("WiFi OK");
}

void loop() {
  TempAndHumidity t = dht.getTempAndHumidity();
  int ldr = analogRead(LDR_PIN);
  int soil = analogRead(SOIL_PIN);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:"); lcd.print(t.temperature,1);
  lcd.print(" H:"); lcd.print(t.humidity,0);
  lcd.setCursor(0,1);
  lcd.print("L:"); lcd.print(ldr);
  lcd.print(" S:"); lcd.print(soil);

  // TODO: send to cloud (HTTP/MQTT). For quick tests, ThingSpeak works well.
  // e.g., see an ESP32 + DHT22 + ThingSpeak example for reference.

  delay(2000);
}
