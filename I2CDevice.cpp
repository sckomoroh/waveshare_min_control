#include "I2CDevice.h"

#include <Wire.h>

#include "Command_ids.h"

namespace component::i2c {

constexpr uint8_t QMI8658_SLAVE_ADDR_L = 0x6B;

constexpr const char* JSON_RESPONSE_TEMPLATE = "{'Data': %u}\n";

void I2CDevice::onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params)
{
    Serial.printf("[DEBUG] Handle command: %u\n", cmdId);

    switch (cmdId) {
    case CMD_I2C_READ:
        processReadDevice(params);
        break;
    case CMD_I2C_WRITE:
        processWriteDevice(params);
        break;
    }
}

void I2CDevice::processReadDevice(StaticJsonDocument<256> params)
{
    auto reg = params["Reg"].as<uint8_t>();

    Serial.printf("[DEBUG] Read registry: %u\n", reg);
    Wire.beginTransmission(QMI8658_SLAVE_ADDR_L);
    Serial.printf("[DEBUG] Write result: %u\n", Wire.write(reg));
    Wire.endTransmission();

    Serial.printf("[DEBUG] Request result: %u\n", Wire.requestFrom(QMI8658_SLAVE_ADDR_L, 1));
    uint8_t value = Wire.read();
    Serial.printf("[DEBUG] Registry value: %u\n", value);
    Serial.printf(JSON_RESPONSE_TEMPLATE, value);
}

void I2CDevice::processWriteDevice(StaticJsonDocument<256> params)
{
    auto reg = params["Reg"].as<uint8_t>();
    auto value = params["Value"].as<uint8_t>();

    Serial.printf("[DEBUG] Write to registry: %u value: %u\n", reg, value);

    Wire.beginTransmission(QMI8658_SLAVE_ADDR_L);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

}  // namespace component::i2c
