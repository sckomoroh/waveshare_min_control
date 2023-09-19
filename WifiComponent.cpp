#include "WifiComponent.h"

#include <WiFi.h>

#include "Command_ids.h"

namespace component::wifi {

constexpr const char* AP_SSID = "WAVE_ROVER";
constexpr const char* AP_PWD = "12345678";

WifiControl::~WifiControl() { WiFi.disconnect(); }

void WifiControl::onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params)
{
    switch (cmdId) {
    case WIFI_SCAN:
        scanWifiNetworks();
        break;
    case WIFI_CONNECT:
        processConnectToNetwork(params);
        break;
    case WIFI_HOTSPOT:
        startHotspot();
        break;
    }
}

void WifiControl::processConnectToNetwork(StaticJsonDocument<256> params)
{
    String ssid = params["SSID"].as<String>();
    String password = params["PWD"].as<String>();

    connectToNetwork(ssid, password);
}

void WifiControl::scanWifiNetworks()
{
    Serial.println("Scanning WiFi...");
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int networkCount = WiFi.scanNetworks();
    if (networkCount < 1) {
        Serial.println("Networks not found");
        return;
    }

    for (auto i = 0; i < networkCount; i++) {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        delay(10);
    }

    if (mSsid.isEmpty()) {
        startHotspot();
    }
    else {
        connectToNetwork(mSsid, mPassword);
    }
}

void WifiControl::startHotspot()
{
    Serial.println("Start hotspot");

    WiFi.disconnect();
    delay(100);

    WiFi.mode(WIFI_STA);
    auto res = WiFi.softAP(AP_SSID, AP_PWD);
    if (res) {
        Serial.printf("Start hotspot '%s'\n", AP_SSID);
        auto ipAddress = WiFi.softAPIP();
        Serial.printf("STA IP address: '%s'\n", ipAddress.toString().c_str());
    }
    else {
        Serial.printf("Failed to Start hotspot: '%s'. Error: %d\n", AP_SSID, res);
    }
}

void WifiControl::connectToNetwork(String ssid, String password)
{
    Serial.printf("Connect to network: '%s'\n", ssid.c_str());
    WiFi.disconnect();
    delay(100);

    mSsid.clear();
    mPassword.clear();

    WiFi.mode(WIFI_STA);
    auto res = WiFi.begin(ssid, password);

    int retryCount = 10;
    while ((res = WiFi.status()) != WL_CONNECTED && retryCount-- > 0) {
        delay(1000);
    }

    if (res == WL_CONNECTED) {
        Serial.printf("Connected to '%s'\n", ssid.c_str());
        auto ipAddress = WiFi.localIP();
        Serial.printf("STA IP address: '%s'\n", ipAddress.toString().c_str());

        mSsid = ssid;
        mPassword = password;
    }
    else {
        Serial.printf("Failed to connect to network: '%s'. Error: %d\n", ssid.c_str(), res);
    }
}

}  // namespace component::wifi
