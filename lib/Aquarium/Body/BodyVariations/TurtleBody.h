#ifndef TURTLEBODY_H
#define TURTLEBODY_H

#include <Arduino.h>
#include <Body/Body.h>
#include <TankDetail.h>

class TurtleBody : public Body {
 protected:
  uint8_t rad, length;

 public:
  TurtleBody(Matrix* m, Head* head, Tail* tail, Fin* fin)
      : Body(m, head, tail, fin) {
    length = 6;
    rad = 3;
    colorPalette = new ColorPalette(3, false);
    std::vector<CHSV> palette = {
        CHSV(85, 180, 200),   // olive shell
        CHSV(70, 140, 160),   // darker rim
        CHSV(55, 120, 180),   // head/flippers
    };
    colorPalette->setColors(palette);
    type = "Turtle";
  }

  void display() override {
    const float s = max(0.35f, size);
    const uint8_t rad2draw = max<uint8_t>(1, static_cast<uint8_t>(rad * s));
    const uint8_t length2draw =
        max<uint8_t>(2, static_cast<uint8_t>(length * (1 + vel.mag() / 20) * s));
    const CRGB shell = colorPalette->colors[0];
    const CRGB rim = colorPalette->colors[1];
    const CRGB limb = colorPalette->colors[2];

    // Shell oval.
    matrix->foreground->drawCircleArray(pos.x, pos.y, rad2draw, length2draw,
                                        angle + PI / 2, shell);
    if (TankDetail::nearView) {
      matrix->foreground->drawCircleArray(pos.x, pos.y,
                                          max<uint8_t>(1, rad2draw / 2),
                                          max<uint8_t>(2, length2draw / 2),
                                          angle + PI / 2, rim);
      // Scute dots.
      for (int i = -1; i <= 1; ++i) {
        const float ox = cos(angle + PI / 2) * i * rad2draw * 0.6f;
        const float oy = sin(angle + PI / 2) * i * rad2draw * 0.6f;
        matrix->foreground->drawPixel(pos.x + ox, pos.y + oy, rim);
      }
    }

    // Head nub.
    const float hx = pos.x + cos(angle) * length2draw * 0.75f;
    const float hy = pos.y + sin(angle) * length2draw * 0.75f;
    matrix->foreground->fillCircle(hx, hy, max(1, static_cast<int>(rad2draw * 0.55f)),
                                   limb);
    matrix->foreground->drawPixel(hx + cos(angle), hy + sin(angle),
                                  CRGB(10, 10, 20));

    // Four flippers.
    for (int i = 0; i < 4; ++i) {
      const float side = (i % 2 == 0) ? 1.0f : -1.0f;
      const float along = (i < 2) ? 0.35f : -0.35f;
      const float fx =
          pos.x + cos(angle) * length2draw * along +
          cos(angle + PI / 2) * side * (rad2draw + 1.2f * s);
      const float fy =
          pos.y + sin(angle) * length2draw * along +
          sin(angle + PI / 2) * side * (rad2draw + 1.2f * s);
      matrix->foreground->drawLine(pos.x + cos(angle) * length2draw * along,
                                   pos.y + sin(angle) * length2draw * along, fx,
                                   fy, limb);
      matrix->foreground->fillCircle(fx, fy, 1, limb);
    }
  }
};

#endif
