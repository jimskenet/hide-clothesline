#include <NetworkSetup.h>

const char* AP_SSID = "Device_Setup_001";
const char* AP_PASSWORD = "EightLetters";

const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress AP_GATEWAY(192, 168, 4, 1);
const IPAddress AP_SUBNET(255, 255, 255, 0);

const unsigned long WIFI_TIMEOUT_MS = 20000;

WebServer server(80);
Preferences preferences;

void startSetupAP() {
    WiFi.mode(WIFI_AP_STA);

    WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);

    if (WiFi.softAP(AP_SSID, AP_PASSWORD)) {
        Serial.println("Setup AP started.");
        Serial.print("SSID: ");
        Serial.println(AP_SSID);
        Serial.print("AP IP: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed to start setup AP.");
    }
}

void beginWiFiConnection(const String& ssid, const String& password) {
    if (ssid.isEmpty()) {
        Serial.println("SSID is empty.");
        return;
    }

    // Keep AP active, disconnect from current router.
    WiFi.disconnect(false, false);

    WiFi.mode(WIFI_AP_STA);

    WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    Serial.println("Starting new Wi-Fi connection...");

    WiFi.begin(ssid.c_str(), password.c_str());
}

void startSavedWiFiConnection() {
    preferences.begin("wifi-config", true);

    String savedSSID = preferences.getString("ssid", "");
    String savedPassword = preferences.getString("password", "");

    preferences.end();

    // Always start AP so phone provisioning remains available.
    startSetupAP();

    if (savedSSID.isEmpty()) {
        Serial.println("No saved Wi-Fi credentials.");
        return;
    }

    beginWiFiConnection(savedSSID, savedPassword);
}