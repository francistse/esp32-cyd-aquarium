#ifndef FISHBODY_H
#define FISHBODY_H

#include <Arduino.h>
#include <Body/Body.h>
#include <SpeciesCatalog.h>
#include <TankDetail.h>

class FishBody : public Body {
 public:
  SpeciesId speciesId = SpeciesId::Clownfish;
  SilhouetteKind silhouette = SilhouetteKind::Clownfish;

  FishBody(Matrix* m, Head* head, Tail* tail, Fin* fin,
           SpeciesId species = SpeciesId::Clownfish)
      : Body(m, head, tail, fin), speciesId(species) {
    silhouette = speciesInfo(species).silhouette;
    const int numSegments = 6;
    for (int i = 0; i < numSegments; ++i) {
      float phase = (PI * i) / (numSegments - 1);
      uint8_t segmentSize =
          static_cast<uint8_t>(2 + sin(phase) * 2);  // 2..4
      segmentSize = max(segmentSize, static_cast<uint8_t>(1));
      segments.push_back(segmentSize);
      segmentPositions.push_back(PVector(0, 0));
    }
    gapBetweenSegments = 0.78f;
    colorPalette = new ColorPalette(numSegments, false);
    applySpeciesPalette();
    type = speciesInfo(speciesId).bodyType;
  }

  float scale() const {
    return sizeClassScale(speciesInfo(speciesId).sizeClass);
  }

  void applySpeciesPalette() {
    std::vector<CHSV> palette;
    palette.reserve(segments.size());
    const SpeciesInfo& info = speciesInfo(speciesId);
    for (size_t i = 0; i < segments.size(); ++i) {
      palette.push_back(CHSV(info.hue, info.sat, info.val));
    }
    colorPalette->setColors(palette);
  }

  void placeSegments() {
    segmentPositions[0] = pos;
    const float sc = scale();
    for (size_t i = 1; i < segments.size(); ++i) {
      const float step =
          max(segments[i - 1], segments[i]) * size * gapBetweenSegments * sc;
      segmentPositions[i].x = segmentPositions[i - 1].x - cos(angle) * step;
      segmentPositions[i].y = segmentPositions[i - 1].y - sin(angle) * step;
    }
  }

  CRGB bodyColor() const { return colorPalette->colors[0]; }

  void drawEye(float ex, float ey) {
    matrix->foreground->drawPixel(ex, ey, CRGB(10, 10, 20));
    if (TankDetail::nearView) {
      matrix->foreground->drawPixel(ex + cos(angle) * 0.6f,
                                    ey + sin(angle) * 0.6f, CRGB(240, 240, 255));
    }
  }

  void drawForkedTail(const CRGB& c, float sc, float spread, float length) {
    const PVector& tailPos = segmentPositions.back();
    const float bx = tailPos.x - cos(angle) * size * length * sc;
    const float by = tailPos.y - sin(angle) * size * length * sc;
    const float ux = -sin(angle) * size * spread * sc;
    const float uy = cos(angle) * size * spread * sc;
    matrix->foreground->drawLine(tailPos.x, tailPos.y, bx + ux, by + uy, c);
    matrix->foreground->drawLine(tailPos.x, tailPos.y, bx - ux, by - uy, c);
  }

  void drawOvalBody(const CRGB& c, float sc, float lengthScale, float fat) {
    placeSegments();
    for (size_t i = 0; i < segments.size(); ++i) {
      const float t = static_cast<float>(i) / (segments.size() - 1);
      const float r =
          max(1.0f, segments[i] * size * sc * fat * (0.7f + 0.3f * sinf(t * PI)));
      // Stretch spacing already set by placeSegments; thicken mid-body.
      matrix->foreground->fillCircle(segmentPositions[i].x, segmentPositions[i].y,
                                     r, c);
      (void)lengthScale;
    }
  }

  // Slender midwater damsel with a deep V/forked tail.
  void drawChromisShoal() {
    const CRGB c = bodyColor();
    const float sc = scale();
    gapBetweenSegments = 0.9f;
    drawOvalBody(c, sc, 1.1f, 0.7f);
    drawForkedTail(c, sc, 2.4f, 2.8f);
    drawEye(pos.x + cos(angle) * size * 1.2f * sc,
            pos.y + sin(angle) * size * 1.2f * sc);
  }

  // Long torpedo fusilier with crescent fork and a lateral streak.
  void drawFusilier() {
    const CRGB c = bodyColor();
    const CRGB streak = CRGB(255, 230, 80);
    const float sc = scale();
    gapBetweenSegments = 1.05f;
    drawOvalBody(c, sc, 1.35f, 0.65f);
    drawForkedTail(c, sc, 2.0f, 3.2f);
    if (TankDetail::nearView) {
      matrix->foreground->drawLine(
          pos.x + cos(angle) * size * 1.5f * sc,
          pos.y + sin(angle) * size * 1.5f * sc,
          pos.x - cos(angle) * size * 2.5f * sc,
          pos.y - sin(angle) * size * 2.5f * sc, streak);
    }
    drawEye(pos.x + cos(angle) * size * 1.3f * sc,
            pos.y + sin(angle) * size * 1.3f * sc);
  }

  // Tall anthias body with lyre/lunate tail filaments.
  void drawAnthias() {
    const CRGB c = bodyColor();
    const float sc = scale();
    gapBetweenSegments = 0.72f;
    placeSegments();
    for (size_t i = 0; i < segments.size(); ++i) {
      const float r = max(1.0f, segments[i] * size * sc * 1.05f);
      matrix->foreground->fillCircle(segmentPositions[i].x, segmentPositions[i].y,
                                     r, c);
      // Vertical stretch via offset pixels.
      matrix->foreground->drawPixel(
          segmentPositions[i].x - sin(angle) * r * 0.9f,
          segmentPositions[i].y + cos(angle) * r * 0.9f, c);
    }
    drawForkedTail(c, sc, 2.8f, 2.2f);
    // Dorsal filament.
    matrix->foreground->drawLine(
        pos.x, pos.y, pos.x - sin(angle) * size * 3.2f * sc,
        pos.y + cos(angle) * size * 3.2f * sc, c);
    drawEye(pos.x + cos(angle) * size * 1.0f * sc,
            pos.y + sin(angle) * size * 1.0f * sc);
  }

  // Compact cardinal with oversized eye.
  void drawCardinal() {
    const CRGB c = bodyColor();
    const float sc = scale();
    gapBetweenSegments = 0.55f;
    drawOvalBody(c, sc, 0.75f, 1.15f);
    const PVector& tailPos = segmentPositions.back();
    matrix->foreground->fillTriangle(
        tailPos.x - cos(angle) * size * 2.0f * sc,
        tailPos.y - sin(angle) * size * 2.0f * sc,
        tailPos.x - sin(angle) * size * 1.4f * sc,
        tailPos.y + cos(angle) * size * 1.4f * sc,
        tailPos.x + sin(angle) * size * 1.4f * sc,
        tailPos.y - cos(angle) * size * 1.4f * sc, c);
    const float ex = pos.x + cos(angle) * size * 0.9f * sc;
    const float ey = pos.y + sin(angle) * size * 0.9f * sc;
    matrix->foreground->fillCircle(ex, ey, max(1.0f, 1.3f * sc), CRGB(10, 10, 20));
    if (TankDetail::nearView) {
      matrix->foreground->drawPixel(ex + cos(angle) * 0.5f,
                                    ey + sin(angle) * 0.5f, CRGB(240, 240, 255));
    }
  }

  // Damsel/tang-like oval with vertical bars.
  void drawSergeant() {
    const CRGB c = bodyColor();
    const CRGB bar = CRGB(20, 20, 30);
    const float sc = scale();
    gapBetweenSegments = 0.7f;
    drawOvalBody(c, sc, 0.95f, 1.0f);
    drawForkedTail(c, sc, 1.8f, 2.0f);
    for (int b = 1; b <= 3; ++b) {
      const size_t idx = min(segments.size() - 1, static_cast<size_t>(b + 1));
      matrix->foreground->drawLine(
          segmentPositions[idx].x - sin(angle) * size * 1.6f * sc,
          segmentPositions[idx].y + cos(angle) * size * 1.6f * sc,
          segmentPositions[idx].x + sin(angle) * size * 1.6f * sc,
          segmentPositions[idx].y - cos(angle) * size * 1.6f * sc, bar);
    }
    drawEye(pos.x + cos(angle) * size * 1.1f * sc,
            pos.y + sin(angle) * size * 1.1f * sc);
  }

  // Cave sweeper: short deep body, soft wedge tail.
  void drawSweeper() {
    const CRGB c = bodyColor();
    const float sc = scale();
    gapBetweenSegments = 0.6f;
    drawOvalBody(c, sc, 0.8f, 1.2f);
    const PVector& tailPos = segmentPositions.back();
    matrix->foreground->fillTriangle(
        tailPos.x, tailPos.y, tailPos.x - cos(angle) * size * 2.2f * sc,
        tailPos.y - sin(angle) * size * 2.2f * sc,
        tailPos.x - sin(angle) * size * 1.2f * sc,
        tailPos.y + cos(angle) * size * 1.2f * sc, c);
    drawEye(pos.x + cos(angle) * size * 1.0f * sc,
            pos.y + sin(angle) * size * 1.0f * sc);
  }

  // True schooler / sprat: needle thin.
  void drawSilverside() {
    const CRGB c = bodyColor();
    const float sc = scale();
    gapBetweenSegments = 1.15f;
    drawOvalBody(c, sc, 1.4f, 0.45f);
    drawForkedTail(c, sc, 1.4f, 2.6f);
    drawEye(pos.x + cos(angle) * size * 1.4f * sc,
            pos.y + sin(angle) * size * 1.4f * sc);
  }

  // Hovering dartfish: very thin with a bright tail tip.
  void drawDartfish() {
    const CRGB c = bodyColor();
    const CRGB tip = CRGB(245, 245, 250);
    const float sc = scale();
    gapBetweenSegments = 1.2f;
    drawOvalBody(c, sc, 1.5f, 0.4f);
    const PVector& tailPos = segmentPositions.back();
    matrix->foreground->fillCircle(tailPos.x - cos(angle) * size * 1.5f * sc,
                                   tailPos.y - sin(angle) * size * 1.5f * sc,
                                   max(1.0f, size * sc), tip);
    drawEye(pos.x + cos(angle) * size * 1.5f * sc,
            pos.y + sin(angle) * size * 1.5f * sc);
  }

  // Surgeon disc with caudal spine hint.
  void drawSurgeon() {
    const CRGB c = bodyColor();
    const CRGB spine = CRGB(220, 220, 230);
    const float sc = scale();
    const float rad = max(2.0f, 3.2f * size * sc);
    matrix->foreground->fillCircle(pos.x, pos.y, rad, c);
    PVector tail = PVector::fromAngle(angle + PI);
    tail *= rad * 1.35f;
    tail += pos;
    PVector t2 = PVector::fromAngle(angle + PI / 2);
    t2 *= rad * 0.85f;
    t2 += pos;
    PVector t3 = PVector::fromAngle(angle - PI / 2);
    t3 *= rad * 0.85f;
    t3 += pos;
    matrix->foreground->fillTriangle(tail.x, tail.y, t2.x, t2.y, t3.x, t3.y, c);
    matrix->foreground->drawPixel(pos.x - cos(angle) * rad * 0.7f,
                                  pos.y - sin(angle) * rad * 0.7f, spine);
    drawEye(pos.x + cos(angle) * rad * 0.5f, pos.y + sin(angle) * rad * 0.5f);
  }

  // Unicornfish: elongate oval with forehead horn bump.
  void drawUnicorn() {
    const CRGB c = bodyColor();
    const float sc = scale();
    gapBetweenSegments = 0.85f;
    drawOvalBody(c, sc, 1.15f, 0.95f);
    drawForkedTail(c, sc, 1.6f, 2.4f);
    matrix->foreground->fillCircle(pos.x + cos(angle) * size * 2.0f * sc,
                                   pos.y + sin(angle) * size * 2.0f * sc,
                                   max(1.0f, size * 0.9f * sc), c);
    drawEye(pos.x + cos(angle) * size * 1.2f * sc,
            pos.y + sin(angle) * size * 1.2f * sc);
  }

  void drawSchooler() {
    drawSilverside();
  }

  void drawClownfish() {
    const CRGB orange = bodyColor();
    const CRGB white = CRGB(245, 245, 250);
    const CRGB black = CRGB(15, 15, 20);
    const float sc = scale();
    placeSegments();

    for (size_t i = 0; i < segments.size(); ++i) {
      const float r = max(1.0f, segments[i] * size * sc);
      matrix->foreground->fillCircle(segmentPositions[i].x, segmentPositions[i].y,
                                     r, orange);
    }

    for (int band = 1; band <= 3; ++band) {
      const size_t idx = min(segments.size() - 1, static_cast<size_t>(band));
      const float r = max(1.0f, segments[idx] * size * sc * 0.85f);
      matrix->foreground->fillCircle(segmentPositions[idx].x,
                                     segmentPositions[idx].y, r, white);
      if (TankDetail::nearView) {
        matrix->foreground->drawPixel(segmentPositions[idx].x + cos(angle),
                                      segmentPositions[idx].y + sin(angle),
                                      black);
        matrix->foreground->drawPixel(segmentPositions[idx].x - cos(angle),
                                      segmentPositions[idx].y - sin(angle),
                                      black);
      }
    }

    const PVector& tailPos = segmentPositions.back();
    PVector t1 = PVector::fromAngle(angle + PI);
    t1 *= size * 3 * sc;
    t1 += tailPos;
    PVector t2 = PVector::fromAngle(angle + PI / 2);
    t2 *= size * 2 * sc;
    t2 += tailPos;
    PVector t3 = PVector::fromAngle(angle - PI / 2);
    t3 *= size * 2 * sc;
    t3 += tailPos;
    matrix->foreground->fillTriangle(t1.x, t1.y, t2.x, t2.y, t3.x, t3.y, orange);

    drawEye(pos.x + cos(angle) * size * 1.2f * sc,
            pos.y + sin(angle) * size * 1.2f * sc);
  }

  void drawBlueTang() {
    const CRGB blue = bodyColor();
    const CRGB yellow = CRGB(255, 220, 40);
    const CRGB black = CRGB(10, 10, 20);
    const float sc = scale();
    const float rad = max(2.0f, 3.5f * size * sc);
    matrix->foreground->fillCircle(pos.x, pos.y, rad, blue);

    PVector tail = PVector::fromAngle(angle + PI);
    tail *= rad * 1.4f;
    tail += pos;
    PVector t2 = PVector::fromAngle(angle + PI / 2);
    t2 *= rad * 0.9f;
    t2 += pos;
    PVector t3 = PVector::fromAngle(angle - PI / 2);
    t3 *= rad * 0.9f;
    t3 += pos;
    matrix->foreground->fillTriangle(tail.x, tail.y, t2.x, t2.y, t3.x, t3.y,
                                     yellow);

    if (TankDetail::nearView) {
      const float mx = pos.x + cos(angle + PI / 2) * rad * 0.15f;
      const float my = pos.y + sin(angle + PI / 2) * rad * 0.15f;
      matrix->foreground->fillCircle(mx, my, max(1.0f, rad * 0.25f), black);
      const float dx = pos.x + cos(angle - PI / 2) * rad * 0.85f;
      const float dy = pos.y + sin(angle - PI / 2) * rad * 0.85f;
      matrix->foreground->drawPixel(dx, dy, yellow);
    }

    drawEye(pos.x + cos(angle) * rad * 0.55f, pos.y + sin(angle) * rad * 0.55f);
  }

  void drawYellowTang() {
    const CRGB yellow = bodyColor();
    const CRGB pale = CRGB(255, 245, 180);
    const float sc = scale();
    const float rad = max(2.0f, 3.2f * size * sc);
    matrix->foreground->fillCircle(pos.x, pos.y, rad, yellow);

    PVector nose = PVector::fromAngle(angle);
    nose *= rad * 1.35f;
    nose += pos;
    PVector n2 = PVector::fromAngle(angle + 0.7f);
    n2 *= rad * 0.55f;
    n2 += pos;
    PVector n3 = PVector::fromAngle(angle - 0.7f);
    n3 *= rad * 0.55f;
    n3 += pos;
    matrix->foreground->fillTriangle(nose.x, nose.y, n2.x, n2.y, n3.x, n3.y,
                                     yellow);

    PVector tail = PVector::fromAngle(angle + PI);
    tail *= rad * 1.3f;
    tail += pos;
    PVector t2 = PVector::fromAngle(angle + PI / 2);
    t2 *= rad * 0.8f;
    t2 += pos;
    PVector t3 = PVector::fromAngle(angle - PI / 2);
    t3 *= rad * 0.8f;
    t3 += pos;
    matrix->foreground->fillTriangle(tail.x, tail.y, t2.x, t2.y, t3.x, t3.y,
                                     yellow);

    if (TankDetail::nearView) {
      matrix->foreground->drawLine(tail.x, tail.y, pos.x, pos.y, pale);
    }

    drawEye(pos.x + cos(angle) * rad * 0.4f, pos.y + sin(angle) * rad * 0.4f);
  }

  void drawMoorishIdol() {
    const CRGB black = CRGB(15, 15, 25);
    const CRGB white = CRGB(245, 245, 250);
    const CRGB yellow = CRGB(255, 210, 40);
    const float sc = scale();
    const float rad = max(2.0f, 2.8f * size * sc);
    matrix->foreground->drawCircleArray(pos.x, pos.y, rad * 0.55f, rad * 1.35f,
                                        angle + PI / 2, white);

    PVector b1 = PVector::fromAngle(angle);
    b1 *= rad * 0.35f;
    matrix->foreground->fillCircle(pos.x + b1.x, pos.y + b1.y, rad * 0.55f,
                                   yellow);
    PVector b2 = PVector::fromAngle(angle + PI);
    b2 *= rad * 0.2f;
    matrix->foreground->fillCircle(pos.x + b2.x, pos.y + b2.y, rad * 0.7f, black);

    PVector stream = PVector::fromAngle(angle - PI / 2);
    stream *= rad * (TankDetail::nearView ? 2.8f : 2.0f);
    stream += pos;
    matrix->foreground->drawLine(pos.x, pos.y, stream.x, stream.y, white);
    if (TankDetail::nearView) {
      matrix->foreground->fillCircle(stream.x, stream.y, 1, white);
    }

    drawEye(pos.x + cos(angle) * rad * 0.5f, pos.y + sin(angle) * rad * 0.5f);
  }

  void display() override {
    switch (silhouette) {
      case SilhouetteKind::BlueTang:
        drawBlueTang();
        break;
      case SilhouetteKind::YellowTang:
        drawYellowTang();
        break;
      case SilhouetteKind::MoorishIdol:
        drawMoorishIdol();
        break;
      case SilhouetteKind::Clownfish:
        drawClownfish();
        break;
      case SilhouetteKind::ChromisShoal:
        drawChromisShoal();
        break;
      case SilhouetteKind::Fusilier:
        drawFusilier();
        break;
      case SilhouetteKind::Anthias:
        drawAnthias();
        break;
      case SilhouetteKind::Cardinal:
        drawCardinal();
        break;
      case SilhouetteKind::Sergeant:
        drawSergeant();
        break;
      case SilhouetteKind::Sweeper:
        drawSweeper();
        break;
      case SilhouetteKind::Silverside:
        drawSilverside();
        break;
      case SilhouetteKind::Dartfish:
        drawDartfish();
        break;
      case SilhouetteKind::Surgeon:
        drawSurgeon();
        break;
      case SilhouetteKind::Unicorn:
        drawUnicorn();
        break;
      case SilhouetteKind::Schooler:
      default:
        drawSchooler();
        break;
    }
  }
};

#endif
