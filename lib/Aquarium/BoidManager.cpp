#include "BoidManager.h"

#include <random>

BoidManager::BoidManager(Matrix* m)
    : matrix(m), limits(m->getXResolution(), m->getYResolution()) {}

void BoidManager::initializeBoids() {
  boidGroups.resize(BOID_GROUPS);

  for (int group = 0; group < BOID_GROUPS; group++) {
    int numBoids = random(NUM_BOIDS);
    for (int i = 0; i < numBoids; i++) {
      boidGroups[group].emplace_back(random(0, matrix->getXResolution()),
                                     random(0, matrix->getYResolution()),
                                     &limits);
      boidGroups[group][i].maxspeed = random(BOID_MAX_SPEED) / 10.0f;
      boidGroups[group][i].maxforce = random(BOID_MAX_FORCE) / 10.0f;
    }
  }
}

void BoidManager::updateBoids(long co2) {
  float speedMultiplier = map(co2, CO2_BAD, CO2_REALBAD, 100.0f, 0.0f);
  speedMultiplier = constrain(speedMultiplier, 0.0f, 100.0f);
  speedMultiplier /= 100.0f;
  for (auto& group : boidGroups) {
    for (auto& boid : group) {
      if (weekendSchooling) {
        boid.desiredseparation = 2.0f;
        boid.neighbordist = 14.0f;
      } else {
        boid.desiredseparation = 4.0f;
        boid.neighbordist = 8.0f;
      }
      boid.run(group.data(), group.size(), speedMultiplier);
      boid.avoidBorders();
    }
  }
}

void BoidManager::renderBoids() {
  for (const auto& group : boidGroups) {
    for (const auto& boid : group) {
      // Tiny blue chromis: head, body, tail along velocity.
      const float angle = atan2(boid.velocity.y, boid.velocity.x);
      const float x = boid.location.x;
      const float y = boid.location.y;
      const float cx = x + cos(angle);
      const float cy = y + sin(angle);
      const float tx = x - cos(angle);
      const float ty = y - sin(angle);
      const CRGB body(40, 140, 220);
      const CRGB tail(20, 90, 180);
      matrix->foreground->drawPixel(cx, cy, body);  // head
      matrix->foreground->drawPixel(x, y, body);    // body
      matrix->foreground->drawPixel(tx, ty, tail);  // tail
    }
  }
}

void BoidManager::setWeekendSchooling(bool enabled) {
  weekendSchooling = enabled;
}

void BoidManager::scareFrom(PVector obstacle, float radius) {
  for (auto& group : boidGroups) {
    for (auto& boid : group) {
      boid.repelForce(obstacle, radius);
    }
  }
}