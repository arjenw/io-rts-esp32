# Supported devices

The project has been fully tested on these IO devices:
- Somfy RS100 SOLAR IO roller shutter
- Velux solar shutter
- Somfy Dexxo Smart IO 800 (garage door with on/off light and on/off switch)

The project should work with most of the IO devices of type blind, shutter, awning, window opener, garage opener, light, gate opener, rolling door opener, lock, blind and on/off switch.  
Please provide feedback for models working (or not) on your side.

# Supporting new devices

If your device is not fully supported, it will be necessary to collect information to add support for your device.  
Here are a few questions to start:
- Is [discover and pair](io_devices_management.md#discover-and-pair) working?
- Which features normally supported by your device are currently working?
- Which features normally supported by your device are not working / not available?

Here is what you can do to collect information:
- Put the ESP32 in Passive / listening mode
- Use your remote to control your device, for each frame captured by the ESP32 note which remote order was asked
- If you already have a 2W box (like Tahoma or connectivity kit), use it to control your device and again for each frame captured by the ESP32 note which order was asked.

Then open an issue / improvement first check if there is no open issue for your device) and attach all the information you collected so we can see what can be done directly and what missing information / testing nedds to be done.