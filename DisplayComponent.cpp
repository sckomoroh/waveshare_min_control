#include "DisplayComponent.h"

namespace component::display {

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 32;
constexpr uint8_t OLED_RESET = -1;
constexpr uint8_t SCREEN_ADDRESS = 0x3C;

DisplayControl::DisplayControl()
    : mLines(4)
    , mDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

void DisplayControl::initDisplay()
{
    if (!mDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
    }
    mDisplay.clearDisplay();
    mDisplay.display();
}

void DisplayControl::updateDisplay()
{
    mDisplay.clearDisplay();
    mDisplay.setTextSize(1);
    mDisplay.setTextColor(SSD1306_WHITE);
    mDisplay.setCursor(0, 0);

    mDisplay.println(mLines[0]);
    mDisplay.println(mLines[1]);
    mDisplay.println(mLines[2]);
    mDisplay.println(mLines[3]);

    mDisplay.display();
}

void DisplayControl::setLine(uint8_t lineNumber, String text)
{
    if (lineNumber > 3) {
        Serial.printf("[ERROR] Line number shall be in range 0-3. You passed: %u\n", lineNumber);
    }

    mLines[lineNumber] = text;
}

}  // namespace component::display
