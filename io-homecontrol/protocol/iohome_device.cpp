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

    bool hasReachedTargetPosition(uint16_t targetPos, uint16_t currentPos)
    {
        if (targetPos > CMD_PARAM_STATUS_POS_MAX || currentPos > CMD_PARAM_STATUS_POS_MAX)
            return false;
        uint16_t diff = (targetPos > currentPos) ? (targetPos - currentPos) : (currentPos - targetPos);
        return diff <= CMD_PARAM_STATUS_POS_TOLERANCE;
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
    std::string IoDeviceType(DeviceType type)
    {
        switch (type)
        {
        case DeviceType::VENETIAN_BLIND:
            return "Venetian blind";
        case DeviceType::ROLLER_SHUTTER:
            return "Roller shutter";
        case DeviceType::AWNING:
            return "Awning";
        case DeviceType::WINDOW_OPENER:
            return "Window opener";
        case DeviceType::GARAGE_OPENER:
            return "Garage opener";
        case DeviceType::LIGHT:
            return "Light";
        case DeviceType::GATE_OPENER:
            return "Gate opener";
        case DeviceType::ROLLING_DOOR_OPENER:
            return "Rolling door opener";
        case DeviceType::LOCK:
            return "Lock";
        case DeviceType::BLIND:
            return "Blind";
        case DeviceType::UNKNOWN_0B:
            return "Unknown 0xOB";
        case DeviceType::BEACON:
            return "Beacon";
        case DeviceType::DUAL_SHUTTER:
            return "Dual shutter";
        case DeviceType::HEATING_TEMPERATURE_INTERFACE:
            return "Heating temperature interface";
        case DeviceType::ON_OFF_SWITCH:
            return "On/off switch";
        case DeviceType::HORIZONTAL_AWNING:
            return "Horizontal Awning";
        case DeviceType::EXTERNAL_VENETIAN_BLIND:
            return "External venetian blind";
        case DeviceType::LOUVRE_BLIND:
            return "Louvre blind";
        case DeviceType::CURTAIN_TRACK:
            return "Curtain track";
        case DeviceType::VENTILATION_POINT:
            return "Ventilation point";
        case DeviceType::EXTERIOR_HEATING:
            return "Exterior heating";
        case DeviceType::HEAT_PUMP:
            return "Heat pump";
        case DeviceType::INTRUSION_ALARM:
            return "Intrusion alarm";
        case DeviceType::SWINGING_SHUTTER:
            return "Swinging shutter";
        case DeviceType::UNKNOWN:
        default:
            return "unknown";
        }
    }
}