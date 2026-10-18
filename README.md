# ESP32 CYD Aquarium

A little animated pixel aquarium for the ESP32-2432S028R, better known as the
Cheap Yellow Display. It's a customized fork of
[Lagerpun/esp32-cyd-aquarium](https://github.com/Lagerpun/esp32-cyd-aquarium),
which in turn is adapted from
[Livegrid OpenMatrix](https://github.com/livegrid/OpenMatrix).

<p align="center">
  <img src="https://img.shields.io/badge/license-AGPL--3.0-blue.svg" alt="License: AGPL-3.0">
  <img src="https://img.shields.io/badge/platform-ESP32--2432S028R-orange.svg" alt="Platform: ESP32-2432S028R">
  <img src="https://img.shields.io/badge/built%20with-PlatformIO-f5822a.svg" alt="Built with PlatformIO">
  <a href="https://github.com/sponsors/francistse"><img src="https://img.shields.io/badge/sponsor-GitHub-further?logo=github&logoColor=white&color=green" alt="Sponsor on GitHub"></a>
</p>

<p align="center">
  <img src="docs/images/cyd-aquarium-demo.gif" alt="ESP32 CYD Aquarium running on a Cheap Yellow Display" width="360">
</p>

<p align="center">
  <a href="docs/images/aquarium.mp4">MP4 video</a>
</p>

It turns one of these small 2.8 inch ESP32 touchscreens into a self-running
desk aquarium: moving water, creatures, plants, drifting food, and a 12-hour
clock. Give it Wi-Fi and it syncs the time over NTP; leave Wi-Fi out and it
still runs fine on its own.

This fork keeps the upstream CYD project and adds a richer living tank, a reef
guide, and fixes that make common CYD panels look correct out of the box.

## What It Does

It boots straight into the scene and keeps itself going without you touching
anything. Bigger creatures swim around while smaller stuff drifts in the
background, and the whole thing is drawn on the TFT in a way that mimics a
small LED matrix.

## Enhancements Over Upstream

- **~100-species reef roster** (`SpeciesCatalog.h`) with weighted spawn rates,
  family-based silhouettes, and six size classes.
- **Reef Guide** — a paged, touchable in-tank guide that records which species
  you've discovered (`/field_guide.json` on LittleFS) and shows a family
  silhouettes index plus per-species details.
- **Large fauna** — sharks, whale sharks, and reef mantas alongside the
  classic small reef life.
- **Tank camera** — a zoom/follow crop for a closer view; tap a creature to
  follow it, tap again or use the control to zoom back out.
- **Correct colors on common CYD panels** — TFT inversion is set to
  `TFT_INVERSION_OFF` so the scene does not look like a photographic negative.
- **Inverted touch X axis fixed** — the fork maps the ESP32-2432S028R
  XPT2046's opposite X polarity correctly (`TOUCH_INVERT_X = true`).
- **Manual backlight control** — touch the top strip of the screen to change
  brightness (left side dims, right side brightens); auto-brightness from the
  onboard light sensor stays available.
- **Arduino core 3.x** pinned via the pioarduino ESP32 platform so
  `ledcAttach()` backlight PWM works.
- **Safer `115200` upload baud** for CH340 CYD boards that choke on `921600`.
- **Factory reset also clears the reef guide.**

## Features

- Standalone live aquarium mode for the ESP32-2432S028R / CYD board.
- 240x320 ILI9341 TFT output with an 80x106 logical pixel-aquarium frame.
- Dot-grid renderer that gives the TFT a small LED-matrix look.
- Autonomous fish, stars, turtles, snakes, octopuses, boids, plants, food, and
  animated water.
- Reef guide with species discovery tracking.
- Tank camera with follow and zoom.
- 12-hour clock with AM/PM, seconds, and a date overlay.
- Optional Wi-Fi/NTP time sync, with a compile-time fallback when there's no
  network.
- Wi-Fi disconnects on its own after the NTP sync by default.
- Automatic TFT backlight using the onboard GPIO34 light sensor (optional).
- Manual backlight control via touch.
- Optional touch-to-feed.
- No external environmental sensors needed for the default build.
- Tested on the common ESP32-2432S028R / Cheap Yellow Display hardware.

## Hardware

Tested on:

| Part | Detail |
| --- | --- |
| Board | ESP32-2432S028R / Cheap Yellow Display style |
| Module | ESP32-WROOM-32 class |
| Display | 2.8 inch 240x320 ILI9341 TFT |
| Touch | XPT2046 resistive |
| Light sensor | Onboard, on GPIO34 |

CYD boards and clones vary. This fork defaults to the display, touch, and
light-sensor settings that make the common ESP32-2432S028R panel look correct.
If your board behaves differently, see [docs/HARDWARE.md](docs/HARDWARE.md) and
[docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md).

### Display, backlight, and touch notes (this fork)

- The default build uses **`TFT_INVERSION_OFF`** and `ILI9341_2_DRIVER`, so on a
  typical CYD panel colors read as a normal aquarium scene rather than a
  negative.
- **Backlight** is driven by PWM on GPIO21. The onboard GPIO34 light sensor is
  *polarity-inverted*: a lower raw ADC value means a brighter room, and a
  higher value means a darker room. Auto-brightness maps that inversion; the
  manual touch control is independent of it.
- **Touch X axis is inverted** on the ESP32-2432S028R XPT2046: the controller
  reports raw X opposite to on-screen X, so this fork sets `TOUCH_INVERT_X =
  true` to keep touch aligned with the display.

## Quick Start

Install [PlatformIO](https://platformio.org/) first, then build the default
environment:

```powershell
pio run -e esp32-2432s028r
```

Flash your board, replacing `COMx` with your serial port:

```powershell
pio run -e esp32-2432s028r -t upload --upload-port COMx
```

Open the monitor:

```powershell
pio device monitor --environment esp32-2432s028r --baud 115200 --port COMx
```

Detailed steps are in [docs/BUILD_AND_FLASH.md](docs/BUILD_AND_FLASH.md).
On a Mac, use [docs/MACOS_FLASH.md](docs/MACOS_FLASH.md).

## Wi-Fi and Time

Wi-Fi is optional. Without Wi-Fi credentials, the aquarium still boots and uses
the firmware compile time as a fallback clock.

For NTP time sync, copy:

```text
include/WifiSecrets.example.h
```

to:

```text
include/WifiSecrets.h
```

Then fill in your own Wi-Fi name and password. `WifiSecrets.h` is ignored by
git on purpose.

See [docs/WIFI_TIME.md](docs/WIFI_TIME.md).

## Documentation

- [Hardware notes](docs/HARDWARE.md)
- [Build and flash guide](docs/BUILD_AND_FLASH.md)
- [macOS flash guide](docs/MACOS_FLASH.md)
- [Memory and flash budget](docs/MEMORY_AND_FLASH.md)
- [Wi-Fi and NTP setup](docs/WIFI_TIME.md)
- [Troubleshooting](docs/TROUBLESHOOTING.md)
- [Development notes](docs/DEVELOPMENT_NOTES.md)
- [Attribution](docs/ATTRIBUTION.md)

## Support ☕

This fork is free and open source. If you enjoy it and would like to support
its development, sponsoring on GitHub helps keep the tinkering going. A star,
an issue about your board variant, or a short hardware report are always
welcome too.

<p align="center">
  <a href="https://github.com/sponsors/francistse">
    <img src="https://img.shields.io/badge/GitHub-Sponsors-green?logo=github&logoColor=white" alt="Sponsor on GitHub">
  </a>
</p>

## Attribution / Credits

This project is a fork of
[Lagerpun/esp32-cyd-aquarium](https://github.com/Lagerpun/esp32-cyd-aquarium),
which is a standalone CYD adaptation of
[Livegrid OpenMatrix](https://github.com/livegrid/OpenMatrix), an AGPL-3.0
project for ESP32-driven HUB75 RGB matrices.

Acknowledgments:

- **Lagerpun** for the original ESP32-2432S028R / CYD port this fork builds on.
- **Livegrid** for the OpenMatrix aquarium this is ultimately derived from.

This fork's enhancements (reef roster, reef guide, tank camera, large fauna,
panel-color fixes, manual backlight) were added on top of that upstream work.
It isn't an official Livegrid project.

See [docs/ATTRIBUTION.md](docs/ATTRIBUTION.md).

## License

This project is released under the GNU Affero General Public License v3.0
because the upstream OpenMatrix project is AGPL-3.0.

See [LICENSE](LICENSE).