#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;

// TODO: Replace YOUR_AP_NAME
const char* ssid = "YOUR_AP_NAME";
// TODO: Replace AP_PASSWORD
const char* password = "AP_PASSWORD";

long subscribersCount = 0;

LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
  pinMode(D5, OUTPUT);

  Wire.begin(D2, D1);
  lcd.begin();
  lcd.home();
  lcd.print("Connecting...");
  
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
}

void loop() {
  
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    // TODO: Replace PUT_YOUR_BOT_TOKEN_HERE and CHANNEL_NAME
    http.begin("https://api.telegram.org/botPUT_YOUR_BOT_TOKEN_HERE/getChatMembersCount?chat_id=@CHANNEL_NAME", "BB DC 45 2A 07 E3 4A 71 33 40 32 DA BE 81 F7 72 6F 4A 2B 6B"); //HTTPS
    
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);

        StaticJsonBuffer<200> jsonBuffer;
        // Root of the object tree.
        //
        // It's a reference to the JsonObject, the actual bytes are inside the
        // JsonBuffer with all the other nodes of the object tree.
        // Memory is freed when jsonBuffer goes out of scope.
        JsonObject& root = jsonBuffer.parseObject(payload);
        // Test if parsing succeeds.
        if (!root.success()) {
          Serial.println("parseObject() failed");
          return;
        }
        long tempCount = root["result"];
        Serial.print("New count = ");
        Serial.println(tempCount);
        if (subscribersCount != tempCount) {
          if (tempCount > subscribersCount) {
            tone (D5, 500);
            delay(500);
            tone (D5, 0);
            showSubscribersCount(tempCount);
          } else if (tempCount < subscribersCount) {
            tone (D5, 1000);
            delay(500);
            tone (D5, 0);
            showSubscribersCount(tempCount);
          }
          subscribersCount = tempCount;
        }
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(1000);
}

void showSubscribersCount(int count) {
  lcd.clear();
  lcd.print("Subscribers num:");
  lcd.setCursor(0, 1);
  lcd.print(count);
}

