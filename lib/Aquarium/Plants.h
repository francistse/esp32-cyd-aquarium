#ifndef PLANTS_H
#define PLANTS_H

#include <Arduino.h>
#include <Matrix.h>
#include <PVector.h>
#include <SpeciesCatalog.h>

struct Branch {
  PVector startPos;
  std::vector<PVector> nodes;
};

class Plants {
 private:
  Matrix* matrix;
  PVector pos;
  std::vector<Branch> branches;
  uint8_t numBranches;
  uint8_t branchSizeBase = 3;
  uint8_t numNodes;
  std::vector<float> phaseOffsets;
  SpeciesId plantSpecies = SpeciesId::Kelp;

 public:
  Plants(Matrix* m, uint8_t x, uint8_t y, SpeciesId species = SpeciesId::Kelp)
      : matrix(m), plantSpecies(species) {
    pos = PVector(x, y);

    if (plantSpecies == SpeciesId::Anemone) {
      numBranches = random(8, 12);
      branchSizeBase = 2;
    } else {
      numBranches = random(6, 10);
      branchSizeBase = 3;
    }

    for (uint8_t i = 0; i < numBranches; i++) {
      PVector branchStart =
          PVector::fromAngle(random((PI)*1000, (TWO_PI)*1000) / 1000.0);
      branchStart *= branchSizeBase;

      Branch branch;
      branch.startPos = branchStart;
      branches.push_back(branch);
      phaseOffsets.push_back(random(0, 500) / 100.0);
    }
    setupNodes();
  }

  SpeciesId getSpeciesId() const { return plantSpecies; }

  void update(uint8_t humidity = 50) {
    unsigned long currentTime = millis();
    float sizeFactor = map(humidity, 0, 100, 0, 250);
    sizeFactor /= 100;

    const CRGB stemColor =
        plantSpecies == SpeciesId::Anemone ? CRGB(40, 90, 70) : CRGB(20, 120, 45);
    const CRGB leafColor =
        plantSpecies == SpeciesId::Anemone ? CRGB(220, 140, 150)
                                           : CRGB(40, 180, 70);
    const CRGB tipColor =
        plantSpecies == SpeciesId::Anemone ? CRGB(255, 180, 170)
                                           : CRGB(255, 220, 60);

    for (uint8_t i = 0; i < branches.size(); i++) {
      matrix->foreground->drawLine(
          branches[i].nodes[0].x * sizeFactor + pos.x,
          branches[i].nodes[0].y * sizeFactor + pos.y, pos.x, pos.y, stemColor);
      for (uint8_t j = 1; j < branches[i].nodes.size(); j++) {
        PVector node = branches[i].nodes[j];
        PVector prevNode = branches[i].nodes[j - 1];

        float swayAmplitude = 0.8 * j;
        float sway =
            sin(currentTime / 10000.0 + phaseOffsets[i]) * swayAmplitude;

        matrix->foreground->drawLine(
            prevNode.x * sizeFactor + sway + pos.x,
            prevNode.y * sizeFactor + pos.y, node.x * sizeFactor + sway + pos.x,
            node.y * sizeFactor + pos.y, stemColor);

        // Leaf bump on alternating nodes for kelp; soft tips for anemone.
        if (j % 2 == 0) {
          const float lx = node.x * sizeFactor + sway + pos.x +
                           cos(currentTime / 8000.0f + i) * 1.2f;
          const float ly = node.y * sizeFactor + pos.y;
          matrix->foreground->drawPixel(lx, ly, leafColor);
        }

        if (j == branches[i].nodes.size() - 1) {
          float glowFactor =
              (sin(currentTime / 10000.0 + phaseOffsets[i])) - 0.5;
          if (glowFactor > 0 || plantSpecies == SpeciesId::Anemone) {
            matrix->foreground->fillCircle(
                node.x * sizeFactor + sway + pos.x,
                node.y * sizeFactor + pos.y, 1, tipColor);
          }
        }
      }
    }
  }

 private:
  void setupNodes() {
    for (uint8_t i = 0; i < branches.size(); i++) {
      numNodes = plantSpecies == SpeciesId::Anemone ? random(3, 6)
                                                    : random(4, 8);
      branches[i].nodes.push_back(branches[i].startPos);
      for (uint8_t j = 1; j < numNodes; j++) {
        PVector node = buildNode(branches[i].nodes[j - 1]);
        branches[i].nodes.push_back(node);
      }
    }
  }

  PVector buildNode(PVector startVec) {
    float theta = startVec.heading();
    float target = 3 * PI / 2;
    float diff = target - theta;
    if (diff > PI) {
      diff -= TWO_PI;
    } else if (diff < -PI) {
      diff += TWO_PI;
    }
    float increment = diff * 0.5;

    PVector endPos = PVector::fromAngle(theta + increment);
    endPos *= branchSizeBase;
    endPos += startVec;
    return endPos;
  }
};

#endif  // PLANTS_H
