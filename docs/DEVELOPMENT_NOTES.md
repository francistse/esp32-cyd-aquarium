# Development Notes

This repository is a focused CYD adaptation of the OpenMatrix aquarium code.

## Why a Dot Renderer?

The original OpenMatrix target is a HUB75 RGB LED matrix. A TFT LCD does not
look like an LED matrix by default, so this firmware renders each logical
aquarium pixel as a small separated dot. The result is closer to the original
pixel-art / LED-panel feel on a 240x320 ILI9341 screen.

## Logical Resolution

The aquarium runs at `80x106` logical pixels and is scaled into the 240x320 TFT
viewport. This keeps the scene readable, leaves room for the dot-grid look, and
fits the memory limits of the no-PSRAM board.

## Framebuffer Choice

Full RGB565 framebuffer rendering is disabled for the CYD target:

```ini
-DCYD_FRAMEBUFFER_RENDERER=0
```

The firmware uses row/tile buffering instead. This is more realistic for common
ESP32-WROOM-32 CYD boards.

## Color Calibration

The TFT cannot perfectly match a HUB75 LED matrix. The current profile is tuned
for the tested ILI9341 panel:

- foreground creatures are kept saturated and readable;
- background water is dark teal rather than pure black;
- TFT inversion and RGB565 byte order are explicitly configured.

Important display flags (this fork):

```ini
-DTFT_INVERSION_OFF
-DCYD_TFT_SWAP_BYTES=1
-DCYD_TFT_TONE_CORRECTION=1
```

Note: upstream used `TFT_INVERSION_ON`. This fork sets `TFT_INVERSION_OFF` so a
common CYD panel renders true colors instead of a photographic negative. The
touch mapping in `lib/Matrix/CYD/CydMatrixSettings.h` also inverts the X axis
(`TOUCH_INVERT_X = true`) because the board's XPT2046 reports raw X opposite to
screen X.

## Autonomous Life

The aquarium is designed to run without touch input.

The CYD target enables:

- curated starting population;
- mostly small/medium creatures with occasional larger accents;
- autonomous food events;
- keep-inside behavior to reduce empty-screen periods;
- fixed ideal environment values.

Touch-to-feed remains available, but it is not required.

## Time and Brightness

Wi-Fi/NTP is optional and non-blocking. If Wi-Fi credentials are present, the
device syncs time and disconnects Wi-Fi afterward.

Auto-brightness changes the TFT backlight PWM only. It does not change the
aquarium RGB color calibration.

## Known Limits

- Tested primarily on one ESP32-2432S028R / CYD hardware variant.
- CYD clones may require display, touch, or light-sensor adjustments.
- The animation is tuned for the small TFT and may not match HUB75 brightness
  or contrast exactly.
- The public target is the CYD firmware, not the full original OpenMatrix web
  controller.
