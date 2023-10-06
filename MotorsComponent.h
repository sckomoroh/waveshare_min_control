#pragma once

#include <atomic>

#include "SerialHandler.h"

namespace component::motors {

class MotorsControll : public serial::handler::ISerialCommandHandler {
private:
    static volatile std::atomic<long> mLeftPulseCount, mRightPulseCount;

private:
    float mLeftPwm = 0.0, mRightPwm = 0.0;

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

    static void IRAM_ATTR handleLeftPulse();
    static void IRAM_ATTR handleRightPulse();

    void processGetEncodersValues();
};

}  // namespace component::motors