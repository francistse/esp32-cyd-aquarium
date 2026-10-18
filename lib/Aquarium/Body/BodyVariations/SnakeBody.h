#ifndef SNAKEBODY_H
#define SNAKEBODY_H

#include <Arduino.h>
#include <Body/Body.h>
#include <TankDetail.h>

class SnakeBody : public Body {
 public:
  SnakeBody(Matrix* m, Head* head, Tail* tail, Fin* fin)
      : Body(m, head, tail, fin) {
    // Cap length so it reads as a moray, not a thread.
    const int numSegments = random(14, 28);
    for (int i = 0; i < numSegments; ++i) {
      segmentPositions.push_back(PVector(0, 0));
    }
    colorPalette = new ColorPalette(numSegments, false);
    std::vector<CHSV> palette;
    palette.reserve(numSegments);
    for (int i = 0; i < numSegments; ++i) {
      // Yellow-green spotted eel.
      const bool spot = (i % 3 == 0);
      palette.push_back(CHSV(spot ? 55 : 70, spot ? 200 : 160, spot ? 220 : 180));
    }
    colorPalette->setColors(palette);
    type = "Snake";
  }

  void drawSegment(uint8_t i, PVector vin, uint8_t r, uint8_t g, uint8_t b,
                   float thickness) {
    PVector dv = vin - segmentPositions[i];
    float segmentAngle = dv.heading();
    const float step = max(1.0f, thickness * 0.85f);
    segmentPositions[i].x = vin.x - cos(segmentAngle) * step;
    segmentPositions[i].y = vin.y - sin(segmentAngle) * step;
    const int rad = max(1, static_cast<int>(thickness));
    matrix->foreground->fillCircle(segmentPositions[i].x, segmentPositions[i].y,
                                   rad, CRGB(r, g, b));
  }

  void display() override {
    const int visible =
        max(4, static_cast<int>((segmentPositions.size() - 1) * size));
    const float headThick = max(1.4f, 2.2f * size);
    for (int8_t i = static_cast<int8_t>(visible - 1); i > -1; i--) {
      const float t = static_cast<float>(i) / max(1, visible - 1);
      const float thickness = headThick * (1.0f - t * 0.65f);
      drawSegment(i + 1, segmentPositions[i], colorPalette->colors[i].r,
                  colorPalette->colors[i].g, colorPalette->colors[i].b,
                  thickness);
    }
    drawSegment(0, pos, colorPalette->colors[0].r, colorPalette->colors[0].g,
                colorPalette->colors[0].b, headThick);

    // Larger head with eye.
    matrix->foreground->fillCircle(pos.x, pos.y,
                                   max(1, static_cast<int>(headThick + 0.5f)),
                                   colorPalette->colors[0]);
    const float eyeX = pos.x + cos(angle) * headThick * 0.4f;
    const float eyeY = pos.y + sin(angle) * headThick * 0.4f;
    matrix->foreground->drawPixel(eyeX, eyeY, CRGB(10, 10, 20));
    if (TankDetail::nearView) {
      matrix->foreground->drawPixel(eyeX + cos(angle + PI / 2),
                                    eyeY + sin(angle + PI / 2),
                                    CRGB(40, 40, 50));
    }
  }
};

#endif
