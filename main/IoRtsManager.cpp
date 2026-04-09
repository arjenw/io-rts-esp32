#include "IoRtsManager.hpp"
#include "HardwareConfig.hpp"
#include "MqttConfig.hpp"

#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "ioRtsMan";

using namespace Helpers;
using namespace Config;

namespace IoRts
{
    static IoRtsManager *sIoRtsManager; // Pointer to IoRtsManager isntance
    static MqttHelpers *sMqttHelper;    // Pointer to MQTT instance to manage MQTT communication layer

#ifdef CONFIG_ENABLE_IOHOMECONTROL
    static void loggerCallback(esp_log_level_t log_level, const char *tag, std::string log)
    {
        switch (log_level)
        {
        case ESP_LOG_ERROR:
            ESP_LOGE(tag, "%s", log.c_str());
            break;
        case ESP_LOG_INFO:
            ESP_LOGI(tag, "%s", log.c_str());
            break;
        default:
            break;
        }
    }

    static void deviceStatusCallback(const std::string deviceID, const iohome::IoDevice &device)
    {
        ESP_LOGI(TAG, "Callback received device status for %s: %s (0x%02X/0x%02X) / Position %.1f / Target %0.1f / Moving: %s",
                 deviceID.c_str(), device.info.name, device.info.device_type, device.info.device_subtype,
                 device.position, device.target,
                 device.is_stopped ? "No" : "Yes");
        if (sIoRtsManager == nullptr)
            return;
        if (!sIoRtsManager->isIoPassive()                                                                  // not passive mode
            && (device.info.device_type != iohome::DeviceType::UNKNOWN) && (strlen(device.info.name) > 0)) // we have at least device name and type
        {
            // So we can process device status
            sIoRtsManager->mIoDevicesMutex.lock(); // Take mutex
            if (!sIoRtsManager->mIoDevices.contains(deviceID))
            {
                // We have a new device detected!
                // add device to mIoDevices
                sIoRtsManager->mIoDevices.insert({deviceID, device});
                // TODO: add device to flash storage
                sIoRtsManager->mIoDevicesMutex.unlock(); // release mutex as discovery needs it!
                // send MQTT discovery message
                if (sMqttHelper != nullptr)
                    sMqttHelper->SendDiscovery();
            }
            else
            {
                // update mIoDevices!
                auto it = sIoRtsManager->mIoDevices.find(deviceID);                       // we have something as we found it just before
                memcpy(it->second.info.name, device.info.name, sizeof(device.info.name)); // could be update by "SetName"
                it->second.is_stopped = device.is_stopped;
                it->second.last_status_timestamp = device.last_status_timestamp;
                it->second.next_status_update_timestamp = device.next_status_update_timestamp;
                it->second.position = device.position;
                it->second.target = device.target;
                sIoRtsManager->mIoDevicesMutex.unlock(); // release mutex!
            }
            // send MQTT device status
            if (sMqttHelper != nullptr)
                sMqttHelper->SendIoDeviceStatus(deviceID);
        }
    }
#endif // ENABLE_IOHOMECONTROL

    IoRtsManager::IoRtsManager()
    {
        sIoRtsManager = this;
        // Initialize IO objects
        InitializeIo();
        // Initialize MQTT objects
        InitializeMqtt();
        // Start everything
        if (mIoHome != nullptr)
        {
            mIoHome->StartReceive();
        }
        if (sMqttHelper != nullptr)
        {
            sMqttHelper->StartMqttClient();
        }
    }
    void IoRtsManager::Reboot()
    {
        if (mIoHome != nullptr)
            mIoHome->StopReceive();
        esp_restart();
    }
    void IoRtsManager::InitializeIo()
    {
// Initialize IO-HOMECONTROL
#ifdef CONFIG_ENABLE_IOHOMECONTROL
#ifdef CONFIG_IOHOMECONTROL_LOGGING_ENABLED
        bool logging = true;
#else
        bool logging = false;
#endif
#ifdef CONFIG_IOHOMECONTROL_PASSIVE_MODE
        mIoPassive = true;
#else
        mIoPassive = false;
#endif
        mSX1276Radio = new RadioLinks::RadioSX1276(Config::GetSX1276SpiHost(),
                                                   CONFIG_IOHOMECONTROL_SX1276_SPI_CS, CONFIG_IOHOMECONTROL_SX1276_RST,
                                                   CONFIG_IOHOMECONTROL_SX1276_DIO0, CONFIG_IOHOMECONTROL_SX1276_DIO4);
        if (mSX1276Radio != nullptr)
        {
            mIoHome = new iohome::IoHomeControl(mSX1276Radio, loggerCallback, deviceStatusCallback);
            if (mIoHome != nullptr)
            {
                mIoHome->SetVerbose(logging);
                mIoHome->Begin(CONFIG_IOHOMECONTROL_DEFAULT_NODEID, CONFIG_IOHOMECONTROL_DEFAULT_KEY, mIoPassive);
                mIoHome->ConfigureRadio(CONFIG_IOHOMECONTROL_DEFAULT_TX_POWER);
            }
        }
#endif // ENABLE_IOHOMECONTROL
    }
    void IoRtsManager::InitializeMqtt()
    {
        if (MqttConfig::isEnabled())
        {
            // Create MQTT helper
            sMqttHelper = new MqttHelpers(this);
        }
        else
        {
            sMqttHelper = nullptr;
        }
    }
}
