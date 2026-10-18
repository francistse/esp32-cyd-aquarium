#ifndef STARBODY_H
#define STARBODY_H

#include <Arduino.h>
#include <Body/Body.h>
#include <TankDetail.h>

class StarBody : public Body {
 protected:
  uint8_t rad, length;
  float starAngle;
  float rotationSpeed;

 public:
  StarBody(Matrix* m, Head* head, Tail* tail, Fin* fin)
      : Body(m, head, tail, fin) {
    length = 5;
    rad = 2;
    rotationSpeed = random(5, 10) / 1000.0f;
    starAngle = random(TWO_PI);
    colorPalette = new ColorPalette(2, false);
    std::vector<CHSV> palette = {
        CHSV(240, 160, 255),  // pink
        CHSV(250, 80, 255),   // lighter tips
    };
    colorPalette->setColors(palette);
    type = "Star";
  }

  void display() override {
    float velocityMagnitude = max(vel.mag(), 0.1f);
    starAngle += velocityMagnitude * rotationSpeed;
    starAngle = fmod(starAngle, TWO_PI);
    if (starAngle < 0) starAngle += TWO_PI;

    const CRGB pink = colorPalette->colors[0];
    const CRGB tip = colorPalette->colors[1];
    const float armLen = length * size;
    const float armRad = max(1.0f, rad * size);

    // Always five thick arms — no stick-and-ball insect mode.
    constexpr int kArms = 5;
    for (int i = 0; i < kArms; i++) {
      const float armAngle = starAngle + (TWO_PI / kArms * i);
      PVector tipPt = PVector::fromAngle(armAngle);
      tipPt.setMag(armLen);
      tipPt += pos;

      PVector left = PVector::fromAngle(armAngle - PI / kArms);
      left.setMag(armRad);
      left += pos;
      PVector right = PVector::fromAngle(armAngle + PI / kArms);
      right.setMag(armRad);
      right += pos;

      matrix->foreground->fillTriangle(tipPt.x, tipPt.y, left.x, left.y,
                                       right.x, right.y, pink);
      matrix->foreground->fillCircle(tipPt.x, tipPt.y,
                                     max(1, static_cast<int>(armRad * 0.55f)),
                                     tip);
    }

    matrix->foreground->fillCircle(
        pos.x, pos.y, max(1, static_cast<int>(armRad * 0.7f)), pink);

    if (TankDetail::nearView) {
      matrix->foreground->drawPixel(pos.x, pos.y, tip);
    }
  }
};

#endif
