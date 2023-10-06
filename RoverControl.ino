#include <ArduinoJson.h>
#include <Wire.h>

#include "BatteryInfoComponent.h"
#include "Command_ids.h"
#include "DisplayComponent.h"
#include "I2CDevice.h"
#include "MotorsComponent.h"
#include "SerialHandler.h"
#include "WifiComponent.h"

#define S_SCL 33
#define S_SDA 32

constexpr uint32_t UART_BAUD = 115200;
constexpr uint16_t LOOP_DELAY = 100;
constexpr long BATTERY_UPDATE_INTERVAL = 3000;

serial::handler::SerialHandler serialHandler;
component::motors::MotorsControll motorsControl;
component::display::DisplayControl displayControl;
component::battery_info::BatteryInfoMonitor batteryInfoMonitor;
component::wifi::WifiControl wifiControl;
component::i2c::I2CDevice i2cDevice;

long lastTimeUpdate = 0;

void setup()
{
    Wire.begin(S_SDA, S_SCL);

    Serial.begin(UART_BAUD);
    while (!Serial) {
        delay(10);  // 10ms
    }

    motorsControl.initMotors();
    displayControl.initDisplay();
    batteryInfoMonitor.setup();

    serialHandler.addHandler(CMD_MOVE_ROBOT, &motorsControl);
    serialHandler.addHandler(CMD_STOP_ROBOT, &motorsControl);
    serialHandler.addHandler(CMD_GET_ENC_VALUES, &motorsControl);

    serialHandler.addHandler(CMD_WIFI_SCAN, &wifiControl);
    serialHandler.addHandler(CMD_WIFI_HOTSPOT, &wifiControl);
    serialHandler.addHandler(CMD_WIFI_CONNECT, &wifiControl);
    serialHandler.addHandler(CMD_WIFI_STOP, &wifiControl);

    serialHandler.addHandler(CMD_CLS_DISPLAY, &displayControl);
    serialHandler.addHandler(CMD_SET_LINE, &displayControl);
    serialHandler.addHandler(CMD_SET_LINES, &displayControl);

    serialHandler.addHandler(CMD_I2C_READ, &i2cDevice);
    serialHandler.addHandler(CMD_I2C_WRITE, &i2cDevice);

    serialHandler.initSerialHandler();
    // wifiControl.startHotspot();
}

void loop()
{
    motorsControl.processMotors();
    auto currentMillis = millis();
    if (currentMillis - lastTimeUpdate > BATTERY_UPDATE_INTERVAL) {
        // lastTimeUpdate = currentMillis;
        // char currentMABuffer[32] = {0};
        // char loadVoltageBuffer[32] = {0};
        // sprintf(currentMABuffer, "Current: %.02f mA", batteryInfoMonitor.getCurrentMA());
        // sprintf(loadVoltageBuffer, "Battery: %.02f V", batteryInfoMonitor.getLoadVoltage());
        // displayControl.setLine(2, currentMABuffer);
        // displayControl.setLine(3, loadVoltageBuffer);
    }

    // char leftBuffer[32] = {0};
    // char rightBuffer[32] = {0};

    // sprintf(leftBuffer, "Left: %.02f", motorsControl.getLeftPwm());
    // sprintf(rightBuffer, "Right: %.02f", motorsControl.getRightPwm());

    // displayControl.setLine(0, leftBuffer);
    // displayControl.setLine(1, rightBuffer);

    displayControl.updateDisplay();

    delay(LOOP_DELAY);
}
