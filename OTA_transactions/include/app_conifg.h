#pragma once

// Connnection to Wi-Fi network:
#define WIFI_SSID     "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// Connection to the server providing the version and firmware files:
#define VERSION_URL   "http://192.168.1.100:8080/version.json"
#define FIRMWARE_URL  "http://192.168.1.100:8080/firmware.bin"

// Current application version:
#define APP_VERSION   "1.0.0"

// Update interval:
#define OTA_INTERVAL  15000   // ms