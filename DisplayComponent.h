#pragma once

#include <vector>

#include <Adafruit_SSD1306.h>

namespace component::display {

class DisplayControl {
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
};

}  // namespace component::display