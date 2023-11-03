#pragma once

#include "SerialHandler.h"

namespace component::i2c {

class I2CDevice : public serial::handler::ISerialCommandHandler {
private:  // ISerialCommandHandler
    void onCommandReceived(uint8_t cmdId, StaticJsonDocument<256> params) override;

private:
    void processReadDevice(StaticJsonDocument<256> params);
    void processReadWordDevice(StaticJsonDocument<256> params);
    void processWriteDevice(StaticJsonDocument<256> params);
};

}  // namespace component::i2c