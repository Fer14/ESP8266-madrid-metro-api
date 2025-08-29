#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

#define SDA_PIN 13
#define SCL_PIN 14


#define BUTTON_PIN 5

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi credentials
const char* ssid = "WIFI";
const char* password = "PASS";

// EMT API credentials
const char* email = "API MAIL";
const char* pass = "API PASS";

String loginAndGetToken() {
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();                   
  String url = "https://openapi.emtmadrid.es/v2/mobilitylabs/user/login/";
  http.begin(client, url);            
  http.addHeader("email", email);
  http.addHeader("password", pass);

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Login response:");
    Serial.println(payload);

    // Parse JSON
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);
    String token = doc["data"][0]["accessToken"].as<String>();
    http.end();
    return token;
  } else {
    Serial.print("Login failed, HTTP error code: ");
    Serial.println(httpCode);
    Serial.print("Error detail: ");
    Serial.println(http.errorToString(httpCode));
    http.end();
    return "";
  }
}


float getBusEstimate(String token) {
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();  
  String url = "https://openapi.emtmadrid.es/v2/transport/busemtmad/stops/1649/arrives/132/";

  http.begin(client, url); 
  http.addHeader("accessToken", token);
  http.addHeader("Content-Type", "application/json");

  // Prepare body JSON
  String body = "{\"cultureInfo\":\"ES\",\"Text_StopRequired_YN\":\"Y\",\"Text_EstimationsRequired_YN\":\"Y\",\"Text_IncidencesRequired_YN\":\"Y\",\"DateTime_Referenced_Incidencies_YYYYMMDD\":\"2025-08-15\"}";
  
  int httpCode = http.POST(body);
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Estimate response:");
    Serial.println(payload);

    // Parse JSON
    StaticJsonDocument<4096> doc;
    deserializeJson(doc, payload);
    int estimate_s = doc["data"][0]["Arrive"][0]["estimateArrive"];
    http.end();

    return estimate_s / 60.0;
  } else {
    Serial.print("Estimate request failed, HTTP error code: ");
    Serial.println(httpCode);
    Serial.print("Error detail: ");
    Serial.println(http.errorToString(httpCode));
    http.end();
    return -1;
  }
}
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button with pull-up resistor
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press button to");
  lcd.setCursor(0,1);
  lcd.print("start...");
  Serial.println("Waiting for button press...");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) { // Button pressed
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Connecting WiFi");
    lcd.setCursor(0,1);
    lcd.print("please wait...");
    Serial.println("Connecting to WiFi...");
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WiFi connected");

    // Login
    lcd.setCursor(0,1);
    lcd.print("Logging in...");
    Serial.println("Logging in...");
    String token = loginAndGetToken();

    if (token != "") {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Getting estimate...");
      float minutes = getBusEstimate(token);
      Serial.printf("Bus 132 arrives in %.1f min\n", minutes);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Bus 132 arrives:");
      lcd.setCursor(0,1);
      lcd.printf("%.1f min", minutes);
      delay(5000); 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Press button to");
      lcd.setCursor(0,1);
      lcd.print("start...");
      Serial.println("Waiting for button press...");
    } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Login failed");
      Serial.println("Login failed!");
    }

    // Wait until button released to avoid re-running immediately
    while(digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
    }
  }
}