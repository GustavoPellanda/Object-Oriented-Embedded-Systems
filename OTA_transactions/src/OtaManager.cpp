#include "OtaManager.h"

char OtaManager::_remoteVersionBuf[16];

const char* OtaManager::fetchRemoteVersion() {
  HTTPClient http;
  http.begin(_versionUrl);

  if (http.GET() != HTTP_CODE_OK) {
    Serial.printf("[OTA] Failed to fetch version.json: %d\n", http.GET());
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

  if (http.GET() != HTTP_CODE_OK) {
    Serial.println("[OTA] Failed to download firmware.");
    http.end();
    return;
  }

  int totalBytes = http.getSize();
  Serial.printf("[OTA] Firmware size: %d bytes\n", totalBytes);

  if (!Update.begin(totalBytes)) {
    Serial.printf("[OTA] Not enough flash space: %s\n", Update.errorString());
    http.end();
    return;
  }

  streamToFlash(http, totalBytes);
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

  while (http.connected() && written < totalBytes) {
    if (stream->available()) {
      int n = stream->readBytes(buf, min((int)stream->available(), (int)sizeof(buf)));
      Update.write(buf, n);
      written += n;
      Serial.printf("[OTA] %d%%\r", (written * 100) / totalBytes);
    }
    delay(1);
  }
  Serial.println();
}

void OtaManager::check() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[OTA] No Wi-Fi connection, skipping check.");
    return;
  }

  const char* remoteVersion = fetchRemoteVersion();
  if (!remoteVersion) return;

  Serial.printf("[OTA] Local: %s | Remote: %s\n", _currentVersion, remoteVersion);

  if (strcmp(remoteVersion, _currentVersion) == 0) {
    Serial.println("[OTA] Already on the latest version.");
    return;
  }

  Serial.printf("[OTA] New version available: %s\n", remoteVersion);
  downloadAndFlash();
}