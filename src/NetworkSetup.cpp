#include <NetworkSetup.h>

const char* AP_SSID = "Device_Setup_001";
const char* AP_PASSWORD = "EightLetters";

const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress AP_GATEWAY(192, 168, 4, 1);
const IPAddress AP_SUBNET(255, 255, 255, 0);

const unsigned long WIFI_TIMEOUT_MS = 20000;

// Globals

WebServer server(80);
Preferences preferences;

String pendingSSID;
String pendingPassword;

bool connectionPending = false;
bool credentialsSaved = false;

unsigned long connectionStartTime = 0;

//Function definitions

void startSetupAP() {
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);

  bool success = WiFi.softAP(AP_SSID, AP_PASSWORD);

  if (success) {
    Serial.println("Setup AP started.");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Failed to start setup AP.");
  }
}

void startSavedWiFiConnection() {
  preferences.begin("wifi-config", true);

  String savedSSID = preferences.getString("ssid", "");
  String savedPassword = preferences.getString("password", "");

  preferences.end();

  if (savedSSID.length() == 0) {
    Serial.println("No saved Wi-Fi credentials.");
    startSetupAP();
    return;
  }

  Serial.print("Connecting to saved network: ");
  Serial.println(savedSSID);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  WiFi.begin(savedSSID.c_str(), savedPassword.c_str());

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to saved Wi-Fi.");
    Serial.print("Router IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nSaved Wi-Fi connection failed.");
    startSetupAP();
  }
}

