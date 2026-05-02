# IO Devices management

## Operations

Following operations can be performed to manage IO devices.

### Discover and pair

_Discover and pair_ is a 2 steps operation:
- Discover an IO device (device must be in pairing mode, check your device documentation to enter this state)
- Pair with the device by sending the system key to it.

After this operation, the device:
- Will accept to perform all operation authenticated by system key
- Will not accept to pair to a new 2W manager/coordinator.

> [!NOTE]
> That's why you can't use discover and pair if your device has been paired to a 2W manager in the past and you don't know the system key (reset is required, check device documentation to perform a reset using power supply or registered remote).

### Add device

_Add device_ operation can be used if your device is already paired with a known system key and you want to manage this device with your ESP32 board.

### Remove device

_Remove device_ operation will remove the device from the list of managed devices inside ESP32. It will not erase the system key inside the device (only a reset can do that). If you want to manage a previously removed device, you can use the _Add device_ operation as the device already contains the system key.

### Rename device

_Rename device_ or _Set name_ operation permits to change the name stored inside the device.

> [!NOTE]
> This name will be lost if you reset the device to factory state.

### Invert device

If your device is inverted (_Open_ closes the device and _Close_ opens the device), you can use _Invert device_ operation to fix this issue.

### Link a remote to a IO-HomeControl device

_Link remote_ operation permits to declare to the ESP32 a remote that can control a device, by linking the remote ID and the device ID.  
If the ESP32 detects an order sents by a remote (identified by the remote ID on the radio link), the ESP32 will send an update request to the linked device to get its updated status.

### Remove remote

_Remove remote_ operation unlinks a remote from all previously linked devices.

> [!NOTE]
> This link is only removed inside ESP32, the remote itself will still be able to control the device.

### Normal operation

All other operation are used to control the device: open, close, stop, set to position, ...

## Storage

The IO devices are stored in ESP32 Flash as JSON files in a dedicated LittleFS partition.
Each JSON file stores the information for only one IO device, including linked remote(s). Each file is named _io_XXXXXX_ where XXXXXX is the device ID.

> [!TIP]
> You can also use ESP-IDF tools to erase the LittleFS partition in Flash (address 0x377000 and size 0x40000, see [partition.csv](../partitions.csv))