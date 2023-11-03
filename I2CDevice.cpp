#include "I2CDevice.h"

#include <Wire.h>

#include "Command_ids.h"

namespace component::i2c {

constexpr uint8_t QMI8658_SLAVE_ADDR_L = 0x6B;

constexpr const char* JSON_WRITE_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{}}
)";

constexpr const char* JSON_RESPONSE_TEMPLATE =
    R"({"CmdId": %u, "Data":{"Reg": %u, "Data": %u}}
)";

constexpr const char* JSON_RESPONSE_2B_TEMPLATE =
    R"({"CmdId": %u, "Data":{"Reg": %u, "Data": [%u, %u]}}
)";

void I2CDevice::onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params)
{
    switch (cmdId) {
    case CMD_I2C_READ:
        processReadDevice(params);
        break;
    case CMD_I2C_WRITE:
        processWriteDevice(params);
        break;
    case CMD_I2C_READ_2B:
        processReadWordDevice(params);
        break;
    }
}

void I2CDevice::processReadDevice(StaticJsonDocument<256> params)
{
    auto reg = params["Reg"].as<uint8_t>();
    Wire.beginTransmission(QMI8658_SLAVE_ADDR_L);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(QMI8658_SLAVE_ADDR_L, 1);
    uint8_t value = Wire.read();
    Serial.printf(JSON_RESPONSE_TEMPLATE, CMD_I2C_READ, reg, value);
}

void I2CDevice::processReadWordDevice(StaticJsonDocument<256> params)
{
    auto reg = params["Reg"].as<uint8_t>();
    Wire.beginTransmission(QMI8658_SLAVE_ADDR_L);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(QMI8658_SLAVE_ADDR_L, 2);
    uint8_t value_h = Wire.read();
    uint8_t value_l = Wire.read();
    Serial.printf(JSON_RESPONSE_2B_TEMPLATE, CMD_I2C_READ_2B, reg, value_h, value_l);
}

void I2CDevice::processWriteDevice(StaticJsonDocument<256> params)
{
    auto reg = params["Reg"].as<uint8_t>();
    auto value = params["Value"].as<uint8_t>();

    Wire.beginTransmission(QMI8658_SLAVE_ADDR_L);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
    //Serial.printf(JSON_WRITE_RESPONSE_TEMPLATE, CMD_I2C_WRITE);
}

}  // namespace component::i2c
