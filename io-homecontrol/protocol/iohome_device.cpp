#include "iohome_device.hpp"

namespace iohome
{
    bool deviceTypeSupportsTilt(DeviceType type)
    {
        switch (type)
        {
        case DeviceType::VENETIAN_BLIND:
        case DeviceType::EXTERNAL_VENETIAN_BLIND:
        case DeviceType::LOUVRE_BLIND:
        case DeviceType::BLIND:
            return true;
        default:
            return false;
        }
    }
    std::string IoDeviceManufacturer(Manufacturer mf)
    {
        switch (mf)
        {
        case Manufacturer::VELUX:
            return "Velux";
        case Manufacturer::SOMFY:
            return "Somfy";
        case Manufacturer::HONEYWELL:
            return "Honeywell";
        case Manufacturer::HORMANN:
            return "Hormann";
        case Manufacturer::ASSA_ABLOY:
            return "Assa Abloy";
        case Manufacturer::NIKO:
            return "Niko";
        case Manufacturer::WINDOW_MASTER:
            return "Window Master";
        case Manufacturer::RENSON:
            return "Renson";
        case Manufacturer::CIAT:
            return "Ciat";
        case Manufacturer::SECUYOU:
            return "Secuyou";
        case Manufacturer::OVERKIZ:
            return "Overkiz";
        case Manufacturer::ATLANTIC_GROUP:
            return "Atlantic";
        case Manufacturer::UNKNOWN:
        default:
            return "unknown";
        }
    }
}