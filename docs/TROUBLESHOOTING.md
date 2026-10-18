# Troubleshooting

## Screen Is Black

- Check USB power and cable quality.
- Confirm you are building `esp32-2432s028r`.
- Confirm `TFT_BL=21`.
- Confirm `TFT_BACKLIGHT_ON=HIGH`.
- If the backlight is off but serial logs show boot, the issue is probably
  backlight wiring or board variant.

## Backlight Is On but Image Is Corrupted

Start with these known-good flags:

- `ILI9341_2_DRIVER`
- `USE_HSPI_PORT`
- `TFT_INVERSION_OFF`
- `CYD_TFT_SWAP_BYTES=1`
- `CYD_DOT_RENDERER=1`

If colors are inverted or swapped, the first things to inspect are
`TFT_INVERSION_OFF`/`ON` and `CYD_TFT_SWAP_BYTES`. This fork ships
`TFT_INVERSION_OFF` for common CYD panels; some clones need it back to `ON`.

## Image Is Upside Down

Change:

```ini
-DCYD_TFT_ROTATION=2
```

Common TFT_eSPI rotations are `0`, `1`, `2`, and `3`. This firmware is tuned
for `2` on the tested board.

If you change rotation and use touch, make sure touch mapping still matches the
screen.

## Touch Is Offset or Mirrored

Check:

- touch pins in `platformio.ini`;
- `lib/Touch/CYD/CydTouch.cpp`;
- display rotation;
- touch swap/inversion settings in `lib/Matrix/CYD/CydMatrixSettings.h`.

Touch is optional. The aquarium does not need touch to stay alive.

## Time Is Wrong

- If there is no `include/WifiSecrets.h`, the firmware uses compile-time
  fallback.
- Check Wi-Fi name and password in `include/WifiSecrets.h`.
- Check timezone in `CYD_TIMEZONE`.
- Make sure your Wi-Fi network can reach NTP servers.
- Watch serial monitor for `clock ntp=...` messages.

## Auto-Brightness Seems Wrong

The tested board has a light sensor on GPIO34, but CYD clones vary.

Default mapping:

- lower raw ADC value = brighter room;
- higher raw ADC value = darker room;
- raw `<= 10` -> `100%` backlight;
- raw `>= 650` -> `38%` backlight.

Tune these flags in `platformio.ini`:

```ini
-DCYD_AUTO_BACKLIGHT_SENSOR_PIN=34
-DCYD_AUTO_BACKLIGHT_BRIGHT_RAW=10
-DCYD_AUTO_BACKLIGHT_DARK_RAW=650
-DCYD_AUTO_BACKLIGHT_MIN_PERCENT=38
-DCYD_AUTO_BACKLIGHT_MAX_PERCENT=100
```

## Build Errors

- Use the `esp32-2432s028r` environment.
- Delete `.pio/` and rebuild if dependency state gets weird.
- Make sure PlatformIO can download dependencies.
- If TFT_eSPI complains about setup, confirm `-DUSER_SETUP_LOADED` and the TFT
  pin flags are present in `platformio.ini`.

## Performance

The firmware uses a row/tile buffer and dot renderer because the common
ESP32-WROOM-32 CYD board has limited RAM and no PSRAM.

The animation is tuned for visual richness over maximum FPS. If you want more
speed, reduce creature population and autonomous food rate before changing TFT
driver settings.
