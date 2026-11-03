#include <HTTPHandler.h>

void handleRoot() {
  server.send(
    200,
    "text/plain",
    "ESP32 Wi-Fi Provisioning Server"
  );
}

void handleScan() {
  Serial.println("Scanning Wi-Fi networks...");

  int count = WiFi.scanNetworks();

  JsonDocument doc;
  JsonArray networks = doc["networks"].to<JsonArray>();

  for (int i = 0; i < count; i++) {
    JsonObject network = networks.add<JsonObject>();

    network["ssid"] = WiFi.SSID(i);
    network["rssi"] = WiFi.RSSI(i);
    network["channel"] = WiFi.channel(i);
    network["secure"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    network["bssid"] = WiFi.BSSIDstr(i);
  }

  String response;
  serializeJson(doc, response);

  server.send(200, "application/json", response);

  WiFi.scanDelete();
}

void handleConnect() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json",
                "{\"error\":\"Missing request body\"}");
    return;
  }

  JsonDocument doc;

  DeserializationError error =
    deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "application/json",
                "{\"error\":\"Invalid JSON\"}");
    return;
  }

  const char* ssid = doc["ssid"];
  const char* password = doc["password"];

  if (ssid == nullptr || password == nullptr ||
      strlen(ssid) == 0) {
    server.send(400, "application/json",
                "{\"error\":\"SSID and password required\"}");
    return;
  }

  pendingSSID = String(ssid);
  pendingPassword = String(password);

  connectionPending = true;
  credentialsSaved = false;
  connectionStartTime = millis();

  WiFi.disconnect(false, false);
  WiFi.begin(pendingSSID.c_str(), pendingPassword.c_str());

  Serial.print("Attempting connection to: ");
  Serial.println(pendingSSID);

  server.send(202, "application/json",
              "{\"status\":\"connecting\"}");
}

void handleStatus() {
  JsonDocument doc;

  if (WiFi.status() == WL_CONNECTED) {
    doc["status"] = "connected";
    doc["ssid"] = WiFi.SSID();
    doc["ip"] = WiFi.localIP().toString();
    doc["rssi"] = WiFi.RSSI();

    // Save credentials only after successful connection.
    if (connectionPending && !credentialsSaved) {
      preferences.begin("wifi-config", false);
      preferences.putString("ssid", pendingSSID);
      preferences.putString("password", pendingPassword);
      preferences.end();

      credentialsSaved = true;
      connectionPending = false;

      Serial.println("Wi-Fi credentials saved.");
    }
  } else if (connectionPending) {
    if (millis() - connectionStartTime > WIFI_TIMEOUT_MS) {
      connectionPending = false;

      doc["status"] = "failed";
      doc["message"] = "Connection timeout";
    } else {
      doc["status"] = "connecting";
    }
  } else {
    doc["status"] = "disconnected";
  }

  String response;
  serializeJson(doc, response);

  server.send(200, "application/json", response);
}

void handleReset() {
  preferences.begin("wifi-config", false);
  preferences.clear();
  preferences.end();

  WiFi.disconnect(true, true);

  connectionPending = false;
  credentialsSaved = false;

  server.send(200, "application/json",
              "{\"status\":\"credentials_erased\"}");

  Serial.println("Saved Wi-Fi credentials erased.");
}

// -------------------- Server setup --------------------

void setupServer() {
  server.on("/", HTTP_GET, handleRoot);

  server.on("/wifi/scan", HTTP_GET, handleScan);
  server.on("/wifi/connect", HTTP_POST, handleConnect);
  server.on("/wifi/status", HTTP_GET, handleStatus);
  server.on("/wifi/reset", HTTP_POST, handleReset);

  server.begin();

  Serial.println("HTTP server started.");
}