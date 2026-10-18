#pragma once

#include <stdint.h>

namespace CydMatrixSettings {

// Logical aquarium panel. This matches the browser preview's 80x106-device
// mode.
inline constexpr uint16_t LOGICAL_WIDTH = 80;
inline constexpr uint16_t LOGICAL_HEIGHT = 106;

// ESP32-2432S028R / CYD portrait target.
inline constexpr uint16_t PHYSICAL_WIDTH = 240;
inline constexpr uint16_t PHYSICAL_HEIGHT = 320;

// Matches the simulator fit-scaling choice:
// 80x106 scaled by 3 becomes 240x318, centered in 240x320.
inline constexpr uint16_t VIEWPORT_X = 0;
inline constexpr uint16_t VIEWPORT_Y = 1;
inline constexpr uint16_t VIEWPORT_WIDTH = 240;
inline constexpr uint16_t VIEWPORT_HEIGHT = 318;

inline constexpr uint16_t screenXForLogicalEdge(uint16_t logicalX) {
  return VIEWPORT_X + (logicalX * VIEWPORT_WIDTH) / LOGICAL_WIDTH;
}

inline constexpr uint16_t screenYForLogicalEdge(uint16_t logicalY) {
  return VIEWPORT_Y + (logicalY * VIEWPORT_HEIGHT) / LOGICAL_HEIGHT;
}

inline constexpr uint16_t logicalXFromScreen(int32_t screenX) {
  const int32_t relativeX = screenX - static_cast<int32_t>(VIEWPORT_X);
  if (relativeX <= 0) return 0;
  if (relativeX >= static_cast<int32_t>(VIEWPORT_WIDTH)) return LOGICAL_WIDTH - 1;
  return static_cast<uint16_t>((relativeX * LOGICAL_WIDTH) / VIEWPORT_WIDTH);
}

inline constexpr uint16_t logicalYFromScreen(int32_t screenY) {
  const int32_t relativeY = screenY - static_cast<int32_t>(VIEWPORT_Y);
  if (relativeY <= 0) return 0;
  if (relativeY >= static_cast<int32_t>(VIEWPORT_HEIGHT)) return LOGICAL_HEIGHT - 1;
  return static_cast<uint16_t>((relativeY * LOGICAL_HEIGHT) / VIEWPORT_HEIGHT);
}

// Candidate ESP32-2432S028R/CYD pins. Verify on the exact hardware revision
// before treating these as final.
inline constexpr int TFT_MISO_PIN = 12;
inline constexpr int TFT_MOSI_PIN = 13;
inline constexpr int TFT_SCLK_PIN = 14;
inline constexpr int TFT_CS_PIN = 15;
inline constexpr int TFT_DC_PIN = 2;
inline constexpr int TFT_RST_PIN = -1;
inline constexpr int TFT_BL_PIN = 21;

inline constexpr int TOUCH_MOSI_PIN = 32;
inline constexpr int TOUCH_MISO_PIN = 39;
inline constexpr int TOUCH_SCLK_PIN = 25;
inline constexpr int TOUCH_CS_PIN = 33;
inline constexpr int TOUCH_IRQ_PIN = 36;

inline constexpr uint32_t TFT_SPI_FREQUENCY = 40000000;
inline constexpr uint32_t TOUCH_SPI_FREQUENCY = 2500000;

// Conservative XPT2046 calibration defaults. These are good enough for binary
// feeding and first visual smoke tests, but must be calibrated on the exact
// ESP32-2432S028R revision before relying on precise menu coordinates.
inline constexpr int TOUCH_RAW_MIN_X = 200;
inline constexpr int TOUCH_RAW_MAX_X = 3900;
inline constexpr int TOUCH_RAW_MIN_Y = 200;
inline constexpr int TOUCH_RAW_MAX_Y = 3900;
inline constexpr bool TOUCH_SWAP_XY = false;
// ESP32-2432S028R XPT2046 maps raw X opposite to portrait screen X.
inline constexpr bool TOUCH_INVERT_X = true;
inline constexpr bool TOUCH_INVERT_Y = false;

}  // namespace CydMatrixSettings
