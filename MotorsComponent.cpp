#include "MotorsComponent.h"

#include "Command_ids.h"

namespace component::motors {

constexpr uint16_t PWMA = 25;  // Motor A PWM control  Orange
constexpr uint16_t AIN2 = 17;  // Motor A input 2      Brown
constexpr uint16_t AIN1 = 21;  // Motor A input 1      Green
constexpr uint16_t BIN1 = 22;  // Motor B input 1       Yellow
constexpr uint16_t BIN2 = 23;  // Motor B input 2       Purple
constexpr uint16_t PWMB = 26;  // Motor B PWM control   White

constexpr int FREQ = 100000;
constexpr int CHANNEL_A = 5;
constexpr int CHANNEL_B = 6;
constexpr uint16_t ANALOG_WRITE_BITS = 8;

const uint16_t BENCB = 16;  // Encoder B input      Green
const uint16_t BENCA = 27;

const uint16_t AENCA = 35;  // Encoder A input      Yellow
const uint16_t AENCB = 34;

void IRAM_ATTR right_wheel_pulse() {}

void IRAM_ATTR left_wheel_pulse() {}

void MotorsControll::initMotors()
{
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(BENCB), right_wheel_pulse, RISING);
    attachInterrupt(digitalPinToInterrupt(AENCB), left_wheel_pulse, RISING);

    ledcSetup(CHANNEL_A, FREQ, ANALOG_WRITE_BITS);
    ledcAttachPin(PWMA, CHANNEL_A);

    ledcSetup(CHANNEL_B, FREQ, ANALOG_WRITE_BITS);
    ledcAttachPin(PWMB, CHANNEL_B);

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
}

void MotorsControll::onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params)
{
    switch (cmdId) {
    case CMD_MOVE_ROBOT:
        processMoveRobot(params);
        break;
    case CMD_STOP_ROBOT:
        mLeftPwm = mRightPwm = 0.0;
        break;
    }
}

void MotorsControll::processMoveRobot(StaticJsonDocument<256> params)
{
    auto leftSpeed = params["L"].as<float>();
    auto rightSpeed = params["R"].as<float>();
    if (abs(leftSpeed) > 255) {
        Serial.printf("[DEBUG] Incorrect left speed: %.02f\n", leftSpeed);
        return;
    }

    if (abs(rightSpeed) > 255) {
        Serial.printf("[DEBUG] Incorrect right speed speed: %.02f\n", rightSpeed);
        return;
    }

    Serial.printf("[DEBUG] Command move robot. Left: %.02f Right: %.02f\n", leftSpeed, rightSpeed);
    mLeftPwm = leftSpeed;
    mRightPwm = rightSpeed;
}

void MotorsControll::processMotors()
{
    if (round(mLeftPwm) == 0 && round(mRightPwm) == 0) {
        stopMotor();
    }
    else {
        moveMotor(mLeftPwm, mRightPwm);
    }
}

void MotorsControll::moveMotor(float left, float right)
{
    leftMotor(left);
    rightMotor(right);
}

void MotorsControll::leftMotor(float pwm)
{
    int pwmIntA = -round(pwm);
    if (pwmIntA > 0) {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        ledcWrite(CHANNEL_A, pwmIntA);
    }
    else {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        ledcWrite(CHANNEL_A, -pwmIntA);
    }
}

void MotorsControll::rightMotor(float pwm)
{
    int pwmIntB = -round(pwm);
    if (pwmIntB > 0) {
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        ledcWrite(CHANNEL_B, pwmIntB);
    }
    else {
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        ledcWrite(CHANNEL_B, -pwmIntB);
    }
}

void MotorsControll::stopMotor()
{
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
}

}  // namespace component::motors
