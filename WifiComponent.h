#pragma once

#include "SerialHandler.h"
#include "WifiLogger.h"

namespace component::wifi {

class WifiControl : public serial::handler::ISerialCommandHandler {
private:
    String mSsid;
    String mPassword;

public:
    ~WifiControl();

public:
    void scanWifiNetworks();
    void startHotspot();
    void connectToNetwork(String ssid, String password);

private:  // ISerialCommandHandler
    void onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params) override;

private:
    void processConnectToNetwork(StaticJsonDocument<256> params);
};

}  // namespace component::wifi