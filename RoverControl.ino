#include <ArduinoJson.h>
#include <Wire.h>

// #include "BatteryInfoComponent.h"
#include "Command_ids.h"
// #include "DisplayComponent.h"
#include "I2CDevice.h"
#include "MotorsComponent.h"
#include "QMI8658.h"
#include "SerialHandler.h"
#include "WifiComponent.h"
#include "WifiLogger.h"

#define S_SCL 33
#define S_SDA 32

constexpr uint32_t UART_BAUD = 115200;
constexpr uint16_t LOOP_DELAY = 10;
constexpr long BATTERY_UPDATE_INTERVAL = 3000;

#define ENABLE_WIFI_LOGS

#ifdef ENABLE_WIFI_LOGS
component::wifi::logger::WiFiLogger::Ptr logger = new component::wifi::logger::WiFiLogger();
#else
component::wifi::logger::WiFiLogger::Ptr logger = new component::wifi::logger::WifiLoggerStub();
#endif

serial::handler::SerialHandler serialHandler{logger};
component::motors::MotorsControll motorsControl{logger};
// component::display::DisplayControl displayControl;
//  component::battery_info::BatteryInfoMonitor batteryInfoMonitor;
component::wifi::WifiControl wifiControl;
component::i2c::I2CDevice i2cDevice;
component::sensor::qmi8658::Qmi8658 qmi8659;

long lastTimeUpdate = 0;

void setup()
{
    Wire.begin(S_SDA, S_SCL);

    Serial.begin(UART_BAUD);
    while (!Serial) {
        delay(10);  // 10ms
    }

    qmi8659.setup();
    motorsControl.initMotors();
    // displayControl.initDisplay();
    // batteryInfoMonitor.setup();

    serialHandler.addHandler(CMD_MOVE_ROBOT, &motorsControl);
    serialHandler.addHandler(CMD_STOP_ROBOT, &motorsControl);
    serialHandler.addHandler(CMD_GET_ENC_VALUES, &motorsControl);

    serialHandler.addHandler(CMD_WIFI_SCAN, &wifiControl);
    serialHandler.addHandler(CMD_WIFI_HOTSPOT, &wifiControl);
    serialHandler.addHandler(CMD_WIFI_CONNECT, &wifiControl);
    serialHandler.addHandler(CMD_WIFI_STOP, &wifiControl);

    // serialHandler.addHandler(CMD_CLS_DISPLAY, &displayControl);
    // serialHandler.addHandler(CMD_SET_LINE, &displayControl);
    // serialHandler.addHandler(CMD_SET_LINES, &displayControl);

    serialHandler.addHandler(CMD_I2C_READ, &i2cDevice);
    serialHandler.addHandler(CMD_I2C_WRITE, &i2cDevice);
    serialHandler.addHandler(CMD_I2C_READ_2B, &i2cDevice);

    serialHandler.addHandler(CMD_IMU_DATA, &qmi8659);

    serialHandler.initSerialHandler();
    // wifiControl.connectToNetwork("cplusplus", "Anna198800");
    wifiControl.startHotspot();

    logger->startLogger();
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

    // displayControl.updateDisplay();

    delay(LOOP_DELAY);
}
