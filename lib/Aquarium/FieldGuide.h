#ifndef FIELD_GUIDE_H
#define FIELD_GUIDE_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "SpeciesCatalog.h"

struct FieldGuideEntry {
  bool discovered = false;
  uint32_t timesSeen = 0;
  uint32_t firstSeenMs = 0;
};

class FieldGuide {
 public:
  static constexpr const char* FILENAME = "/field_guide.json";

  FieldGuideEntry entries[SPECIES_COUNT] = {};

  void begin() { load(); }

  bool discover(SpeciesId id) {
    const uint8_t index = static_cast<uint8_t>(id);
    if (index >= SPECIES_COUNT) {
      return false;
    }
    FieldGuideEntry& entry = entries[index];
    const bool first = !entry.discovered;
    entry.discovered = true;
    entry.timesSeen++;
    if (first) {
      entry.firstSeenMs = millis();
      save();
    } else if (entry.timesSeen % 5 == 0) {
      save();
    }
    return first;
  }

  bool isDiscovered(SpeciesId id) const {
    const uint8_t index = static_cast<uint8_t>(id);
    if (index >= SPECIES_COUNT) {
      return false;
    }
    return entries[index].discovered;
  }

  uint32_t timesSeen(SpeciesId id) const {
    const uint8_t index = static_cast<uint8_t>(id);
    if (index >= SPECIES_COUNT) {
      return 0;
    }
    return entries[index].timesSeen;
  }

  uint8_t discoveredCount() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < SPECIES_COUNT; ++i) {
      if (entries[i].discovered) {
        count++;
      }
    }
    return count;
  }

  void save() {
    JsonDocument doc;
    JsonArray arr = doc["species"].to<JsonArray>();
    for (uint8_t i = 0; i < SPECIES_COUNT; ++i) {
      JsonObject obj = arr.add<JsonObject>();
      obj["id"] = i;
      obj["discovered"] = entries[i].discovered;
      obj["timesSeen"] = entries[i].timesSeen;
      obj["firstSeenMs"] = entries[i].firstSeenMs;
    }

    File file = LittleFS.open(FILENAME, "w");
    if (!file) {
      log_e("[FIELD_GUIDE] Failed to open %s for writing", FILENAME);
      return;
    }
    serializeJson(doc, file);
    file.close();
  }

  bool load() {
    File file = LittleFS.open(FILENAME, "r");
    if (!file) {
      return false;
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
      return false;
    }
    JsonArray arr = doc["species"];
    for (JsonObject obj : arr) {
      const uint8_t id = obj["id"] | 0;
      if (id >= SPECIES_COUNT) {
        continue;
      }
      entries[id].discovered = obj["discovered"] | false;
      entries[id].timesSeen = obj["timesSeen"] | 0;
      entries[id].firstSeenMs = obj["firstSeenMs"] | 0;
    }
    return true;
  }

  static void eraseFile() { LittleFS.remove(FILENAME); }
};

#endif  // FIELD_GUIDE_H
