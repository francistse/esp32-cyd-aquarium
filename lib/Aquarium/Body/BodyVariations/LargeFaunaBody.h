#ifndef LARGE_FAUNA_BODY_H
#define LARGE_FAUNA_BODY_H

#include <Arduino.h>
#include <Body/Body.h>
#include <SpeciesCatalog.h>
#include <TankDetail.h>

// Compact side-view silhouettes for sharks, rays, and whales.
class LargeFaunaBody : public Body {
 public:
  SpeciesId speciesId;
  SilhouetteKind silhouette;

  LargeFaunaBody(Matrix* m, Head* head, Tail* tail, Fin* fin, SpeciesId species)
      : Body(m, head, tail, fin),
        speciesId(species),
        silhouette(speciesInfo(species).silhouette) {
    segments.push_back(4);
    segmentPositions.push_back(PVector(0, 0));
    gapBetweenSegments = 1.0f;
    colorPalette = new ColorPalette(1, false);
    const SpeciesInfo& info = speciesInfo(species);
    std::vector<CHSV> palette;
    palette.push_back(CHSV(info.hue, info.sat, info.val));
    colorPalette->setColors(palette);
    type = info.bodyType;
  }

  CRGB bodyColor() const { return colorPalette->colors[0]; }

  void drawShark() {
    const CRGB c = bodyColor();
    const CRGB tip = CRGB(10, 10, 15);
    const float s = max(0.45f, size * sizeClassScale(speciesInfo(speciesId).sizeClass));
    const float len = 7.0f * s;
    const float halfH = 1.6f * s;

    // Fusiform body.
    for (float t = -0.9f; t <= 0.85f; t += 0.18f) {
      const float x = pos.x + cos(angle) * t * len;
      const float y = pos.y + sin(angle) * t * len;
      const float r = halfH * (1.0f - fabsf(t) * 0.55f);
      matrix->foreground->fillCircle(x, y, max(1.0f, r), c);
    }

    // Dorsal fin.
    const float dx = pos.x + cos(angle) * len * 0.1f - sin(angle) * halfH * 2.2f;
    const float dy = pos.y + sin(angle) * len * 0.1f + cos(angle) * halfH * 2.2f;
    matrix->foreground->drawLine(pos.x, pos.y, dx, dy, c);
    if (TankDetail::nearView) {
      matrix->foreground->fillCircle(dx, dy, 1, tip);
    }

    // Caudal crescent.
    const float tx = pos.x - cos(angle) * len;
    const float ty = pos.y - sin(angle) * len;
    const float ux = -sin(angle) * halfH * 2.4f;
    const float uy = cos(angle) * halfH * 2.4f;
    matrix->foreground->drawLine(tx, ty, tx + ux, ty + uy, c);
    matrix->foreground->drawLine(tx, ty, tx - ux * 0.7f, ty - uy * 0.7f, c);

    // Eye.
    matrix->foreground->drawPixel(pos.x + cos(angle) * len * 0.55f,
                                  pos.y + sin(angle) * len * 0.55f,
                                  CRGB(10, 10, 20));
  }

  void drawWhaleShark() {
    const CRGB c = bodyColor();
    const CRGB spot = CRGB(240, 240, 245);
    const float s = max(0.5f, size * 1.35f);
    const float len = 10.0f * s;
    for (float t = -0.95f; t <= 0.9f; t += 0.14f) {
      const float x = pos.x + cos(angle) * t * len;
      const float y = pos.y + sin(angle) * t * len;
      matrix->foreground->fillCircle(x, y, max(1.2f, 2.2f * s * (1.0f - fabsf(t) * 0.4f)),
                                     c);
    }
    if (TankDetail::nearView) {
      for (int i = -2; i <= 2; ++i) {
        const float x = pos.x + cos(angle) * i * 1.6f * s;
        const float y = pos.y + sin(angle) * i * 1.6f * s - cos(angle) * 0.8f * s;
        matrix->foreground->drawPixel(x, y, spot);
      }
    }
    const float tx = pos.x - cos(angle) * len;
    const float ty = pos.y - sin(angle) * len;
    matrix->foreground->fillCircle(tx, ty, max(1.0f, 1.8f * s), c);
  }

  void drawManta() {
    const CRGB c = bodyColor();
    const float s = max(0.5f, size * 1.15f);
    // Disc.
    matrix->foreground->fillCircle(pos.x, pos.y, max(2.0f, 3.2f * s), c);
    // Wings.
    const float wx = -sin(angle) * 5.5f * s;
    const float wy = cos(angle) * 5.5f * s;
    matrix->foreground->fillTriangle(pos.x + wx, pos.y + wy, pos.x - wx,
                                     pos.y - wy, pos.x - cos(angle) * 2.0f * s,
                                     pos.y - sin(angle) * 2.0f * s, c);
    // Cephalic fins / head.
    matrix->foreground->fillCircle(pos.x + cos(angle) * 3.0f * s,
                                   pos.y + sin(angle) * 3.0f * s, max(1.0f, 1.4f * s),
                                   c);
    // Short tail.
    matrix->foreground->drawLine(pos.x, pos.y, pos.x - cos(angle) * 5.0f * s,
                                 pos.y - sin(angle) * 5.0f * s, c);
  }

  void drawWhale() {
    const CRGB c = bodyColor();
    const float s = max(0.55f, size * 1.4f);
    const float len = 11.0f * s;
    for (float t = -0.95f; t <= 0.85f; t += 0.12f) {
      const float x = pos.x + cos(angle) * t * len;
      const float y = pos.y + sin(angle) * t * len;
      matrix->foreground->fillCircle(x, y, max(1.4f, 2.4f * s * (1.0f - fabsf(t) * 0.35f)),
                                     c);
    }
    // Fluke.
    const float tx = pos.x - cos(angle) * len;
    const float ty = pos.y - sin(angle) * len;
    const float ux = -sin(angle) * 3.2f * s;
    const float uy = cos(angle) * 3.2f * s;
    matrix->foreground->drawLine(tx + ux, ty + uy, tx - ux, ty - uy, c);
    // Blowhole hint when near.
    if (TankDetail::nearView) {
      matrix->foreground->drawPixel(pos.x - sin(angle) * 1.5f * s,
                                    pos.y + cos(angle) * 1.5f * s, CRGB(20, 20, 30));
    }
  }

  void display() override {
    switch (silhouette) {
      case SilhouetteKind::WhaleShark:
        drawWhaleShark();
        break;
      case SilhouetteKind::Manta:
        drawManta();
        break;
      case SilhouetteKind::Whale:
        drawWhale();
        break;
      case SilhouetteKind::Shark:
      default:
        drawShark();
        break;
    }
  }
};

#endif  // LARGE_FAUNA_BODY_H
