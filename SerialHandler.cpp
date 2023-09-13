#include "SerialHandler.h"

namespace serial::handler {

constexpr uint8_t LOOP_DELAY = 5;

void taskHandler(void* pvParameter)
{
    auto pThis = static_cast<SerialHandler*>(pvParameter);

    while (!pThis->isNeedStop()) {
        pThis->handlerLoop();
        delay(LOOP_DELAY);
    }
}

void SerialHandler::initSerialHandler()
{
    // TODO: remove magic numbers
    xTaskCreate(&taskHandler, "serialCtrl", 4000, this, 5, &mSerialCtrlHandle);
}

void SerialHandler::handlerLoop()
{
    if (Serial.available()) {
        DeserializationError err = deserializeJson(mJsonCmdReceive, Serial);
        if (err == DeserializationError::Ok) {
            handleCommand();
        }
        else {
            // Read noise data
            while (Serial.available() > 0) {
                Serial.read();
            }
        }
    }
}

void SerialHandler::addHandler(uint8_t cmdId, ISerialCommandHandler* handler)
{
    mHandlers[cmdId].push_back(handler);
}

void SerialHandler::handleCommand()
{
    std::unique_lock<std::mutex> lock{mCmdMutex};

    auto commandId = mJsonCmdReceive["CmdID"].as<uint8_t>();
    auto handlers = mHandlers[commandId];
    for (auto handler : handlers) {
        handler->onCommandReceived(commandId, mJsonCmdReceive["Params"]);
    }
}

}  // namespace serial::handler
