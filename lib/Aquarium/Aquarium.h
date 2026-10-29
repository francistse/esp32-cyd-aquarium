#ifndef AQUARIUM_H
#define AQUARIUM_H

#include <Arduino.h>
#include <Fonts/Font4x7Fixed.h>
#include <Fonts/Font5x7Fixed.h>
#include <Matrix.h>
#include <SCD40.h>
#include <time.h>

#include <cstring>
#include <limits>
#include <vector>

#include "AquariumSettings.h"
#include "AquariumStateManager.h"
#include "BoidManager.h"
#include "FieldGuide.h"
#include "Fish.h"
#include "Food.h"
#include "Plants.h"
#include "SpeciesCatalog.h"
#include "TankCamera.h"
#include "TankDetail.h"
#include "Water.h"
#include "StateManager.h"

#ifndef CYD_AUTONOMOUS_LIFE
#define CYD_AUTONOMOUS_LIFE 0
#endif

#ifndef CYD_CURATED_BOOT_POPULATION
#define CYD_CURATED_BOOT_POPULATION 0
#endif

#ifndef CYD_KEEP_CREATURES_ON_SCREEN
#define CYD_KEEP_CREATURES_ON_SCREEN 0
#endif

#ifndef CYD_AUTONOMOUS_BOOT_MIN_MS
#define CYD_AUTONOMOUS_BOOT_MIN_MS 3000
#endif

#ifndef CYD_AUTONOMOUS_BOOT_MAX_MS
#define CYD_AUTONOMOUS_BOOT_MAX_MS 6000
#endif

#ifndef CYD_AUTONOMOUS_FOOD_MIN_MS
#define CYD_AUTONOMOUS_FOOD_MIN_MS 7000
#endif

#ifndef CYD_AUTONOMOUS_FOOD_MAX_MS
#define CYD_AUTONOMOUS_FOOD_MAX_MS 16000
#endif

#ifndef CYD_AUTONOMOUS_MAX_FOOD
#define CYD_AUTONOMOUS_MAX_FOOD 3
#endif

#ifndef CYD_FIXED_HUMIDITY_PERCENT
#define CYD_FIXED_HUMIDITY_PERCENT 50
#endif

class Aquarium {
 private:
  Matrix* matrix;
  SCD40* scd40;
  StateManager* stateManager;
  Water water;
  std::vector<std::unique_ptr<Fish>> fishArray;
  std::vector<std::unique_ptr<Plants>> plantArray;
  std::vector<std::unique_ptr<Food>> foodArray;
  BoidManager boidManager;
  AquariumStateManager aquariumStateManager;
  FieldGuide fieldGuide;
  TankCamera tankCamera;
  bool chromisDiscovered = false;
  bool flakeDiscovered = false;
  unsigned long lastSaveTime;
  char buffer[100];

  // Demo settings
  bool demoMode;
  int demoStep;
  unsigned long demoStartTime;
  float demoTemperature;
  float demoHumidity;
  float demoCO2;
  bool demoFinished;

  // Touch input  
  bool touchActive = false;
  unsigned long lastFoodTime = 0;
  const unsigned long FOOD_INTERVAL = 200; // Add food every 200ms while touched
  unsigned long lastAutonomousFoodTime = 0;
  unsigned long nextAutonomousFoodDelay = 0;

  // Desk events
  unsigned long bootMs = 0;
  uint32_t pelletsEaten = 0;
  char pokeBubbleText[20] = {};
  int16_t pokeBubbleX = 0;
  int16_t pokeBubbleY = 0;
  unsigned long pokeBubbleUntilMs = 0;
  uint8_t pokePhraseIndex = 0;
  static constexpr uint8_t TOUCH_PELLET_HISTORY = 12;
  unsigned long touchPelletTimes[TOUCH_PELLET_HISTORY] = {};
  Fish* touchPelletFish[TOUCH_PELLET_HISTORY] = {};
  uint8_t touchPelletHead = 0;
  uint8_t touchPelletStored = 0;
  unsigned long murkUntilMs = 0;
  unsigned long burpUntilMs = 0;
  int16_t burpX = 0;
  int16_t burpBaseY = 0;
  unsigned long diaryUntilMs = 0;
  char diaryText[32] = {};
  bool weekendMode = false;
  bool tgifActive = false;

  enum class VisitorKind : uint8_t { None, CatPaw, Duck, Diver, Shadow };
  VisitorKind visitorKind = VisitorKind::None;
  unsigned long visitorStartMs = 0;
  unsigned long visitorEndMs = 0;
  unsigned long nextVisitorMs = 0;
  unsigned long nextSpeciesRefillMs = 0;
  float visitorX = 0;
  float visitorY = 0;
  float visitorSpeed = 0;

  enum class TextAlignment { LEFT, CENTER, RIGHT };

  float initialCreatureAge(uint8_t index = 0) {
#if CYD_CURATED_BOOT_POPULATION
    if (index == 0) {
      return random(74, 84) / 100.0f;
    }
    if (index == 1) {
      return random(66, 78) / 100.0f;
    }
    return random(52, 69) / 100.0f;
#else
    (void)index;
    return 0.5f;
#endif
  }

  void markSpeciesOnScreen(bool* used) const {
    for (uint8_t i = 0; i < SPECIES_COUNT; ++i) {
      used[i] = false;
    }
    for (const auto& fish : fishArray) {
      const uint8_t id = static_cast<uint8_t>(fish->getSpeciesId());
      if (id < SPECIES_COUNT) {
        used[id] = true;
      }
    }
  }

  SpeciesId nextRosterSpecies() {
    bool used[SPECIES_COUNT];
    markSpeciesOnScreen(used);
    return pickWeightedSwimmer(used);
  }

  void scheduleNextSpeciesRefill() {
    nextSpeciesRefillMs =
        millis() +
        randomDelayBetween(SPECIES_REFILL_MIN_MS, SPECIES_REFILL_MAX_MS);
  }

  void spawnFishAt(uint8_t index, SpeciesId species) {
    fishArray.emplace_back(std::make_unique<Fish>(
        matrix, safeSpawnPosition(index), initialCreatureAge(index), species));
  }

  PVector safeSpawnPosition(uint8_t index) {
    const uint8_t width = matrix->getXResolution();
    const uint8_t height = matrix->getYResolution();
    const uint8_t cols = 3;
    const uint8_t rows = 3;
    const uint8_t cell = index % (cols * rows);
    const uint8_t col = cell % cols;
    const uint8_t row = cell / cols;

    float marginX = width > 24 ? 8.0f : 1.0f;
    float marginTop = height > 48 ? 18.0f : 1.0f;
    float marginBottom = height > 48 ? 18.0f : 1.0f;
    if (marginTop + marginBottom >= height) {
      marginTop = 1.0f;
      marginBottom = 1.0f;
    }
    if (marginX * 2.0f >= width) {
      marginX = 1.0f;
    }

    float usableW = width - marginX * 2.0f;
    float usableH = height - marginTop - marginBottom;
    if (usableW < 1.0f) {
      usableW = 1.0f;
    }
    if (usableH < 1.0f) {
      usableH = 1.0f;
    }

    const float jitterX = random(20, 80) / 100.0f;
    const float jitterY = random(18, 82) / 100.0f;
    const float x = marginX + (col + jitterX) * (usableW / cols);
    const float y = marginTop + (row + jitterY) * (usableH / rows);
    return PVector(x, y);
  }

  // Finger slack outside the drawn capsule; ambiguity rejects near-ties.
  static constexpr float kHitSlack = 2.0f;
  static constexpr float kHitAmbiguityMargin = 2.5f;

  Fish* nearestCreatureAt(float worldX, float worldY, int8_t* outIndex) {
    Fish* nearest = nullptr;
    int8_t nearestIndex = -1;
    float bestDist = 999.0f;
    float secondBestDist = 999.0f;
    int8_t secondIndex = -1;
    for (size_t i = 0; i < fishArray.size(); ++i) {
      if (!fishArray[i]->isPokeable()) {
        continue;
      }
      const float dist = fishArray[i]->tapHitDistance(worldX, worldY);
      if (dist > kHitSlack) {
        continue;
      }
      if (dist < bestDist) {
        secondBestDist = bestDist < 900.0f ? bestDist : secondBestDist;
        secondIndex = nearestIndex;
        bestDist = dist;
        nearest = fishArray[i].get();
        nearestIndex = static_cast<int8_t>(i);
      } else if (dist < secondBestDist) {
        secondBestDist = dist;
        secondIndex = static_cast<int8_t>(i);
      }
    }
    if (nearest != nullptr && secondIndex >= 0 &&
        (secondBestDist - bestDist) < kHitAmbiguityMargin) {
      nearest = nullptr;
      nearestIndex = -1;
    }
    if (outIndex != nullptr) {
      *outIndex = nearestIndex;
    }
    return nearest;
  }

 public:
  Aquarium(Matrix* m, SCD40* s, StateManager* stateManager)
      : matrix(m),
        scd40(s),
        stateManager(stateManager),
        water(matrix),
        boidManager(m),
        demoMode(false),
        demoStep(0),
        demoFinished(false),
        lastSaveTime(0) {}

  void begin() {
    bootMs = millis();
    tankCamera.configure(matrix->getXResolution(), matrix->getYResolution(),
                         0, 1, 240, 318);
    fieldGuide.begin();
#if CYD_CURATED_BOOT_POPULATION
    initializeFish();
#else
    loadState();
    ensureMinimumFishPopulation();
#endif
    scheduleNextSpeciesRefill();
    initializePlants();
    boidManager.initializeBoids();
    scheduleNextAutonomousFood(true);
    scheduleNextVisitor(true);
  }

  TankCamera& camera() { return tankCamera; }
  const TankCamera& camera() const { return tankCamera; }
  FieldGuide& guide() { return fieldGuide; }
  const FieldGuide& guide() const { return fieldGuide; }

  void announceDiscovery(SpeciesId id) {
    const SpeciesInfo& info = speciesInfo(id);
    snprintf(pokeBubbleText, sizeof(pokeBubbleText), "NEW %s", info.shortName);
    pokeBubbleX = matrix->getXResolution() / 2 -
                  static_cast<int16_t>(strlen(pokeBubbleText) * 2);
    if (pokeBubbleX < 1) {
      pokeBubbleX = 1;
    }
    pokeBubbleY = 16;
    pokeBubbleUntilMs = millis() + 2200;
  }

  bool unlockSpecies(SpeciesId id) {
    if (fieldGuide.discover(id)) {
      announceDiscovery(id);
      return true;
    }
    return false;
  }

  // Tap-to-follow zoom. Returns true if a creature was locked.
  bool zoomToCreatureAt(float worldX, float worldY) {
    int8_t nearestIndex = -1;
    Fish* nearest = nearestCreatureAt(worldX, worldY, &nearestIndex);
    if (nearest == nullptr) {
      tankCamera.zoomOut();
      return false;
    }
    const PVector pos = nearest->getPosition();
    tankCamera.follow(pos.x, pos.y, nearestIndex, nearest->getSpeciesId());
    unlockSpecies(nearest->getSpeciesId());
    return true;
  }

  void zoomOutCamera() { tankCamera.zoomOut(); }

  bool isCameraNear() const { return tankCamera.isNear(); }

  size_t fishCount() const { return fishArray.size(); }

  Fish* fishAt(size_t index) {
    return index < fishArray.size() ? fishArray[index].get() : nullptr;
  }

  bool isDemoFinished() const {
    return demoFinished;
  }

  void startDemo() {
    demoMode = true;
    demoStep = 0;
    demoStartTime = millis();
    demoTemperature = DEFAULT_TEMPERATURE_VALUE;
    demoHumidity = 50.0f;
    demoCO2 = DEFAULT_CO2_VALUE;
  }

  void updateDemo() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - demoStartTime;


  // Define step durations in milliseconds
  const unsigned long stepDurations[] = {
    5000,  // 0 - Welcome to Livegrid
    5000,  // 1 - Your very own aquatic ecosystem
    5000,  // 2 - You start with 5 fish
    5000,  // 3 - They will slowly grow, and reproduce
    5000,  // 4 - But they are affected by the environment
    5000,  // 5 - Temperature affects water color
    15000, // 6 - show
    5000,  // 7 - Humidity affects plant growth
    15000, // 8 - show
    5000,  // 9 - CO2 affects fish behavior
    30000, // 10 - show
    5000,  // 11 - If your environment is good, they will thrive
    5000,  // 12 - And soon you will have an amazing ecosystem
    5000,  // 13 - Take care of them by taking care of yourself
    5000   // 14 - Enjoy
  };

  // Calculate current step and time within step
  unsigned long totalDuration = 0;
  for (demoStep = 0; demoStep < sizeof(stepDurations) / sizeof(stepDurations[0]); demoStep++) {
    if (elapsedTime < totalDuration + stepDurations[demoStep]) {
      break;
    }
    totalDuration += stepDurations[demoStep];
  }

  unsigned long stepElapsedTime = elapsedTime - totalDuration;

  auto pausingSine = [](float t, float pauseDuration) {
    float sineValue = sin(t);
    if (abs(sineValue) < 0.1) {  // Pause near the midpoint
      return 0.0f;
    }
    return sineValue;
  };

  float t;

  // Update demo values and display text based on the current step
  switch (demoStep) {
    case 0:
      snprintf(buffer, sizeof(buffer), "Welcome to\nLivegrid");
      break;
    case 1:
      snprintf(buffer, sizeof(buffer), "Your very own\naquatic\necosystem");
      break;
    case 2:
      snprintf(buffer, sizeof(buffer), "You start\nwith 5 fish");
      break;
    case 3:
      snprintf(buffer, sizeof(buffer), "They will\nslowly grow,\nand reproduce");
      break;
    case 4:
      snprintf(buffer, sizeof(buffer), "But they are\naffected by\nthe\nenvironment");
      break;
    case 5:
      snprintf(buffer, sizeof(buffer), "Temperature\naffects\nwater color");
      updateWater();
      break;
    case 6:
      demoTemperature = DEFAULT_TEMPERATURE_VALUE;
      snprintf(buffer, sizeof(buffer), "Temperature:\n%.0f C", demoTemperature);
      updateWater();
      break;
    case 7:
      snprintf(buffer, sizeof(buffer), "Humidity\naffects\nplant growth");
      updateWater();
      updatePlants();
      break;
    case 8:
      t = 2 * PI * stepElapsedTime / stepDurations[demoStep];
      demoHumidity = 50.0f + 40.0f * pausingSine(t, 0.2);
      snprintf(buffer, sizeof(buffer), "Humidity:\n%.0f %%", demoHumidity);
      updateWater();
      updatePlants();
      break;
     case 9:
      snprintf(buffer, sizeof(buffer), "CO2 affects\nfish behavior");
      break;
    case 10:
      demoCO2 = DEFAULT_CO2_VALUE;
      snprintf(buffer, sizeof(buffer), "CO2:\n%.0f ppm", demoCO2);
      break;
    case 11:
      demoCO2 = DEFAULT_CO2_VALUE;
      snprintf(buffer, sizeof(buffer), "If your\nenvironment\nis good,\nthey will\nthrive");
      break;
    case 12:
      snprintf(buffer, sizeof(buffer), "And soon you\nwill have an\namazing\necosystem");
      break;
    case 13:
      snprintf(buffer, sizeof(buffer), "Take care of\nthem by taking\ncare of\nyourself");
      break;
    case 14:
      snprintf(buffer, sizeof(buffer), "Enjoy !");
      break;
    default:
      demoMode = false;
      demoFinished = true;  // Set demoFinished to true when demo is complete
      saveState();
      return;
  }

  if (demoStep < 5 || demoStep > 8) {
    updateWater();
    boidManager.updateBoids(demoCO2);
    boidManager.renderBoids();
    updateFish();
    updateFood();
    updatePlants();
  }

  drawMultilineText(matrix->foreground, buffer, MIDDLE,
                    TextAlignment::CENTER, &Font5x7Fixed,
                    CRGB(150, 150, 150));
}

  void loadState() {
    if (!aquariumStateManager.loadState(fishArray, matrix)) {
      log_w("Failed to load aquarium state, initializing with default values");
      initializeFish();
    }
  }

  void saveState() {
    aquariumStateManager.saveState(fishArray);
    log_i("Aquarium state saved");
  }

  void periodicSave() {
    unsigned long currentTime = millis();
    if (currentTime - lastSaveTime >= AQUARIUM_SAVE_INTERVAL*60000) {
      aquariumStateManager.saveState(fishArray);
      lastSaveTime = currentTime;
    }
  }

  void ensureMinimumFishPopulation() {
    while (fishArray.size() > NUM_FISH_IDEAL) {
      fishArray.pop_back();
    }
    while (fishArray.size() < NUM_FISH_START) {
      const uint8_t index = static_cast<uint8_t>(fishArray.size());
      spawnFishAt(index, nextRosterSpecies());
    }
  }

  // Initialize fish and store them in a vector of unique pointers
  void initializeFish() {
    fishArray.clear();
    SpeciesId roster[NUM_FISH_IDEAL];
    pickWeightedRoster(roster, NUM_FISH_IDEAL);
    for (int i = 0; i < NUM_FISH_START; i++) {
      spawnFishAt(static_cast<uint8_t>(i), roster[i]);
    }
  }

  // Initialize plants and store them in a vector of unique pointers
  void initializePlants() {
    for (int i = 0; i < NUM_PLANTS; i++) {
      const SpeciesId plantSpecies =
          (i == 0) ? SpeciesId::Anemone : SpeciesId::Kelp;
      plantArray.emplace_back(std::make_unique<Plants>(
          matrix, matrix->getXResolution() * i / NUM_PLANTS,
          matrix->getYResolution() + 7, plantSpecies));
    }
  }

  Fish* addFood(float x = -1, bool fromTouch = false) {
    if (x < 0) {
      x = random(0, matrix->getXResolution());
    }
    foodArray.emplace_back(std::make_unique<Food>(matrix, x));

    float minDistance = std::numeric_limits<float>::max();
    Fish* closestFish = nullptr;

    for (const auto& fish : fishArray) {
      if (fish->getFood() == nullptr && fish->getAge() > AGE_EGG &&
          fish->getAge() < AGE_SENIOR) {
        float distance = fish->getPosition().dist(PVector(x, 0));
        if (distance < minDistance) {
          minDistance = distance;
          closestFish = fish.get();
        }
      }
    }

    if (closestFish) {
      closestFish->setFood(foodArray.back().get());
    }
    if (fromTouch) {
      recordTouchPellet(closestFish);
    }
    return closestFish;
  }

  uint8_t activeFoodCount() {
    uint8_t count = 0;
    for (const auto& food : foodArray) {
      if (food && !food->isOffScreen() && !food->isEaten()) {
        count++;
      }
    }
    return count;
  }

  unsigned long randomDelayBetween(unsigned long minDelay,
                                   unsigned long maxDelay) {
    if (maxDelay <= minDelay) {
      return minDelay;
    }
    return minDelay + static_cast<unsigned long>(
                          random(0, static_cast<long>(maxDelay - minDelay + 1)));
  }

  void scheduleNextAutonomousFood(bool bootWindow = false) {
#if CYD_AUTONOMOUS_LIFE
    lastAutonomousFoodTime = millis();
    unsigned long minDelay = bootWindow ? CYD_AUTONOMOUS_BOOT_MIN_MS
                                        : CYD_AUTONOMOUS_FOOD_MIN_MS;
    unsigned long maxDelay = bootWindow ? CYD_AUTONOMOUS_BOOT_MAX_MS
                                        : CYD_AUTONOMOUS_FOOD_MAX_MS;
    if (weekendMode && !bootWindow) {
      minDelay = (minDelay * 2) / 3;
      maxDelay = (maxDelay * 2) / 3;
    }
    nextAutonomousFoodDelay = randomDelayBetween(minDelay, maxDelay);
#else
    (void)bootWindow;
#endif
  }

  float autonomousFoodX() {
    const uint8_t width = matrix->getXResolution();
    if (width <= 8) {
      return random(0, width);
    }
    return random(4, width - 4);
  }

  void handleAutonomousLife() {
#if CYD_AUTONOMOUS_LIFE
    if (nextAutonomousFoodDelay == 0) {
      scheduleNextAutonomousFood(true);
    }

    const unsigned long now = millis();
    if (now - lastAutonomousFoodTime < nextAutonomousFoodDelay) {
      return;
    }

    if (activeFoodCount() < CYD_AUTONOMOUS_MAX_FOOD) {
      addFood(autonomousFoodX(), false);
    }
    scheduleNextAutonomousFood(false);
#endif
  }

  void recordTouchPellet(Fish* fish) {
    const unsigned long now = millis();
    touchPelletTimes[touchPelletHead] = now;
    touchPelletFish[touchPelletHead] = fish;
    touchPelletHead = (touchPelletHead + 1) % TOUCH_PELLET_HISTORY;
    if (touchPelletStored < TOUCH_PELLET_HISTORY) {
      touchPelletStored++;
    }

    uint8_t recent = 0;
    Fish* countsFish[TOUCH_PELLET_HISTORY] = {};
    uint8_t counts[TOUCH_PELLET_HISTORY] = {};
    uint8_t uniqueFish = 0;
    for (uint8_t i = 0; i < touchPelletStored; ++i) {
      const uint8_t idx =
          (touchPelletHead + TOUCH_PELLET_HISTORY - 1 - i) % TOUCH_PELLET_HISTORY;
      if (now - touchPelletTimes[idx] > 4000) {
        break;
      }
      recent++;
      Fish* candidate = touchPelletFish[idx];
      if (candidate == nullptr) {
        continue;
      }
      bool found = false;
      for (uint8_t j = 0; j < uniqueFish; ++j) {
        if (countsFish[j] == candidate) {
          counts[j]++;
          found = true;
          break;
        }
      }
      if (!found && uniqueFish < TOUCH_PELLET_HISTORY) {
        countsFish[uniqueFish] = candidate;
        counts[uniqueFish] = 1;
        uniqueFish++;
      }
    }

    if (recent >= 8 && murkUntilMs < now) {
      Fish* plumpTarget = fish;
      uint8_t bestCount = 0;
      for (uint8_t j = 0; j < uniqueFish; ++j) {
        if (counts[j] > bestCount) {
          bestCount = counts[j];
          plumpTarget = countsFish[j];
        }
      }
      startOverfeedMess(plumpTarget);
    }
  }

  bool fishStillPresent(Fish* target) const {
    if (target == nullptr) {
      return false;
    }
    for (const auto& fish : fishArray) {
      if (fish.get() == target) {
        return true;
      }
    }
    return false;
  }

  void startOverfeedMess(Fish* plumpTarget) {
    const unsigned long now = millis();
    murkUntilMs = now + 50000;
    if (fishStillPresent(plumpTarget)) {
      plumpTarget->setPlump(1.7f, 50000);
      const PVector pos = plumpTarget->getPosition();
      burpX = static_cast<int16_t>(pos.x);
      burpBaseY = static_cast<int16_t>(pos.y);
      burpUntilMs = now + 1500;
    }
  }

  void updateOverfeedEffects() {
    const unsigned long now = millis();
    if (murkUntilMs > now) {
      const float remaining =
          static_cast<float>(murkUntilMs - now) / 50000.0f;
      water.setMurk(constrain(remaining, 0.0f, 1.0f));
    } else {
      water.setMurk(0.0f);
    }

    if (burpUntilMs > now && matrix->foreground != nullptr) {
      const float t =
          1.0f - static_cast<float>(burpUntilMs - now) / 1500.0f;
      const int16_t y =
          burpBaseY - static_cast<int16_t>(t * 12.0f);
      const int16_t x = burpX + static_cast<int16_t>(sinf(t * 6.0f) * 2.0f);
      matrix->foreground->drawPixel(x, y, CRGB(200, 230, 255));
      if ((millis() / 80) % 2 == 0) {
        matrix->foreground->drawPixel(x, y - 1, CRGB(140, 180, 210));
      }
    }
  }

  const char* nextPokePhrase(const String& bodyType) {
    if (bodyType == "Turtle") {
      return "GERALD";
    }
    static const char* phrases[] = {"NOM", "HI", "ZZZ", "OOF", "HEY"};
    const char* phrase = phrases[pokePhraseIndex % 5];
    pokePhraseIndex++;
    return phrase;
  }

  bool pokeCreature(float x, float y) {
    Fish* nearest = nearestCreatureAt(x, y, nullptr);
    if (nearest == nullptr) {
      return false;
    }

    nearest->poke();
    const PVector pos = nearest->getPosition();
    const char* phrase = nextPokePhrase(nearest->getBodyType());
    strncpy(pokeBubbleText, phrase, sizeof(pokeBubbleText) - 1);
    pokeBubbleText[sizeof(pokeBubbleText) - 1] = '\0';

    const int16_t width = matrix->getXResolution();
    const int16_t height = matrix->getYResolution();
    const int16_t textW =
        static_cast<int16_t>(strlen(pokeBubbleText) * 4);
    pokeBubbleX = static_cast<int16_t>(pos.x) - textW / 2;
    pokeBubbleY = static_cast<int16_t>(pos.y) - 8;
    if (pokeBubbleX < 1) {
      pokeBubbleX = 1;
    }
    if (pokeBubbleX + textW > width - 1) {
      pokeBubbleX = width - 1 - textW;
    }
    if (pokeBubbleY < 14) {
      pokeBubbleY = 14;
    }
    if (pokeBubbleY > height - 14) {
      pokeBubbleY = height - 14;
    }
    pokeBubbleUntilMs = millis() + 1200;
    return true;
  }

  void dropPellet(float x = -1) {
    addFood(x, false);
  }

  void showDiary() {
    const unsigned long uptimeMs = millis() - bootMs;
    const uint32_t hours = uptimeMs / 3600000UL;
    snprintf(diaryText, sizeof(diaryText), "%luH  %u ALIVE  %lu EAT",
             static_cast<unsigned long>(hours),
             static_cast<unsigned>(fishArray.size()),
             static_cast<unsigned long>(pelletsEaten));
    diaryUntilMs = millis() + 8000;
  }

  bool hasPokeBubble(char* textOut, size_t textSize, int16_t* x,
                     int16_t* y) const {
    if (millis() >= pokeBubbleUntilMs || pokeBubbleText[0] == '\0') {
      return false;
    }
    if (textOut != nullptr && textSize > 0) {
      strncpy(textOut, pokeBubbleText, textSize - 1);
      textOut[textSize - 1] = '\0';
    }
    if (x != nullptr) {
      *x = pokeBubbleX;
    }
    if (y != nullptr) {
      *y = pokeBubbleY;
    }
    return true;
  }

  bool hasDiary(char* textOut, size_t textSize) const {
    if (millis() >= diaryUntilMs || diaryText[0] == '\0') {
      return false;
    }
    if (textOut != nullptr && textSize > 0) {
      strncpy(textOut, diaryText, textSize - 1);
      textOut[textSize - 1] = '\0';
    }
    return true;
  }

  bool shouldShowTgif() const {
    return tgifActive;
  }

  void scheduleNextVisitor(bool first) {
    if (first) {
      nextVisitorMs =
          millis() + randomDelayBetween(4UL * 60000UL, 8UL * 60000UL);
    } else {
      nextVisitorMs =
          millis() + randomDelayBetween(45UL * 60000UL, 75UL * 60000UL);
    }
  }

  void startVisitor() {
    const int kind = random(0, 4);
    visitorStartMs = millis();
    visitorX = 0;
    visitorY = 0;
    visitorSpeed = 0;
    switch (kind) {
      case 0:
        visitorKind = VisitorKind::CatPaw;
        visitorX = random(10, matrix->getXResolution() - 10);
        visitorY = 0;
        visitorEndMs = visitorStartMs + 2000;
        break;
      case 1:
        visitorKind = VisitorKind::Duck;
        visitorX = -4;
        visitorY = 18;
        visitorSpeed = 0.35f;
        visitorEndMs = visitorStartMs + 6000;
        break;
      case 2:
        visitorKind = VisitorKind::Diver;
        visitorX = random(0, 2) == 0 ? 6 : matrix->getXResolution() - 7;
        visitorY = 12;
        visitorSpeed = 0.22f;
        visitorEndMs = visitorStartMs + 8000;
        break;
      default:
        visitorKind = VisitorKind::Shadow;
        visitorX = -8;
        visitorY = matrix->getYResolution() / 2;
        visitorSpeed = 0.55f;
        visitorEndMs = visitorStartMs + 3500;
        break;
    }
  }

  void scareNear(PVector source, float radius) {
    for (auto& fish : fishArray) {
      if (fish->getPosition().dist(source) <= radius) {
        fish->scareFrom(source);
      }
    }
    boidManager.scareFrom(source, radius);
  }

  void updateVisitors() {
    const unsigned long now = millis();
    if (visitorKind == VisitorKind::None) {
      if (now >= nextVisitorMs) {
        startVisitor();
      }
      return;
    }

    if (now >= visitorEndMs) {
      visitorKind = VisitorKind::None;
      scheduleNextVisitor(false);
      return;
    }

    const float elapsed = static_cast<float>(now - visitorStartMs);
    switch (visitorKind) {
      case VisitorKind::CatPaw: {
        const int16_t x = static_cast<int16_t>(visitorX);
        for (int16_t dy = 0; dy < 4; ++dy) {
          for (int16_t dx = -2; dx <= 2; ++dx) {
            matrix->foreground->drawPixel(x + dx, dy, CRGB(40, 30, 28));
          }
        }
        matrix->foreground->drawPixel(x - 3, 4, CRGB(40, 30, 28));
        matrix->foreground->drawPixel(x - 1, 5, CRGB(40, 30, 28));
        matrix->foreground->drawPixel(x + 1, 5, CRGB(40, 30, 28));
        matrix->foreground->drawPixel(x + 3, 4, CRGB(40, 30, 28));
        if (elapsed < 200) {
          scareNear(PVector(visitorX, 8), 18.0f);
        }
        break;
      }
      case VisitorKind::Duck: {
        visitorX += visitorSpeed;
        const int16_t x = static_cast<int16_t>(visitorX);
        const int16_t y = static_cast<int16_t>(visitorY);
        for (int16_t dy = 0; dy < 3; ++dy) {
          for (int16_t dx = 0; dx < 5; ++dx) {
            matrix->foreground->drawPixel(x + dx, y + dy, CRGB(255, 200, 40));
          }
        }
        matrix->foreground->drawPixel(x + 5, y + 1, CRGB(255, 120, 20));
        matrix->foreground->drawPixel(x + 1, y - 1, CRGB(255, 220, 80));
        break;
      }
      case VisitorKind::Diver: {
        visitorY += visitorSpeed;
        const int16_t x = static_cast<int16_t>(visitorX);
        const int16_t y = static_cast<int16_t>(visitorY);
        matrix->foreground->drawPixel(x, y, CRGB(220, 220, 240));
        matrix->foreground->drawPixel(x, y + 1, CRGB(30, 90, 180));
        matrix->foreground->drawPixel(x, y + 2, CRGB(30, 90, 180));
        matrix->foreground->drawPixel(x - 1, y + 1, CRGB(200, 40, 40));
        matrix->foreground->drawPixel(x + 1, y + 3, CRGB(180, 180, 200));
        break;
      }
      case VisitorKind::Shadow: {
        visitorX += visitorSpeed;
        const int16_t x = static_cast<int16_t>(visitorX);
        const int16_t midY = matrix->getYResolution() / 2;
        for (int16_t dy = -10; dy <= 10; ++dy) {
          const uint8_t shade = 18 + abs(dy);
          for (int16_t dx = 0; dx < 10; ++dx) {
            matrix->foreground->drawPixel(x + dx, midY + dy,
                                          CRGB(shade, shade, shade + 8));
          }
        }
        scareNear(PVector(visitorX + 5, midY), 22.0f);
        break;
      }
      default:
        break;
    }
  }

  void applyDayContext(const tm* localTime) {
    if (localTime == nullptr) {
      weekendMode = false;
      tgifActive = false;
      boidManager.setWeekendSchooling(false);
      return;
    }

    const bool isFriday = localTime->tm_wday == 5;
    const bool isWeekend =
        localTime->tm_wday == 0 || localTime->tm_wday == 6;
    tgifActive = isFriday && localTime->tm_hour >= 16;
    weekendMode = isWeekend;
    boidManager.setWeekendSchooling(weekendMode);

    if (weekendMode && !fishArray.empty()) {
      PVector centroid(0, 0);
      uint8_t count = 0;
      for (const auto& fish : fishArray) {
        if (!fish->isPokeable()) {
          continue;
        }
        centroid = centroid + fish->getPosition();
        count++;
      }
      if (count > 0) {
        centroid.x /= count;
        centroid.y /= count;
        for (auto& fish : fishArray) {
          if (fish->isPokeable()) {
            fish->schoolToward(centroid);
          }
        }
      }
    }
  }

  float currentHumidity() {
    if (demoMode) {
      return demoHumidity;
    }
#if defined(FIXED_ENVIRONMENT)
    return CYD_FIXED_HUMIDITY_PERCENT;
#else
    return scd40->isFirstReadingReceived() ? scd40->getHumidity() : 50;
#endif
  }

  float currentTemperature() {
    if (demoMode) {
      return demoTemperature;
    }
#if defined(FIXED_ENVIRONMENT)
    return DEFAULT_TEMPERATURE_VALUE;
#else
    return scd40->isFirstReadingReceived() ? scd40->getTemperature()
                                           : DEFAULT_TEMPERATURE_VALUE;
#endif
  }

  long currentCO2() {
    if (demoMode) {
      return static_cast<long>(demoCO2);
    }
#if defined(FIXED_ENVIRONMENT)
    return static_cast<long>(DEFAULT_CO2_VALUE);
#else
    return scd40->isFirstReadingReceived()
               ? static_cast<long>(scd40->getCO2())
               : static_cast<long>(DEFAULT_CO2_VALUE);
#endif
  }

  // Update all plants in the aquarium
  void updatePlants() {
    float humidity = currentHumidity();
    for (auto& plant : plantArray) {
      plant->update(humidity);
    }
  }

  // Update the water environment
  void updateWater() {
    water.update(currentTemperature());
  }

  // Update all fish in the aquarium
  void updateFish() {
    long co2 = currentCO2();
    const bool keepCreaturesVisible =
        demoMode || (CYD_KEEP_CREATURES_ON_SCREEN != 0);
    for (auto it = fishArray.begin(); it != fishArray.end();) {
      bool destroy = (*it)->update(co2, keepCreaturesVisible);
      if (destroy) {
        it = fishArray.erase(it);
      } else {
        (*it)->display();
        ++it;
      }
    }

    // Keep the cast at NUM_FISH_IDEAL with weighted distinct species.
    if (fishArray.size() < NUM_FISH_IDEAL) {
      const SpeciesId id = nextRosterSpecies();
      const uint8_t index = static_cast<uint8_t>(fishArray.size());
      spawnFishAt(index, id);
      scheduleNextSpeciesRefill();
    } else if (millis() >= nextSpeciesRefillMs) {
      // Swap the oldest adult for a fresh weighted pick.
      if (!fishArray.empty()) {
        fishArray.erase(fishArray.begin());
        spawnFishAt(static_cast<uint8_t>(fishArray.size()), nextRosterSpecies());
      }
      scheduleNextSpeciesRefill();
    }
  }

  void updateFood() {
    for (auto it = foodArray.begin(); it != foodArray.end();) {
      (*it)->update();
      if ((*it)->isEaten()) {
        pelletsEaten++;
        it = foodArray.erase(it);
      } else if ((*it)->isOffScreen()) {
        it = foodArray.erase(it);
      } else {
        (*it)->display();
        ++it;
      }
    }
  }

  void handleTouchInput() {
    // Continue adding food while touch is active
    if (touchActive) {
      unsigned long currentTime = millis();
      if (currentTime - lastFoodTime >= FOOD_INTERVAL) {
        addFood(-1, true);
        lastFoodTime = currentTime;
      }
    }
  }

  // Called externally when a feed-hold gesture begins
  void onTouchStarted() {
    if (!touchActive) {
      touchActive = true;
      addFood(-1, true);
      lastFoodTime = millis();
    }
  }

  // Called externally when touch is released
  void onTouchReleased() {
    touchActive = false;
  }

  void updateSensorData(bool showSensorData) {
    if (showSensorData && !demoMode) {
      if (scd40->isFirstReadingReceived()) {
        float temperature = scd40->getTemperature();
        float humidity = scd40->getHumidity();
        float co2 = scd40->getCO2();

        if (stateManager->getState()->temperatureUnit == TemperatureUnit::FAHRENHEIT) {
            // Convert to Fahrenheit
              temperature = stateManager->getState()->environment.temperature_fahrenheit.value;
              snprintf(buffer, sizeof(buffer),
                  "%s\nTemp: %.1f F\nHumidity: %.0f %%\nCO2: %.0f ppm", "",
                  temperature, humidity, co2);
          } else {
              snprintf(buffer, sizeof(buffer),
                  "%s\nTemp: %.1f C\nHumidity: %.0f %%\nCO2: %.0f ppm", "",
                  temperature, humidity, co2);
          }
        
        drawMultilineText(matrix->foreground, buffer, MIDDLE,
                          TextAlignment::CENTER, &Font4x7Fixed,
                          CRGB(150, 150, 150));
      } else {
        drawMultilineText(matrix->foreground, "Sensors\nWarming Up...", MIDDLE,
                          TextAlignment::CENTER, &Font4x7Fixed,
                          CRGB(150, 150, 150));
      }
    }
  }

  // General update function that updates all components of the aquarium
  void update(bool showSensorData = false, const tm* localTime = nullptr) {
    applyDayContext(localTime);
    handleTouchInput();
    handleAutonomousLife();

    if (demoMode) {
      updateDemo();
    } else {
      updateWater();
      boidManager.updateBoids(currentCO2());
      boidManager.renderBoids();
      updateFish();
      updateFood();
      updatePlants();
      updateOverfeedEffects();
      updateVisitors();
      updateSensorData(showSensorData);
      periodicSave();
    }
  }

  void unlockAmbientInView() {
    if (!tankCamera.isNear()) {
      return;
    }
    const float midY = tankCamera.originY + tankCamera.windowHeight() * 0.5f;
    if (midY > matrix->getYResolution() * 0.55f) {
      for (const auto& plant : plantArray) {
        const SpeciesId id = plant->getSpeciesId();
        if (!fieldGuide.isDiscovered(id)) {
          unlockSpecies(id);
        }
      }
    } else if (!fieldGuide.isDiscovered(SpeciesId::Chromis)) {
      chromisDiscovered = true;
      unlockSpecies(SpeciesId::Chromis);
    }
    if (!fieldGuide.isDiscovered(SpeciesId::FishFlake) && !foodArray.empty()) {
      for (const auto& food : foodArray) {
        if (food->isEaten() || food->isOffScreen()) {
          continue;
        }
        const PVector fp = food->getPosition();
        if (fp.x >= tankCamera.originX &&
            fp.x < tankCamera.originX + tankCamera.windowWidth() &&
            fp.y >= tankCamera.originY &&
            fp.y < tankCamera.originY + tankCamera.windowHeight()) {
          flakeDiscovered = true;
          unlockSpecies(SpeciesId::FishFlake);
          break;
        }
      }
    }
  }

  void syncCamera() {
    if (tankCamera.following && tankCamera.followFishIndex >= 0 &&
        static_cast<size_t>(tankCamera.followFishIndex) < fishArray.size()) {
      const PVector pos =
          fishArray[tankCamera.followFishIndex]->getPosition();
      tankCamera.updateFollow(pos.x, pos.y);
    }
    tankCamera.easeStep();
    TankDetail::nearView = tankCamera.isNear();
    matrix->setCameraView(tankCamera.zoom, tankCamera.originX,
                          tankCamera.originY);
    unlockAmbientInView();
  }

  void display() {
    syncCamera();
    matrix->compositeLayers();
  }

  // Destructor to clean up resources
  ~Aquarium() {
    // Unique pointers automatically clean up
  }

  void drawMultilineText(GFX_Layer* layer, const char* text,
                         textPosition textPos, TextAlignment alignment,
                         const GFXfont* f, CRGB color) {
    layer->setFont(f);
    layer->setTextColor(layer->color565(color.r, color.g, color.b));

    // Split text into lines
    std::vector<String> lines;
    String currentLine;
    for (const char* c = text; *c; ++c) {
      if (*c == '\n') {
        lines.push_back(currentLine);
        currentLine = "";
      } else {
        currentLine += *c;
      }
    }
    if (!currentLine.isEmpty()) {
      lines.push_back(currentLine);
    }

    // Calculate total height and maximum width
    int16_t x1, y1;
    uint16_t w, h, maxWidth = 0, totalHeight = 0;
    for (const String& line : lines) {
      layer->getTextBounds(line.c_str(), 0, 0, &x1, &y1, &w, &h);
      maxWidth = max(maxWidth, w);
      totalHeight += h;
    }

    // Calculate starting Y position
    int16_t startY;
    if (textPos == TOP) {
      startY = h;
    } else if (textPos == BOTTOM) {
      startY = layer->getHeight() - totalHeight;
    } else {  // MIDDLE
      startY = (layer->getHeight() - totalHeight) / 2 + h;
    }

    // Draw each line
    for (const String& line : lines) {
      int16_t lineX;
      layer->getTextBounds(line.c_str(), 0, 0, &x1, &y1, &w, &h);

      switch (alignment) {
        case TextAlignment::LEFT:
          lineX = 0;
          break;
        case TextAlignment::CENTER:
          lineX = (layer->getWidth() - w) / 2;
          break;
        case TextAlignment::RIGHT:
          lineX = layer->getWidth() - w;
          break;
      }

      layer->setCursor(lineX, startY);
      layer->print(line);
      startY += h;
    }
  }
};



#endif  // AQUARIUM_H
