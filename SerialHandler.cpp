#include "SerialHandler.h"

namespace serial::handler {

constexpr uint8_t LOOP_DELAY = 5;
constexpr uint32_t STACK_DEPTH = 4000;

void taskHandler(void* pvParameter)
{
    auto pThis = static_cast<SerialHandler*>(pvParameter);

    while (!pThis->isNeedStop()) {
        pThis->handlerLoop();
        delay(LOOP_DELAY);
    }
}

SerialHandler::SerialHandler(component::wifi::logger::WiFiLogger::Ptr logger)
    : mLogger{logger}
{
}

void SerialHandler::initSerialHandler()
{
    xTaskCreate(&taskHandler, "serialCtrl", STACK_DEPTH, this, 5, &mSerialCtrlHandle);
}

void SerialHandler::handlerLoop()
{
    if (Serial.available()) {
        String inputString = "";
        char c = 0;
        while (Serial.available() && c != 13) {
            char c = (char)Serial.read();
            inputString += c;
        }

        mLogger->info("Receive : '%s'", inputString.c_str());

        DeserializationError err = deserializeJson(mJsonCmdReceive, inputString);
        if (err == DeserializationError::Ok) {
            handleCommand();
        }
        else {
            mLogger->warn("Failed to deserialize to json incoming data: '%s'", inputString.c_str());
            while (Serial.available() > 0) {
                Serial.read();
            }
        }
    }
}

void SerialHandler::addHandler(uint8_t cmdId, ISerialCommandHandler* handler)
{
    mLogger->info("Handle command: %u", cmdId);
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
