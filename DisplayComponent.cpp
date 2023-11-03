#include "DisplayComponent.h"

#include "Command_ids.h"

namespace component::display {

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 32;
constexpr uint8_t OLED_RESET = -1;
constexpr uint8_t SCREEN_ADDRESS = 0x3C;

constexpr uint8_t MAX_LINES = 4;

DisplayControl::DisplayControl()
    : mLines(4)
    , mDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

void DisplayControl::initDisplay()
{
    if (!mDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        //Serial.println(F("SSD1306 allocation failed"));
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
    if (lineNumber > MAX_LINES - 1) {
        //Serial.printf("[ERROR] Line number shall be in range 0-3. You passed: %u\n", lineNumber);
    }

    mLines[lineNumber] = text;
}

void DisplayControl::onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params)
{
    switch (cmdId) {
    case CMD_CLS_DISPLAY:
        processClearDisplay();
        break;
    case CMD_SET_LINE:
        processSetLine(params);
        break;
    case CMD_SET_LINES:
        processSetLines(params);
        break;
    }
}

void DisplayControl::processClearDisplay()
{
    mLines[0] = "";
    mLines[1] = "";
    mLines[2] = "";
    mLines[3] = "";
}

void DisplayControl::processSetLine(StaticJsonDocument<256> params)
{
    auto lineNumber = params["Line"].as<uint8_t>();
    if (lineNumber > MAX_LINES - 1) {
        //Serial.printf("[WARN ] Max line number is 3 but got %d", lineNumber);
        return;
    }

    mLines[lineNumber] = params["Str"].as<String>();
}

void DisplayControl::processSetLines(StaticJsonDocument<256> params)
{
    processClearDisplay();
    auto lines = params["Lines"].as<JsonArray>();
    auto count = lines.size() > MAX_LINES ? MAX_LINES : lines.size();
    for (int i = 0; i < count; i++) {
        mLines[i] = lines[i].as<String>();
    }
}

}  // namespace component::display
