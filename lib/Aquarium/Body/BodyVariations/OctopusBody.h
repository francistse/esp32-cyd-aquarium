#ifndef OCTOPUSBODY_H
#define OCTOPUSBODY_H

#include <Arduino.h>
#include <Body/Body.h>
#include <TankDetail.h>

class OctopusBody : public Body {
 protected:
  uint8_t rad, tentacleLength;
  std::vector<std::vector<PVector>> tentacleSegments;
  uint8_t numTentacles;

 public:
  OctopusBody(Matrix* m, Head* head, Tail* tail, Fin* fin)
      : Body(m, head, tail, fin) {
    rad = 14;
    numTentacles = 8;
    tentacleLength = 14;

    for (int i = 0; i < numTentacles; ++i) {
      std::vector<PVector> tentacle;
      for (int j = 0; j < OCTOPUS_TENTACLE_SEGMENTS; ++j) {
        tentacle.push_back(PVector(0, 0));
      }
      tentacleSegments.push_back(tentacle);
    }

    colorPalette = new ColorPalette(OCTOPUS_TENTACLE_SEGMENTS + 1, false);
    std::vector<CHSV> palette;
    palette.reserve(OCTOPUS_TENTACLE_SEGMENTS + 1);
    for (int i = 0; i < OCTOPUS_TENTACLE_SEGMENTS + 1; ++i) {
      palette.push_back(CHSV(210 + i * 2, 180, 230));  // purple/red
    }
    colorPalette->setColors(palette);
    type = "Octopus";
  }

  void drawTentacle(uint8_t i, PVector bodyPos) {
    // Arms only from the rear half so it does not read as a bug.
    float rearSpread = PI * 0.95f;
    float backOffsetX = cos(angle) * (rad * size / 6);
    float backOffsetY = sin(angle) * (rad * size / 6);
    PVector backCenter(bodyPos.x - backOffsetX, bodyPos.y - backOffsetY);

    float tentacleAngle =
        angle + PI +
        (i - (numTentacles - 1) / 2.0f) * (rearSpread / (numTentacles - 1));
    PVector tentacleStart(
        backCenter.x + cos(tentacleAngle) * (rad * size / 7),
        backCenter.y + sin(tentacleAngle) * (rad * size / 7));

    PVector current = tentacleStart;
    float segmentLength =
        (tentacleLength * size * 0.75f) / OCTOPUS_TENTACLE_SEGMENTS;
    float time = millis() / 1000.0f;
    float velocityFactor = min(vel.mag() / 2.0f, 1.0f);

    for (uint8_t j = 0; j < OCTOPUS_TENTACLE_SEGMENTS; ++j) {
      PVector dv = current - tentacleSegments[i][j];
      float segmentAngle = dv.heading();
      float movementAngle =
          sin(time * 2 + i * 0.5f + j * 0.3f) * 0.2f * velocityFactor;
      segmentAngle += movementAngle;

      tentacleSegments[i][j].x = current.x - cos(segmentAngle) * segmentLength;
      tentacleSegments[i][j].y = current.y - sin(segmentAngle) * segmentLength;

      CRGB segmentColor = colorPalette->colors[j + 1];
      matrix->foreground->drawLine(current.x, current.y,
                                   tentacleSegments[i][j].x,
                                   tentacleSegments[i][j].y, segmentColor);
      current = tentacleSegments[i][j];
    }
  }

  void display() override {
    uint8_t rad2draw = max<uint8_t>(2, static_cast<uint8_t>(rad * size / 4));
    uint8_t length2draw = max<uint8_t>(3, static_cast<uint8_t>(rad * size / 2));

    for (uint8_t i = 0; i < numTentacles; ++i) {
      drawTentacle(i, pos);
    }

    // Round mantle on top of the arms.
    matrix->foreground->fillCircle(pos.x, pos.y, rad2draw,
                                   colorPalette->colors[0]);
    matrix->foreground->drawCircleArray(pos.x, pos.y, rad2draw / 2, length2draw,
                                        angle, colorPalette->colors[0]);

    if (TankDetail::nearView) {
      // Two eyes on the front.
      const float eyeSpread = rad2draw * 0.55f;
      for (int side = -1; side <= 1; side += 2) {
        const float ex =
            pos.x + cos(angle) * rad2draw * 0.45f +
            cos(angle + PI / 2) * side * eyeSpread * 0.5f;
        const float ey =
            pos.y + sin(angle) * rad2draw * 0.45f +
            sin(angle + PI / 2) * side * eyeSpread * 0.5f;
        matrix->foreground->fillCircle(ex, ey, 1, CRGB(240, 240, 255));
        matrix->foreground->drawPixel(ex, ey, CRGB(10, 10, 20));
      }
    }
  }
};

#endif
