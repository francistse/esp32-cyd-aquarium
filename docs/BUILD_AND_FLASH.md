# Build and Flash

This guide uses PlatformIO and the default `esp32-2432s028r` environment.

On macOS, prefer the dedicated steps in [MACOS_FLASH.md](MACOS_FLASH.md)
(`/dev/cu.usbserial-*`, CH340 notes, and the `115200` upload baud used by this
fork).

## 1. Install Tools

Install one of these:

- VS Code with the PlatformIO extension; or
- PlatformIO Core from https://platformio.org/.

## 2. Clone the Repository

```powershell
git clone https://github.com/francistse/esp32-cyd-aquarium.git
cd esp32-cyd-aquarium
```

## 3. Optional Wi-Fi Secrets

Wi-Fi is only needed for NTP time sync. The aquarium still runs without it.

To enable Wi-Fi/NTP:

```powershell
Copy-Item include/WifiSecrets.example.h include/WifiSecrets.h
```

Then edit `include/WifiSecrets.h` with your own Wi-Fi details.

Do not commit `include/WifiSecrets.h`.

## 4. Build

```powershell
pio run -e esp32-2432s028r
```

## 5. Find the Serial Port

Plug in the board and check which serial port appears.

On Windows this is usually something like:

```text
COM3
COM4
COMx
```

On Linux/macOS it may look like:

```text
/dev/ttyUSB0
/dev/ttyACM0
```

## 6. Upload

Windows example:

```powershell
pio run -e esp32-2432s028r -t upload --upload-port COMx
```

Linux/macOS example:

```bash
pio run -e esp32-2432s028r -t upload --upload-port /dev/ttyUSB0
```

## 7. Monitor

Windows example:

```powershell
pio device monitor --environment esp32-2432s028r --baud 115200 --port COMx
```

Linux/macOS example:

```bash
pio device monitor --environment esp32-2432s028r --baud 115200 --port /dev/ttyUSB0
```

## Expected Boot Signs

The serial log should show lines similar to:

```text
CYD Aquarium boot
logical=80x106 physical=240x320 viewport=0,1 240x318
autonomous life=on
clock wifi_time=on
CYD TFT rotation=2
CYD TFT pushImage swapBytes=on
CYD TFT backlight pwm=on pin=21
backlight manual=40
backlight control=touch top, left dims, right brightens
```

This fork defaults to manual backlight touch control. If auto-brightness is
enabled instead, you will also see `auto_backlight=on pin=34`.

If Wi-Fi credentials are missing, that is okay:

```text
clock ntp=skipped reason=no_wifi_credentials
```

The aquarium should still run using the compile-time fallback clock.
