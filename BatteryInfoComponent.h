#pragma once

#include <INA219_WE.h>

namespace component::battery_info {

class BatteryInfoMonitor {
private:
    INA219_WE mIna219;

public:
    BatteryInfoMonitor();

public:
    void setup();

    float getLoadVoltage();
    float getCurrentMA();
};

}  // namespace component::battery_info
