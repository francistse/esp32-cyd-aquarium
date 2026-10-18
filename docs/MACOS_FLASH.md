# macOS Build and Flash

Flash the ESP32-2432S028R / Cheap Yellow Display aquarium from a Mac using
PlatformIO Core.

This fork already pins the Arduino ESP32 core 3.x platform, turns TFT inversion
off for common CYD panels, and defaults to a safer `115200` upload baud.

## 1. Install Tools

Install Homebrew if you do not have it yet: https://brew.sh/

Then install PlatformIO Core:

```bash
brew install platformio
```

Or:

```bash
curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
python3 get-platformio.py
```

Confirm:

```bash
pio --version
```

### USB serial chip (CH340)

Many CYD boards use a CH340 USB-serial bridge (`VID:PID=1A86:7523`).

On recent macOS versions the board usually appears as `/dev/cu.usbserial-*`
without an extra driver. If the port never shows up after plugging in USB:

1. Try another cable that carries data, not charge-only.
2. Try another USB port or a powered hub.
3. Install a current CH340 macOS driver from the chip vendor if needed, then
   reboot.

## 2. Clone This Repository

```bash
git clone https://github.com/francistse/esp32-cyd-aquarium.git
cd esp32-cyd-aquarium
```

This fork already carries the macOS + CYD panel fixes. If you instead start
from the upstream [Lagerpun/esp32-cyd-aquarium](https://github.com/Lagerpun/esp32-cyd-aquarium),
apply this fork's `platformio.ini` changes for macOS + CYD panel fixes:

- pioarduino ESP32 platform for Arduino core 3.x (`ledcAttach`)
- `TFT_INVERSION_OFF`
- `upload_speed = 115200`
- manual backlight touch control

## 3. Optional Wi-Fi Secrets

Wi-Fi is only needed for NTP time sync. The aquarium still runs without it.

```bash
cp include/WifiSecrets.example.h include/WifiSecrets.h
```

Edit `include/WifiSecrets.h` with your SSID, password, and timezone.

Hong Kong example timezone string:

```c
#define CYD_TIMEZONE "HKT-8"
```

Do not commit `include/WifiSecrets.h`.

## 4. Find the Serial Port

Plug the CYD into USB, then run:

```bash
ls /dev/cu.usb*
```

Typical names:

```text
/dev/cu.usbserial-2110
/dev/cu.usbserial-0001
/dev/cu.wchusbserial*
```

Or:

```bash
pio device list
```

Use the `cu.*` device on macOS, not `tty.*`.

## 5. Build

```bash
pio run -e esp32-2432s028r
```

The first build downloads the pioarduino ESP32 platform, Arduino core 3.x, and
libraries. That can take several minutes.

## 6. Flash

Replace the port with yours:

```bash
pio run -e esp32-2432s028r -t upload --upload-port /dev/cu.usbserial-2110
```

This fork uses `upload_speed = 115200`. Faster rates such as `921600` can make
some CH340 CYD boards stop responding during flash.

### If upload fails

- Unplug and replug USB, wait a few seconds, retry.
- Hold the board `BOOT` button, start upload, release after it connects.
- Confirm nothing else owns the serial port (Serial Monitor, Arduino IDE,
  another `pio device monitor`).
- Keep `upload_speed = 115200` in `platformio.ini`.

## 7. Serial Monitor

```bash
pio device monitor --environment esp32-2432s028r --baud 115200 --port /dev/cu.usbserial-2110
```

Expected boot lines:

```text
CYD Aquarium boot
logical=80x106 physical=240x320 viewport=0,1 240x318
autonomous life=on
CYD TFT rotation=2
CYD TFT pushImage swapBytes=on
CYD TFT backlight pwm=on pin=21
backlight manual=40
backlight control=touch top, left dims, right brightens
```

Without Wi-Fi credentials you may also see:

```text
clock ntp=skipped reason=no_wifi_credentials
```

That is normal. The clock falls back to firmware compile time.

Press `Ctrl+C` to leave the monitor.

## 8. After Flash

- Colors should look like a normal aquarium scene, not a photographic negative.
- Backlight starts at about `40%`.
- Touch the **top** of the screen to change brightness:
  - left side dims
  - right side brightens
- Touch below the top strip to drop food for the fish.

## Related Docs

- [Build and flash overview](BUILD_AND_FLASH.md)
- [Hardware notes](HARDWARE.md)
- [Troubleshooting](TROUBLESHOOTING.md)
- [Wi-Fi and NTP](WIFI_TIME.md)
