#pragma once

#include "MqttHelpers.hpp"
#include "RadioSX1276.hpp"
#include "IoHomeControl.hpp"

#include <map>
#include <mutex>

namespace IoRts
{
    class IoRtsManager
    {
    public:
        std::mutex mIoDevicesMutex;                         // Mutex to protect IoDevices list
        std::map<std::string, iohome::IoDevice> mIoDevices; // Map of currently managed IoDevices, protected by mIoDevicesMutex, as this list can change in other threads !

        RadioLinks::RadioSX1276 *mSX1276Radio; // Pointer to radio object used in IoHomeControl object
        iohome::IoHomeControl *mIoHome;        // Pointer to IoHomeControl object used to manage Io-HomeControl protocol

        /// @brief Constructor for IoRtsManager
        IoRtsManager();

        /// @brief Ask to reboot ESP32
        void Reboot();

        /// @brief Retrieve current configuration about passive / active mode
        /// @return true if currently in passive mode
        bool isIoPassive() { return mIoPassive; }

    private:
        bool mIoPassive = false; // current configuration, initialized at boot

        /// @brief Initialize Io objects members (mSX1276Radio, mIoHome)
        void InitializeIo();

        /// @brief Initialize MQTT objects members (mMqttHelper)
        void InitializeMqtt();
    };

}