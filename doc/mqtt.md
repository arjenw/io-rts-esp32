# MQTT interface

## Configuration

See [Project configuration](configuration.md#mqtt-configuration-parameters) and [Command line configuration](command_line.md#configure-mqtt-client) to configure MQTT client, discovery prefix and topic prefix.

## Discovery

Discovery is compatible with Home Assistant discovery, that automatically adds devices to Home Assistant without any manual configuration from user.
See [Home Assistant MQTT discovery](https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery) for more details.

### Controller discovery

Main features provided by the ESP32 are exposed to _MQTT_DISCOVERY_PREFIX/config_
By default, _MQTT_DISCOVERY_PREFIX_ is equal to _homeassistant/device/io-rts_, so complete path is _homeassistant/device/io-rts/config_

Discovery message exposes controller description and mandatory fields required by Home Assistant. In addition, it also exposes some features and configuration parameters like:
- IO logging configuration (change is applied after reboot)
- IO passive mode configuration (change is applied after reboot)
- IO Tx power configuration (change is applied after reboot)
- _Reboot_ button: this button orders an immediate soft reboot of the controller
- _Discover IO device_ button: this button launches a [device discover operation](io_devices_management.md#discover-and-pair)
- _Add IO device_ input: expected format is the IO device ID to add (6 hexa characters, like 1290AF), see [Add device](io_devices_management.md#add-device)
- _Remove IO device_ input: expected format is the IO device ID to remove (6 hexa characters, like 1290AF), see [Remove device](io_devices_management.md#remove-device)
- _Change IO device name_ input: expected format is XXXXXX;YYYYYY where XXXXXX is the IO device ID to rename and YYYYYY is the new device name (up to 16 characters), see [Rename device](io_devices_management.md#rename-device)
- _Invert IO device_ input: expected format is the IO device ID to invert (6 hexa characters, like 1290AF), see [Invert device](io_devices_management.md#invert-device)
- _Link IO device to remote_ input: expected format is XXXXXX;YYYYYY where XXXXXX is the IO device ID and YYYYYY is the IO remote ID, see [Link a remote to a IO-HomeControl device](io_devices_management.md#link-a-remote-to-a-io-homecontrol-device)
- _Remove IO remote_ input: expected format is the IO remote ID to remove (6 hexa characters, like 1290AF), see [Remove remote](io_devices_management.md#remove-remote)

Here is an example of discovery message sent to this topic:
```
{
	"dev":	{
		"ids":	"io-rts-esp32",
		"name":	"io-rts-esp32",
		"mf":	"nicolas5000",
		"sw":	"ff68ac1-dirty"
	},
	"o":	{
		"name":	"io-rts-esp32",
		"url":	"https://github.com/nicolas5000/io-rts-esp32",
		"sw":	"ff68ac1-dirty"
	},
	"cmps":	{
		"reboot":	{
			"p":	"button",
			"unique_id":	"button_reboot",
			"name":	"Reboot",
			"command_topic":	"io-rts/button_reboot/set"
		},
		"IoLogging":	{
			"p":	"switch",
			"unique_id":	"IoLogging",
			"name":	"Enable IO logging",
			"optimistic":	true,
			"state_topic":	"io-rts/config_io/state",
			"value_template":	"{{ value_json.IoLogging }}",
			"command_topic":	"io-rts/config_io/set",
			"command_template":	"{\"IoLogging\": \"{{ value }}\"}"
		},
		"IoPassive":	{
			"p":	"switch",
			"unique_id":	"IoPassive",
			"name":	"Enable IO Passive mode",
			"optimistic":	true,
			"state_topic":	"io-rts/config_io/state",
			"value_template":	"{{ value_json.IoPassive }}",
			"command_topic":	"io-rts/config_io/set",
			"command_template":	"{\"IoPassive\": \"{{ value }}\"}"
		},
		"IoTxPower":	{
			"p":	"number",
			"unique_id":	"IoTxPower",
			"name":	"IO Tx Power",
			"optimistic":	true,
			"min":	0,
			"max":	20,
			"state_topic":	"io-rts/config_io/state",
			"value_template":	"{{ value_json.IoTxPower }}",
			"command_topic":	"io-rts/config_io/set",
			"command_template":	"{\"IoTxPower\": {{ value }} }"
		},
		"discover":	{
			"p":	"button",
			"unique_id":	"button_discover",
			"name":	"Discover IO device",
			"command_topic":	"io-rts/button_discover/set"
		},
		"AddIoDevice":	{
			"p":	"text",
			"unique_id":	"AddIoDevice",
			"name":	"Add IO device",
			"command_topic":	"io-rts/manage_io/set",
			"command_template":	"{\"AddIoDevice\": \"{{ value }}\"}"
		},
		"RemoveIoDevice":	{
			"p":	"text",
			"unique_id":	"RemoveIoDevice",
			"name":	"Remove IO device",
			"command_topic":	"io-rts/manage_io/set",
			"command_template":	"{\"RemoveIoDevice\": \"{{ value }}\"}"
		},
		"InvertIoDevice":	{
			"p":	"text",
			"unique_id":	"InvertIoDevice",
			"name":	"Invert IO device",
			"command_topic":	"io-rts/manage_io/set",
			"command_template":	"{\"InvertIoDevice\": \"{{ value }}\"}"
		},
		"LinkIoRemote":	{
			"p":	"text",
			"unique_id":	"LinkIoRemote",
			"name":	"Link IO device to remote",
			"command_topic":	"io-rts/manage_io/set",
			"command_template":	"{\"LinkIoRemote\": \"{{ value }}\"}"
		},
		"RemoveIoRemote":	{
			"p":	"text",
			"unique_id":	"RemoveIoRemote",
			"name":	"Remove IO remote",
			"command_topic":	"io-rts/manage_io/set",
			"command_template":	"{\"RemoveIoRemote\": \"{{ value }}\"}"
		},
		"SetIoDeviceName":	{
			"p":	"text",
			"unique_id":	"SetIoDeviceName",
			"name":	"Change IO device name",
			"command_topic":	"io-rts/manage_io/set",
			"command_template":	"{\"SetIoDeviceName\": \"{{ value }}\"}"
		}
	},
	"availability_topic":	"io-rts/status"
}
```

### Device discovery

Each IO device is exposed to MQTT with a discovery message sent to _MQTT_DISCOVERY_PREFIX/device/io_XXXXXX/config_ where XXXXXX is the IO device ID.
By default, _MQTT_DISCOVERY_PREFIX_ is equal to _homeassistant/device/io-rts_, so complete path is _homeassistant/device/io_XXXXXX/config_

Discovery message exposes device description and mandatory fields required by Home Assistant. In addition, it also exposes features depending on device type.

Here is an example of discovery message for a shutter:
```
{
	"dev":	{
		"ids":	"io_1290AF",
		"name":	"My shutter",
		"mf":	"Somfy",
		"sn":	"1290AF",
		"via_device":	"io-rts-esp32"
	},
	"o":	{
		"name":	"io-rts-esp32",
		"url":	"https://github.com/nicolas5000/io-rts-esp32",
		"sw":	"ff68ac1-dirty"
	},
	"cmps":	{
		"io_1290AF":	{
			"position_closed":	100,
			"position_open":	0,
			"position_topic":	"io-rts/io_1290AF/position",
			"set_position_topic":	"io-rts/io_1290AF/set_position",
			"device_class":	"shutter",
			"p":	"cover",
			"unique_id":	"io_1290AF",
			"name":	"My shutter",
			"command_topic":	"io-rts/io_1290AF/set",
			"state_topic":	"io-rts/io_1290AF/state"
		},
		"io_1290AF_fav":	{
			"p":	"button",
			"unique_id":	"io_1290AF_fav",
			"name":	"My shutter Favorite position",
			"command_topic":	"io-rts/io_1290AF/set_fav_pos"
		},
		"io_1290AF_ident":	{
			"p":	"button",
			"unique_id":	"io_1290AF_ident",
			"name":	"My shutter Identify",
			"command_topic":	"io-rts/io_1290AF/set",
			"payload_press":	"IDENTIFY",
			"device_class":	"identify",
			"entity_category":	"diagnostic"
		}
	},
	"availability_topic":	"io-rts/status"
}
```

## Topics and payloads

All topics are prefixed by _MQTT_TOPIC_PREFIX_, that is by default _io-rts_  
All topics and payloads are described in discovery messages and compatible with Home Assistant, so they will not be described here. See [Home Assistant MQTT documentation](https://www.home-assistant.io/integrations/mqtt/) for more details.