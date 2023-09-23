#pragma once

#include <vector>

#include <Adafruit_SSD1306.h>

#include "SerialHandler.h"

namespace component::display {

class DisplayControl : public serial::handler::ISerialCommandHandler {
private:
    std::vector<String> mLines;
    Adafruit_SSD1306 mDisplay;

public:
    DisplayControl();

public:
    void initDisplay();

public:
    void updateDisplay();
    void setLine(uint8_t lineNumber, String text);

private:  // ISerialCommandHandler
    void onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params) override;

private:
    void processClearDisplay();
    void processSetLine(StaticJsonDocument<256> params);
    void processSetLines(StaticJsonDocument<256> params);
};

}  // namespace component::display