#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>

/*
OtaManager handles all Over-The-Air update transactions.
It periodically fetches a version manifest from a remote server and,
if a newer version is found, downloads and flashes the new firmware.
The device restarts automatically after a successful update.
*/
class OtaManager {
private:
  const char* _versionUrl;
  const char* _firmwareUrl;
  const char* _currentVersion;

  // Stores the remote version string fetched from the server
  static char _remoteVersionBuf[16];

  /* Fetches version.json from the server and returns the version string.
     Returns nullptr if the request fails or the JSON is malformed. */
  const char* fetchRemoteVersion();

  /* Downloads firmware.bin from the server and writes it to the OTA flash partition. */
  void downloadAndFlash();

  /* Reads the HTTP stream and writes it to the flash partition in chunks.
     Prints progress percentage to Serial. */
  void streamToFlash(HTTPClient& http, int totalBytes);

  /* Compares two version strings.
     Returns a positive value if 'a' is newer than 'b', zero if they are equal, and a negative value otherwise. */
  static int compareVersions(const char* a, const char* b);

public:
  OtaManager(const char* versionUrl, const char* firmwareUrl, const char* currentVersion)
    : _versionUrl(versionUrl),
      _firmwareUrl(firmwareUrl),
      _currentVersion(currentVersion) {}

  /* Checks if a newer firmware version is available on the server.
     If so, downloads and flashes it, then restarts the device. */
  void check();
};