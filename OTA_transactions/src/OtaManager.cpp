#include "OtaManager.h"

char OtaManager::_remoteVersionBuf[16];

const char* OtaManager::fetchRemoteVersion() {
  HTTPClient http;
  http.begin(_versionUrl);

  int httpCode = http.GET(); // Makes the HTTP GET request to fetch the version manifest
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("[OTA] Failed to fetch version.json: %d\n", httpCode);
    http.end();
    return nullptr;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, http.getString());
  http.end();

  if (err) {
    Serial.printf("[OTA] Invalid JSON: %s\n", err.c_str());
    return nullptr;
  }

  // Copy to static buffer — the JsonDocument is destroyed after this method returns
  strlcpy(_remoteVersionBuf, doc["version"] | "", sizeof(_remoteVersionBuf));
  return _remoteVersionBuf;
}

void OtaManager::downloadAndFlash() {
  HTTPClient http;
  http.begin(_firmwareUrl);
  http.setTimeout(30000);

  // Checks if GET request was successful:
  if (http.GET() != HTTP_CODE_OK) {
    Serial.println("[OTA] Failed to download firmware.");
    http.end();
    return;
  }

  int totalBytes = http.getSize(); // Gets the size of the new firmware to be updated
  Serial.printf("[OTA] Firmware size: %d bytes\n", totalBytes);

  // Checks if there is enough space to write the new firmware:
  if (!Update.begin(totalBytes)) {
    Serial.printf("[OTA] Not enough flash space: %s\n", Update.errorString());
    http.end();
    return;
  }

  streamToFlash(http, totalBytes); // Calls the method that reads the HTTP stream and writes it to flash
  http.end();

  if (Update.end()) {
    Serial.println("[OTA] Flash successful! Restarting...");
    delay(1000);
    ESP.restart();
  } else {
    Serial.printf("[OTA] Flash error: %s\n", Update.errorString());
  }
}

void OtaManager::streamToFlash(HTTPClient& http, int totalBytes) {
  WiFiClient* stream = http.getStreamPtr();
  uint8_t buf[1024];
  int written = 0;

  // Reads the HTTP stream in chunks and writes it to flash, while printing progress:
  while (http.connected() && written < totalBytes) {
    if (stream->available()) {
      int n = stream->readBytes(buf, min((int)stream->available(), (int)sizeof(buf))); // Reads a chunk of data from the HTTP stream
      Update.write(buf, n); // Writes the chunk to flash
      written += n;
      Serial.printf("[OTA] %d%%\r", (written * 100) / totalBytes);
    }
    delay(1);
  }
  Serial.println();
}

int OtaManager::compareVersions(const char* a, const char* b) {
  int a1=0, a2=0, a3=0;
  int b1=0, b2=0, b3=0;
  sscanf(a, "%d.%d.%d", &a1, &a2, &a3);
  sscanf(b, "%d.%d.%d", &b1, &b2, &b3);
  if (a1 != b1) return a1 - b1;
  if (a2 != b2) return a2 - b2;
  return a3 - b3;
}

void OtaManager::check() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[OTA] No Wi-Fi connection, skipping check.");
    return;
  }

  // Calls the server to check if there is a new firmware version available:
  const char* remoteVersion = fetchRemoteVersion();
  if (!remoteVersion) return;

  Serial.printf("[OTA] Local: %s | Remote: %s\n", _currentVersion, remoteVersion);

  // Compares the local version with the remote version:
  if (compareVersions(remoteVersion, _currentVersion) <= 0) {
    Serial.println("[OTA] Already on the latest version.");
    return;
  }

  // If the remote version is newer, proceeds to download and flash it:
  Serial.printf("[OTA] New version available: %s\n", remoteVersion);
  downloadAndFlash();
}