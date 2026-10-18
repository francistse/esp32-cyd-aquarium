#ifndef FISH_H
#define FISH_H

#include <Arduino.h>
#include <Matrix.h>
#include <PVector.h>

#include <chrono>
#include <random>
#include <utility>
#include <vector>

#include "AquariumSettings.h"
#include "Body/BodyVariations/BodyFactory.h"
#include "Food.h"
#include "Motion/MotionFactory.h"
#include "SpeciesCatalog.h"

class Fish {
  Matrix* matrix = nullptr;
  PVector pos;
  float age;
  float health;
  uint8_t offspringCount = 0;
  SpeciesId speciesId = SpeciesId::Clownfish;

  std::unique_ptr<Motion> motion;
  std::unique_ptr<Body> body;

  Food* food = nullptr;
  float plumpScale = 1.0f;
  unsigned long plumpUntilMs = 0;

  static PVector randomSafePosition(Matrix* matrix) {
    const uint8_t width = matrix->getXResolution();
    const uint8_t height = matrix->getYResolution();

    uint8_t marginX = width > 24 ? 8 : 1;
    uint8_t marginTop = height > 48 ? 16 : 1;
    uint8_t marginBottom = height > 48 ? 16 : 1;

    if (marginTop + marginBottom >= height) {
      marginTop = 1;
      marginBottom = 1;
    }
    if (marginX * 2 >= width) {
      marginX = 1;
    }

    const uint8_t minX = marginX;
    const uint8_t maxX = width > marginX ? width - marginX : width;
    const uint8_t minY = marginTop;
    const uint8_t maxY = height > marginBottom ? height - marginBottom : height;

    return PVector(random(minX, maxX), random(minY, maxY));
  }

  void bindBodyAndMotion(SpeciesId species) {
    speciesId = species;
    const SpeciesInfo& info = speciesInfo(species);
    BodyFactory bodyFactory(matrix);
    this->body = std::unique_ptr<Body>(bodyFactory.createBody(species));
    if (!this->body) {
      speciesId = SpeciesId::Clownfish;
      this->body = std::unique_ptr<Body>(
          bodyFactory.createBody(SpeciesId::Clownfish));
    }

    const char* motionKey = info.motionType;
    if (!info.isSwimmer || isFishMotionSpecies(speciesId)) {
      // Non-swimmers should not use this path; fish species share Fish motion.
      if (isFishMotionSpecies(speciesId)) {
        motionKey = "Fish";
      }
    }

    this->motion = MotionFactory::createMotion(
        motionKey, this->pos * PHYSICS_SCALE,
        matrix->getXResolution() * PHYSICS_SCALE,
        matrix->getYResolution() * PHYSICS_SCALE);
    if (!this->motion) {
      this->motion = MotionFactory::createMotion(
          "Fish", this->pos * PHYSICS_SCALE,
          matrix->getXResolution() * PHYSICS_SCALE,
          matrix->getYResolution() * PHYSICS_SCALE);
    }

    fishDefinition = {age,
                      health,
                      String(speciesInfo(speciesId).bodyType),
                      this->body->getHeadType(),
                      this->body->getTailType(),
                      this->body->getFinType(),
                      String(speciesInfo(speciesId).motionType),
                      this->body->getColorPaletteHSV()};
    // Fish motion key for the four fish species.
    if (isFishMotionSpecies(speciesId)) {
      fishDefinition.motionType = "Fish";
    }
  }

 public:
  struct FishDefinition {
    float age;
    float health;
    String bodyType;
    String headType;
    String tailType;
    String finType;
    String motionType;
    std::vector<CHSV> colors;
    int16_t speciesId = -1;  // >=0 prefers numeric id over bodyType
  };

  FishDefinition fishDefinition;
  const std::vector<CHSV>& getColorsHSV() const {
    return body->getColorPaletteHSV();
  }

  Fish(Matrix* matrix, const FishDefinition& fishDef)
      : matrix(matrix),
        pos(PVector(0, 0)),
        age(fishDef.age),
        health(fishDef.health) {
    initializeAgingRate();
    pos = randomSafePosition(matrix);
    SpeciesId loaded = SpeciesId::Clownfish;
    if (fishDef.speciesId >= 0 &&
        fishDef.speciesId < static_cast<int16_t>(SPECIES_COUNT) &&
        isSwimmerSpecies(static_cast<SpeciesId>(fishDef.speciesId))) {
      loaded = static_cast<SpeciesId>(fishDef.speciesId);
    } else {
      loaded = speciesIdFromBodyType(fishDef.bodyType);
      if (!isSwimmerSpecies(loaded)) {
        loaded = SpeciesId::Clownfish;
      }
    }
    bindBodyAndMotion(loaded);
    // Catalog palettes win; ignore legacy random hues.
    fishDefinition.age = age;
    fishDefinition.health = health;
  }

  Fish(Matrix* matrix, PVector pos = PVector(0, 0), float age = 0,
       float health = 1)
      : matrix(matrix), pos(pos), age(age), health(health) {
    initializeAgingRate();
    if (this->pos.x == 0 && this->pos.y == 0) {
      this->pos = randomSafePosition(matrix);
    }
    bindBodyAndMotion(randomSwimmerSpecies());
  }

  Fish(Matrix* matrix, PVector pos, float age, const char* bodyType,
       float health = 1)
      : matrix(matrix), pos(pos), age(age), health(health) {
    initializeAgingRate();
    if (this->pos.x == 0 && this->pos.y == 0) {
      this->pos = randomSafePosition(matrix);
    }
    bindBodyAndMotion(speciesIdFromBodyType(String(bodyType)));
  }

  Fish(Matrix* matrix, PVector pos, float age, SpeciesId species,
       float health = 1)
      : matrix(matrix), pos(pos), age(age), health(health) {
    initializeAgingRate();
    if (this->pos.x == 0 && this->pos.y == 0) {
      this->pos = randomSafePosition(matrix);
    }
    bindBodyAndMotion(species);
  }

  bool update(long co2 = 600, bool stayInside = false) {
    if (!motion) {
      log_e("Error: motion is null in Fish::update for fish type: %s",
            fishDefinition.bodyType.c_str());
      this->motion = MotionFactory::createMotion(
          fishDefinition.motionType, pos * PHYSICS_SCALE,
          matrix->getXResolution() * PHYSICS_SCALE,
          matrix->getYResolution() * PHYSICS_SCALE);
      if (!this->motion) {
        return false;
      }
    }
    if (plumpUntilMs != 0 && millis() >= plumpUntilMs) {
      plumpScale = 1.0f;
      plumpUntilMs = 0;
    }
    motion->update(age, co2, stayInside);
    pos = motion->getPosition() / PHYSICS_SCALE;
    updateAge(co2);
    updateHealth(co2);
    const float displayAge = constrain(age * plumpScale, 0.0f, 0.98f);
    body->update(pos, motion->getVelocity(), motion->getAngle(), displayAge,
                 health);
    if (food != nullptr) {
      PVector foodDistance = food->getPosition() - pos;
      if (food->isOffScreen()) {
        food = nullptr;
      } else if (foodDistance.mag() < 1) {
        food->eat();
        food = nullptr;
      } else {
        motion->followFood(food->getPosition() * PHYSICS_SCALE);
      }
    }
    if (age > 1.0) age = 0;
    return age > 1.0;
  }

  bool tryReproduce() {
    if (age > 0.5 && age < 0.9 && offspringCount < 2) {
      auto seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::mt19937 gen(seed);
      std::uniform_real_distribution<> dis(0.0, 1.0);
      if (dis(gen) < 0.01) {
        offspringCount++;
        return true;
      }
    }
    return false;
  }

  PVector getPosition() const { return PVector(pos.x, pos.y); }

  PVector getVelocity() const { return motion->getVelocity(); }

  float getAngle() const { return motion ? motion->getAngle() : 0.0f; }

  // Outside distance to an age/species-scaled oriented capsule (0 = solid hit).
  float tapHitDistance(float worldX, float worldY) const {
    const float size = constrain(age, 0.0f, 0.98f);
    const float sc = sizeClassScale(speciesInfo(speciesId).sizeClass);
    float forward = 2.0f * size * sc;
    float back = 10.0f * size * sc;
    float radius = 2.5f * size * sc + 1.5f;

    switch (speciesInfo(speciesId).silhouette) {
      case SilhouetteKind::BlueTang:
      case SilhouetteKind::YellowTang:
      case SilhouetteKind::MoorishIdol:
        forward = 2.5f * size * sc;
        back = 5.5f * size * sc;
        radius = 3.5f * size * sc + 1.5f;
        break;
      case SilhouetteKind::Turtle:
        forward = 5.0f * size * sc;
        back = 5.0f * size * sc;
        radius = 3.5f * size * sc + 1.5f;
        break;
      case SilhouetteKind::Star:
        forward = 5.0f * size * sc;
        back = 5.0f * size * sc;
        radius = 2.5f * size * sc + 1.5f;
        break;
      case SilhouetteKind::Snake:
        forward = 2.0f * size * sc;
        back = 22.0f * size * sc;
        radius = 2.5f * size * sc + 1.5f;
        break;
      case SilhouetteKind::Octopus:
        forward = 4.0f * size * sc;
        back = 10.0f * size * sc;
        radius = 4.0f * size * sc + 2.0f;
        break;
      case SilhouetteKind::Shark:
      case SilhouetteKind::WhaleShark:
      case SilhouetteKind::Whale:
        forward = 4.0f * size * sc;
        back = 12.0f * size * sc;
        radius = 3.0f * size * sc + 1.5f;
        break;
      case SilhouetteKind::Manta:
        forward = 4.0f * size * sc;
        back = 6.0f * size * sc;
        radius = 5.0f * size * sc + 1.5f;
        break;
      case SilhouetteKind::Clownfish:
      case SilhouetteKind::Schooler:
      default:
        break;
    }

    const float angle = getAngle();
    const float cosA = cosf(angle);
    const float sinA = sinf(angle);
    const float dx = worldX - pos.x;
    const float dy = worldY - pos.y;
    const float along = dx * cosA + dy * sinA;
    const float across = -dx * sinA + dy * cosA;
    const float clampedAlong = constrain(along, -back, forward);
    const float dAlong = along - clampedAlong;
    const float distToCenterline = sqrtf(dAlong * dAlong + across * across);
    return max(0.0f, distToCenterline - radius);
  }

  void display() {
    if (age < AGE_EGG)
      body->displayEgg();
    else
      body->display();
  }

  float getAge() { return age; }

  void setFood(Food* assignedFood) { food = assignedFood; }

  Food* getFood() { return food; }

  float getHealth() const { return health; }
  String getBodyType() const { return fishDefinition.bodyType; }
  String getHeadType() const { return fishDefinition.headType; }
  String getTailType() const { return fishDefinition.tailType; }
  String getFinType() const { return fishDefinition.finType; }
  String getMotionType() const { return fishDefinition.motionType; }
  SpeciesId getSpeciesId() const { return speciesId; }

  void poke() {
    if (motion) {
      motion->startWiggle(400);
    }
  }

  void scareFrom(PVector source) {
    if (motion) {
      motion->scareFrom(source * PHYSICS_SCALE);
    }
  }

  void schoolToward(PVector target) {
    if (motion) {
      motion->schoolToward(target * PHYSICS_SCALE);
    }
  }

  void setPlump(float scale, unsigned long durationMs) {
    plumpScale = scale;
    plumpUntilMs = millis() + durationMs;
  }

  bool isPokeable() const { return age > AGE_EGG && age < AGE_DEAD; }

 private:
  unsigned long lastUpdateTime = millis();
  float agingRate;

  void initializeAgingRate() {
    float baseRate = 1.0f / (FISH_LIFESPAN_DAYS * 24 * 60 * 60 * 1000);
    float variation =
        FISH_LIFESPAN_VARIATION * ((float)random(200) / 100.0f - 1.0f);
    agingRate = baseRate * (1.0f + variation);
  }

  void updateAge(long co2) {
    unsigned long currentTime = millis();
    float timeDiff = currentTime - lastUpdateTime;
    if (co2 < 2000) {
      age += timeDiff * agingRate;
    }
    lastUpdateTime = currentTime;
  }

  void updateHealth(long co2) {
    if (co2 >= CO2_REALBAD) {
      health -= HEALTH_REDUCTION_RATE_REALBAD;
    } else if (co2 >= CO2_BAD) {
      health -= HEALTH_REDUCTION_RATE_BAD;
    } else if (co2 < CO2_BAD) {
      health += HEALTH_INCREASE_RATE_GOOD;
    }
    health = max(0.0f, min(1.0f, health));
  }
};

#endif  // FISH_H
