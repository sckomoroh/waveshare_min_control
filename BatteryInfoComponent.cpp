#include "BatteryInfoComponent.h"

namespace component::battery_info {

constexpr uint8_t INA219_ADDRESS = 0x42;

BatteryInfoMonitor::BatteryInfoMonitor()
    : mIna219{INA219_ADDRESS}
{
}

void BatteryInfoMonitor::setup()
{
    if (!mIna219.init()) {
        Serial.println("INA219 not connected!");
    }

    mIna219.setADCMode(BIT_MODE_9);
    mIna219.setPGain(PG_320);
    mIna219.setBusRange(BRNG_16);
    mIna219.setShuntSizeInOhms(0.01);  // used in INA219.
}

float BatteryInfoMonitor::getLoadVoltage()
{
    auto shuntVoltage_mV = mIna219.getShuntVoltage_mV();
    auto busVoltage_V = mIna219.getBusVoltage_V();

    return busVoltage_V + (shuntVoltage_mV / 1000);
}

float BatteryInfoMonitor::getCurrentMA() { return mIna219.getCurrent_mA(); }

}  // namespace component::battery_info
