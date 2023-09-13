#include <ArduinoJson.h>
#include <Wire.h>

#include "Command_ids.h"
#include "DisplayComponent.h"
#include "MotorsComponent.h"
#include "SerialHandler.h"

#define S_SCL 33
#define S_SDA 32

constexpr uint32_t UART_BAUD = 115200;
constexpr uint16_t LOOP_DELAY = 100;

serial::handler::SerialHandler serialHandler;
component::motors::MotorsControll motorsControl;
component::display::DisplayControl displayControl;

void setup()
{
    Wire.begin(S_SDA, S_SCL);

    Serial.begin(UART_BAUD);
    while (!Serial) {
        delay(10);  // 10ms
    }

    motorsControl.initMotors();
    displayControl.initDisplay();

    serialHandler.addHandler(MOVE_ROBOT, &motorsControl);
    serialHandler.addHandler(STOP_ROBOT, &motorsControl);
    serialHandler.initSerialHandler();
}

void loop()
{
    motorsControl.processMotors();
    char leftBuffer[256], rightBuffer[256];
    sprintf(leftBuffer, "Left: %.02f", motorsControl.getLeftPwm());
    sprintf(rightBuffer, "Right: %.02f", motorsControl.getRightPwm());

    displayControl.setLine(0, leftBuffer);
    displayControl.setLine(1, rightBuffer);

    displayControl.updateDisplay();

    delay(LOOP_DELAY);
}
