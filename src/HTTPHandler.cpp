#include <HTTPHandler.h>
#include <NetworkSetup.h>
#include <ControlTasks.h>
#include <ArduinoJson.h>

void setupServer() {

    // Root page
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/plain",
                    "ESP32 Clothesline Controller");
    });

    
    // Rain sensor status
    server.on("/rain/status", HTTP_GET, []() {

        bool rainDetected;

        // Safely read the shared rain state.
        if (xSemaphoreTake(rainMutex, pdMS_TO_TICKS(10)) != pdTRUE) {
            server.send(503, "application/json",
                        "{\"error\":\"Rain sensor status temporarily unavailable\"}");
            return;
        }

        rainDetected = currentRainState;

        xSemaphoreGive(rainMutex);

        StaticJsonDocument<128> doc;

        doc["rainDetected"] = rainDetected;

        String response;
        serializeJson(doc, response);

        server.send(200, "application/json", response);
    });

    // Wi-Fi status
    server.on("/wifi/status", HTTP_GET, []() {
        StaticJsonDocument<256> doc;

        doc["connected"] = WiFi.status() == WL_CONNECTED;
        doc["ssid"] = WiFi.SSID();
        doc["routerIP"] = WiFi.localIP().toString();
        doc["setupAP"] = WiFi.softAPIP().toString();

        String response;
        serializeJson(doc, response);

        server.send(200, "application/json", response);
    });

    // Save and connect to new Wi-Fi credentials
    server.on("/wifi/connect", HTTP_POST, []() {
        StaticJsonDocument<256> doc;

        DeserializationError error =
            deserializeJson(doc, server.arg("plain"));

        if (error) {
            server.send(400, "application/json",
                        "{\"error\":\"Invalid JSON\"}");
            return;
        }

        String ssid = doc["ssid"] | "";
        String password = doc["password"] | "";

        if (ssid.isEmpty()) {
            server.send(400, "application/json",
                        "{\"error\":\"SSID is required\"}");
            return;
        }

        preferences.begin("wifi-config", false);
        preferences.putString("ssid", ssid);
        preferences.putString("password", password);
        preferences.end();

        beginWiFiConnection(ssid, password);

        server.send(200, "application/json",
                    "{\"message\":\"Wi-Fi connection started\"}");
    });

    //Scan Networks
    server.on("/wifi/scan", HTTP_GET, []() {
        int n = WiFi.scanNetworks();
        StaticJsonDocument<512> doc;

        JsonArray networks = doc.createNestedArray("networks");

        for (int i = 0; i < n; ++i) {
            JsonObject network = networks.createNestedObject();
            network["ssid"] = WiFi.SSID(i);
            network["rssi"] = WiFi.RSSI(i);
            network["channel"] = WiFi.channel(i);
            network["secure"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
            network["bssid"] = WiFi.BSSIDstr(i);
        }

        String response;
        serializeJson(doc, response);

        server.send(200, "application/json", response);
    });

    // Manual servo deploy
    server.on("/servo/deploy", HTTP_POST, []() {
        ManualCommand command = CMD_DEPLOY;

        if (xQueueSend(manualQueue, &command, 0) != pdTRUE) {
            server.send(503, "application/json",
                        "{\"error\":\"Command queue full\"}");
            return;
        }

        server.send(200, "application/json",
                    "{\"message\":\"Deploy command accepted\"}");
    });

    // Manual servo retract
    server.on("/servo/retract", HTTP_POST, []() {
        ManualCommand command = CMD_RETRACT;

        if (xQueueSend(manualQueue, &command, 0) != pdTRUE) {
            server.send(503, "application/json",
                        "{\"error\":\"Command queue full\"}");
            return;
        }

        server.send(200, "application/json",
                    "{\"message\":\"Retract command accepted\"}");
    });

    server.onNotFound([]() {
        server.send(404, "text/plain", "Not found");
    });

    server.begin();

    Serial.println("HTTP server started.");
}