#ifndef TANK_CAMERA_H
#define TANK_CAMERA_H

#include <Arduino.h>
#include <math.h>
#include <stdint.h>

#include "SpeciesCatalog.h"

// Crops the tank simulation for a closer view. Physics stay full-tank.
struct TankCamera {
  uint16_t tankWidth = 80;
  uint16_t tankHeight = 106;
  uint16_t viewportX = 0;
  uint16_t viewportY = 1;
  uint16_t viewportWidth = 240;
  uint16_t viewportHeight = 318;

  float zoom = 1.0f;  // 1.0 wide, 2.0 near
  float originX = 0.0f;
  float originY = 0.0f;
  float targetZoom = 1.0f;
  float targetOriginX = 0.0f;
  float targetOriginY = 0.0f;
  int8_t followFishIndex = -1;
  SpeciesId followSpecies = SpeciesId::Clownfish;
  bool following = false;

  static constexpr float kNearZoom = 2.0f;
  static constexpr float kWideZoom = 1.0f;
  static constexpr float kEase = 0.18f;

  void configure(uint16_t width, uint16_t height, uint16_t vpX, uint16_t vpY,
                 uint16_t vpW, uint16_t vpH) {
    tankWidth = width;
    tankHeight = height;
    viewportX = vpX;
    viewportY = vpY;
    viewportWidth = vpW;
    viewportHeight = vpH;
  }

  uint16_t windowWidth() const {
    const float z = max(1.0f, zoom);
    return max<uint16_t>(1, static_cast<uint16_t>(tankWidth / z));
  }

  uint16_t windowHeight() const {
    const float z = max(1.0f, zoom);
    return max<uint16_t>(1, static_cast<uint16_t>(tankHeight / z));
  }

  bool isNear() const { return zoom >= 1.5f; }

  void clampOrigin() {
    const float maxX = static_cast<float>(tankWidth - windowWidth());
    const float maxY = static_cast<float>(tankHeight - windowHeight());
    originX = constrain(originX, 0.0f, max(0.0f, maxX));
    originY = constrain(originY, 0.0f, max(0.0f, maxY));
    targetOriginX = constrain(targetOriginX, 0.0f, max(0.0f, maxX));
    targetOriginY = constrain(targetOriginY, 0.0f, max(0.0f, maxY));
  }

  void lookAt(float worldX, float worldY, float nextZoom) {
    targetZoom = nextZoom;
    const float winW = tankWidth / max(1.0f, nextZoom);
    const float winH = tankHeight / max(1.0f, nextZoom);
    targetOriginX = worldX - winW * 0.5f;
    targetOriginY = worldY - winH * 0.5f;
    const float maxX = static_cast<float>(tankWidth) - winW;
    const float maxY = static_cast<float>(tankHeight) - winH;
    targetOriginX = constrain(targetOriginX, 0.0f, max(0.0f, maxX));
    targetOriginY = constrain(targetOriginY, 0.0f, max(0.0f, maxY));
  }

  void zoomOut() {
    following = false;
    followFishIndex = -1;
    lookAt(tankWidth * 0.5f, tankHeight * 0.5f, kWideZoom);
  }

  void follow(float worldX, float worldY, int8_t fishIndex, SpeciesId species) {
    following = true;
    followFishIndex = fishIndex;
    followSpecies = species;
    lookAt(worldX, worldY, kNearZoom);
  }

  void updateFollow(float worldX, float worldY) {
    if (!following) {
      return;
    }
    lookAt(worldX, worldY, kNearZoom);
  }

  void easeStep() {
    zoom += (targetZoom - zoom) * kEase;
    originX += (targetOriginX - originX) * kEase;
    originY += (targetOriginY - originY) * kEase;
    if (fabsf(targetZoom - zoom) < 0.01f) {
      zoom = targetZoom;
    }
    clampOrigin();
  }

  void screenToWorld(uint16_t screenX, uint16_t screenY, float* worldX,
                     float* worldY) const {
    const int32_t relativeX =
        static_cast<int32_t>(screenX) - static_cast<int32_t>(viewportX);
    const int32_t relativeY =
        static_cast<int32_t>(screenY) - static_cast<int32_t>(viewportY);
    const float u = constrain(
        static_cast<float>(relativeX) / static_cast<float>(viewportWidth), 0.0f,
        0.999f);
    const float v = constrain(
        static_cast<float>(relativeY) / static_cast<float>(viewportHeight),
        0.0f, 0.999f);
    if (worldX != nullptr) {
      *worldX = originX + u * windowWidth();
    }
    if (worldY != nullptr) {
      *worldY = originY + v * windowHeight();
    }
  }
};

#endif  // TANK_CAMERA_H
