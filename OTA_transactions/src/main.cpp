#include "app_config.h"
#include "OtaManager.h"
#include <Arduino.h>
#include <WiFi.h>

#define MSG "Hello from firmware 1.0.0!"

OtaManager otaManager(VERSION_URL, FIRMWARE_URL, APP_VERSION);
unsigned long lastOtaCheck = 0;

void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected! IP: %s\n", WiFi.localIP().toString().c_str());
}

void setup() {
  Serial.begin(9600);
  Serial.printf("\n=== Firmware v%s ===\n", APP_VERSION);
  connectWifi();
  otaManager.check();
  lastOtaCheck = millis();
}

void loop() {
  Serial.println(MSG);

  if (millis() - lastOtaCheck >= OTA_INTERVAL) {
    otaManager.check();
    lastOtaCheck = millis();
  }

  delay(3000);
}