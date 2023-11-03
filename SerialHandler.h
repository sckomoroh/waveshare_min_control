#pragma once

#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <list>
#include <map>
#include <mutex>

#include "WifiLogger.h"

namespace serial::handler {

class ISerialCommandHandler {
public:
    virtual ~ISerialCommandHandler() = default;

public:
    virtual void onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params) = 0;
};

class SerialHandler {
private:
    bool mIsNeedStop = false;
    TaskHandle_t mSerialCtrlHandle;
    StaticJsonDocument<256> mJsonCmdReceive;
    std::mutex mCmdMutex;
    std::map<uint8_t, std::list<ISerialCommandHandler*>> mHandlers;
    component::wifi::logger::WiFiLogger::Ptr mLogger;
    
public:
    SerialHandler(component::wifi::logger::WiFiLogger::Ptr logger);

public:
    void initSerialHandler();
    void handlerLoop();

    inline bool isNeedStop() const { return mIsNeedStop; }
    inline void setNeedStopbool(bool value) { mIsNeedStop = value; }

    void addHandler(uint8_t cmdId, ISerialCommandHandler* handler);

private:
    void handleCommand();
};

}  // namespace serial::handler