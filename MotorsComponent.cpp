#include "MotorsComponent.h"

#include "Command_ids.h"

namespace component::motors {

constexpr const char* JSON_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{"Left": %ld, "Right": %ld}}
)";

constexpr const char* JSON_STOP_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{}}
)";

constexpr const char* JSON_MOVE_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{}}
)";

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

const uint16_t RIGHT_ENC_INT_PIN = 16;
const uint16_t RIGHT_ENC_DIR_PIN = 27;

const uint16_t LEFT_ENC_INT_PIN = 35;
const uint16_t LEFT_ENC_DIR_PIN = 34;

volatile long MotorsControll::mLeftPulseCount;
volatile long MotorsControll::mRightPulseCount;

MotorsControll::MotorsControll(component::wifi::logger::WiFiLogger::Ptr logger)
    : mLogger{logger}
{
}

void MotorsControll::handleLeftPulse()
{
    auto direction = digitalRead(LEFT_ENC_DIR_PIN);
    if (direction) {
        mRightPulseCount++;
    }
    else {
        mRightPulseCount--;
    }
}

void MotorsControll::handleRightPulse()
{
    auto direction = digitalRead(RIGHT_ENC_DIR_PIN);
    if (direction) {
        mLeftPulseCount--;
    }
    else {
        mLeftPulseCount++;
    }
}

void MotorsControll::initMotors()
{
    mLeftPulseCount = 0;
    mRightPulseCount = 0;

    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(RIGHT_ENC_DIR_PIN, INPUT_PULLUP);
    pinMode(LEFT_ENC_DIR_PIN, INPUT_PULLUP);
    pinMode(LEFT_ENC_INT_PIN, INPUT);
    pinMode(RIGHT_ENC_INT_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(LEFT_ENC_INT_PIN), MotorsControll::handleLeftPulse,
                    RISING);
    attachInterrupt(digitalPinToInterrupt(RIGHT_ENC_INT_PIN), MotorsControll::handleRightPulse,
                    RISING);

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
    mLogger->info("Received command: %u", cmdId);

    switch (cmdId) {
    case CMD_MOVE_ROBOT:
        processMoveRobot(params);
        break;
    case CMD_STOP_ROBOT:
        mLeftPwm = mRightPwm = 0.0;
        // Serial.printf(JSON_STOP_RESPONSE_TEMPLATE, CMD_STOP_ROBOT);
        break;
    case CMD_GET_ENC_VALUES:
        mLogger->info("Start get encoders value");
        processGetEncodersValues();
        break;
    }
}

void MotorsControll::processMoveRobot(StaticJsonDocument<256> params)
{
    auto leftSpeed = params["L"].as<float>();
    auto rightSpeed = params["R"].as<float>();
    mLogger->info("Process move command: %.02f %.02f", leftSpeed, rightSpeed);

    if (abs(leftSpeed) > 255) {
        mLogger->info("Incorrect left speed: %.02f\n", leftSpeed);
        return;
    }

    if (abs(rightSpeed) > 255) {
        mLogger->info("Incorrect right speed speed: %.02f\n", rightSpeed);
        return;
    }

    mLogger->info("Command move robot. Left: %.02f Right: %.02f\n", leftSpeed, rightSpeed);
    mLeftPwm = leftSpeed;
    mRightPwm = rightSpeed;

    // Serial.printf(JSON_MOVE_RESPONSE_TEMPLATE, CMD_MOVE_ROBOT);
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

void MotorsControll::rightMotor(float pwm)
{
    int pwmIntA = round(pwm);
    if (pwmIntA > 0) {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        ledcWrite(CHANNEL_A, pwmIntA);
    }
    else {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        ledcWrite(CHANNEL_A, -pwmIntA);
    }
}

void MotorsControll::leftMotor(float pwm)
{
    int pwmIntB = round(pwm);
    if (pwmIntB > 0) {
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        ledcWrite(CHANNEL_B, pwmIntB);
    }
    else {
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
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

void MotorsControll::processGetEncodersValues()
{
    auto leftCount = mLeftPulseCount;
    auto rightCount = mRightPulseCount;

    mLeftPulseCount = 0;
    mRightPulseCount = 0;

    Serial.printf(JSON_RESPONSE_TEMPLATE, CMD_GET_ENC_VALUES, leftCount, rightCount);
}

}  // namespace component::motors
