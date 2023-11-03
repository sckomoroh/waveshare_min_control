#pragma once

#include <atomic>

#include "SerialHandler.h"
#include "WifiLogger.h"

namespace component::motors {

class MotorsControll : public serial::handler::ISerialCommandHandler {
private:
    static volatile long mLeftPulseCount;
    static volatile long mRightPulseCount;

private:
    float mLeftPwm = 0.0, mRightPwm = 0.0;
    component::wifi::logger::WiFiLogger::Ptr mLogger;

public:
    MotorsControll(component::wifi::logger::WiFiLogger::Ptr logger);

public:
    void initMotors();
    void processMotors();

    inline float getLeftPwm() const { return mLeftPwm; }
    inline float getRightPwm() const { return mRightPwm; }

private:  // ISerialCommandHandler
    void onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params) override;

private:
    void processMoveRobot(StaticJsonDocument<256> params);
    void moveMotor(float left, float right);
    void stopMotor();
    void leftMotor(float pwm);
    void rightMotor(float pwm);

    void processGetEncodersValues();

    static void IRAM_ATTR handleLeftPulse();
    static void IRAM_ATTR handleRightPulse();
};

}  // namespace component::motors