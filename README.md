# DueHostTest
A test program to run on a Due to utilise the Native USB port as an OTG Host.

## Files
- `DueOtgStatusReport.ino` - Arduino sketch entrypoint for setup/loop.
- `DueOtgStatusDriver.h` / `DueOtgStatusDriver.cpp` - OTG status reporting driver.

## What it does
- Initializes the Arduino Due Native USB port in host mode via the Arduino `USBHost` stack.
- Reports USB host state transitions (attach, enumeration/reset, addressed, configured, suspend/resume, disconnect).
- Sends status output on the Programming USB serial port at `115200` baud.

## Arduino IDE usage
1. Open `DueOtgStatusReport.ino` in Arduino IDE.
2. Select board: Arduino Due.
3. Select the upload port (Programming Port recommended).
4. Upload the sketch.
5. Open Serial Monitor on the Programming Port at `115200`.
6. Connect a USB device to the Native USB port using a USB OTG cable/adapter.

You should see live status updates for USB enumeration and configuration.

## Example output (Nikon Zf)
Typical Serial Monitor output when a Nikon Zf is connected to the Native USB OTG port:

```text
Due OTG host monitor started on programming USB serial.
OTG status: waiting for device
Native USB: device attach detected
OTG status: attached
Native USB: bus reset complete, enumeration in progress
OTG status: enumerating
Native USB: device address assigned
OTG status: addressed
Native USB: endpoint(s) configured
OTG status: configured
```

Depending on cable, camera USB mode, and power conditions, sequence timing/messages can vary.

## Troubleshooting
- If you see `attached` but never `configured`, use a known-good USB OTG adapter/cable and set the camera USB mode to a data/PTP mode (not charge-only).
- Power the camera from a fully charged battery (or external power) and avoid long/low-quality cables.
- Confirm you are watching the Programming Port Serial Monitor at `115200`.
- If needed, disconnect/reconnect the camera and reset the Due to restart host negotiation.
