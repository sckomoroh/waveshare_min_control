#include "WifiLogger.h"

namespace component::wifi::logger {

constexpr const char* MSG_TEMPLATE = "[%s] %s\n";
constexpr uint16_t BUFFER_SIZE = 1024;
constexpr uint16_t LOCAL_PORT = 2001;
constexpr uint16_t REMOTE_PORT = 2002;
const IPAddress BROADCAST_IP = IPAddress{255, 255, 255, 255};

void WiFiLogger::startLogger()
{
    mUdp.begin(LOCAL_PORT);
    mIsInit = true;
}

void WiFiLogger::info(String log, ...)
{
    if (mIsInit) {
        std::unique_lock<std::mutex> lock{mMutex};

        va_list args;
        va_start(args, log);

        char buf[1024] = {0};
        vsnprintf(buf, sizeof(buf), log.c_str(), args);

        va_end(args);

        String toSend = "[INFO ] " + String(buf) + "\n";
        mUdp.beginPacket(BROADCAST_IP, REMOTE_PORT);
        mUdp.write((uint8_t*)toSend.c_str(), toSend.length());
        mUdp.endPacket();
    }
}

void WiFiLogger::warn(String log, ...)
{
    if (mIsInit) {
        std::unique_lock<std::mutex> lock{mMutex};

        va_list args;
        va_start(args, log);

        char buf[1024] = {0};
        vsnprintf(buf, sizeof(buf), log.c_str(), args);

        va_end(args);

        String toSend = "[WARN ] " + String(buf) + "\n";
        mUdp.beginPacket(BROADCAST_IP, REMOTE_PORT);
        mUdp.write((uint8_t*)toSend.c_str(), toSend.length());
        mUdp.endPacket();
    }
}

void WiFiLogger::error(String log, ...)
{
    if (mIsInit) {
        std::unique_lock<std::mutex> lock{mMutex};

        va_list args;
        va_start(args, log);

        char buf[1024] = {0};
        vsnprintf(buf, sizeof(buf), log.c_str(), args);

        va_end(args);

        String toSend = "[ERROR] " + String(buf) + "\n";
        mUdp.beginPacket(BROADCAST_IP, REMOTE_PORT);
        mUdp.write((uint8_t*)toSend.c_str(), toSend.length());
        mUdp.endPacket();
    }
}

}  // namespace component::wifi::logger
