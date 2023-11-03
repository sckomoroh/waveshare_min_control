#include "WifiComponent.h"

#include <WiFi.h>

#include "Command_ids.h"

namespace component::wifi {

constexpr const char* JSON_SCAN_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{Networks:[]}}
)";

constexpr const char* JSON_CONNECT_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{}}
)";

constexpr const char* JSON_HOTSPOT_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{}}
)";

constexpr const char* JSON_STOP_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{}}
)";

constexpr const char* AP_SSID = "WAVE_ROVER";
constexpr const char* AP_PWD = "12345678";

WifiControl::~WifiControl() { WiFi.disconnect(); }

void WifiControl::onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params)
{
    switch (cmdId) {
    case CMD_WIFI_SCAN:
        scanWifiNetworks();
        break;
    case CMD_WIFI_CONNECT:
        processConnectToNetwork(params);
        break;
    case CMD_WIFI_HOTSPOT:
        startHotspot();
        break;
    case CMD_WIFI_STOP:
        WiFi.disconnect(true);
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
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int networkCount = WiFi.scanNetworks();
    if (networkCount < 1) {
        return;
    }

    for (auto i = 0; i < networkCount; i++) {
        // Serial.print(i + 1);
        // Serial.print(": ");
        // Serial.print(WiFi.SSID(i));
        // Serial.print(" (");
        // Serial.print(WiFi.RSSI(i));
        // Serial.print(")");
        // Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
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
    WiFi.disconnect();
    delay(100);

    WiFi.mode(WIFI_STA);
    auto res = WiFi.softAP(AP_SSID, AP_PWD);
    if (res) {
        auto ipAddress = WiFi.softAPIP();
    }
}

void WifiControl::connectToNetwork(String ssid, String password)
{
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
        auto ipAddress = WiFi.localIP();

        mSsid = ssid;
        mPassword = password;
    }
}

}  // namespace component::wifi
