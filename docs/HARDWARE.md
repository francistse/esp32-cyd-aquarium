# Hardware

This firmware is tested on the ESP32-2432S028R / Cheap Yellow Display style
board.

## Tested Board

- ESP32-2432S028R / CYD style board.
- ESP32-WROOM-32 class module.
- 2.8 inch 240x320 ILI9341 TFT.
- XPT2046 resistive touch controller.
- USB serial through a CH340-class bridge on many boards.
- Onboard ambient light sensor connected to GPIO34 on the tested board.

CYD clones vary. Some boards use different LCD controller variants, different
backlight wiring, or omit parts of the light-sensor circuit.

The default aquarium build uses the CYD display, touch controller, and optional
onboard light sensor only. External temperature, humidity, or CO2 sensors are
not required for this adaptation.

## Known Working Display Setup

The default `esp32-2432s028r` PlatformIO environment in *this fork* uses:

- `ILI9341_2_DRIVER`
- `USE_HSPI_PORT`
- `TFT_BACKLIGHT_ON=HIGH`
- `TFT_INVERSION_OFF` (not `ON` — off so common CYD panels show true colors)
- `CYD_TFT_SWAP_BYTES=1`
- `CYD_TFT_ROTATION=2`
- `CYD_DOT_RENDERER=1`
- `CYD_FRAMEBUFFER_RENDERER=0`
- Custom touch mapping in `lib/Matrix/CYD/CydMatrixSettings.h`:
  - `TOUCH_INVERT_X = true` (CYD XPT2046 reports raw X opposite to screen X)
  - `TOUCH_SWAP_XY = false`
  - `TOUCH_INVERT_Y = false`
- Physical display: `240x320`
- Aquarium logical frame: `80x106`
- Viewport: `0,1 240x318`

The upstream project used `TFT_INVERSION_ON`. This fork turns inversion off for
the default build; if your panel needs it back, flip the flag back in
`platformio.ini`.

## Pin Map

| Function | GPIO |
| --- | ---: |
| TFT MISO | 12 |
| TFT MOSI | 13 |
| TFT SCLK | 14 |
| TFT CS | 15 |
| TFT DC | 2 |
| TFT RST | -1 |
| TFT backlight | 21 |
| Touch MOSI | 32 |
| Touch MISO | 39 |
| Touch SCLK | 25 |
| Touch CS | 33 |
| Touch IRQ | 36 |
| Ambient light sensor | 34 |

## Backlight

Backlight is controlled independently from aquarium colors.

- Backlight pin: GPIO21.
- PWM frequency: 5000 Hz.
- PWM resolution: 8 bit.
- The firmware changes the physical TFT backlight, not the RGB aquarium color
  calibration.
- In this fork, **manual touch backlight is the default** (`DCYD_AUTO_BACKLIGHT=0`,
  `DCYD_MANUAL_BACKLIGHT_PERCENT=40`): touch the top strip of the screen, left
  dims and right brightens. Auto-brightness from the light sensor is still
  available by enabling `CYD_AUTO_BACKLIGHT=1`.

## Ambient Light Sensor

The tested board has an onboard LDR circuit on GPIO34.

The observed behavior is inverted:

- lower raw ADC value means brighter room;
- higher raw ADC value means darker room.

The default calibration maps:

- raw `<= 10` to `100%` backlight;
- raw `>= 650` to `38%` backlight.

If your board responds differently, adjust the `CYD_AUTO_BACKLIGHT_*` build
flags in `platformio.ini`.
