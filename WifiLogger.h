#pragma once

#include <WiFiUdp.h>

#include <mutex>

namespace component::wifi::logger {

class WiFiLogger {
private:
    WiFiUDP mUdp;
    bool mIsInit = false;
    std::mutex mMutex;

public:
    using Ptr = WiFiLogger*;

public:
    virtual void startLogger();
    virtual void info(String log, ...);
    virtual void warn(String log, ...);
    virtual void error(String log, ...);
};

class WifiLoggerStub : public WiFiLogger {
public:
    void startLogger() override {}
    void info(String log, ...) override {}
    void warn(String log, ...) override {}
    void error(String log, ...) override {}
};

}  // namespace component::wifi::logger