#pragma once

#include <vector>
#include "Boid.h"
#include "Matrix.h"
#include "AquariumSettings.h"
#include <SCD40Settings.h>

class BoidManager {
private:
    std::vector<std::vector<Boid>> boidGroups;
    Matrix* matrix;
    PVector limits;
    bool weekendSchooling = false;

public:
    BoidManager(Matrix* m);
    void initializeBoids();
    void updateBoids(long co2 = 600);
    void renderBoids();
    void setWeekendSchooling(bool enabled);
    void scareFrom(PVector obstacle, float radius);
};