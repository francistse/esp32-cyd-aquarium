#ifndef FOOD_H
#define FOOD_H

#include <Arduino.h>
#include <Matrix.h>
#include <PVector.h>

class Food {
 private:
  Matrix* matrix;
  PVector position;
  static constexpr float fallSpeed = 0.3;
  bool eaten = false;
  bool offScreen = false;

 public:
  Food(Matrix* m, float x) : matrix(m), position(x, 0) {}

  void update() {
    position.y += fallSpeed;
    if (position.y >= matrix->getYResolution()) {
      offScreen = true;
    }
  }

  void display() {
    if (eaten) {
      return;
    }
    // Amber 3-pixel flake.
    const CRGB flake(255, 180, 40);
    const CRGB edge(220, 120, 20);
    matrix->foreground->drawPixel(position.x, position.y, flake);
    matrix->foreground->drawPixel(position.x + 1, position.y, edge);
    matrix->foreground->drawPixel(position.x, position.y + 1, edge);
  }

  PVector getPosition() { return position; }

  bool isOffScreen() { return offScreen; }

  void eat() { eaten = true; }

  bool isEaten() { return eaten; }
};

#endif  // FOOD_H
