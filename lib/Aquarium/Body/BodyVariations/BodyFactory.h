#ifndef BODYFACTORY_H
#define BODYFACTORY_H

#include <Arduino.h>
#include <Body/Body.h>
#include <Body/Fin.h>
#include <Body/Head.h>
#include <Body/Tail.h>
#include <Matrix.h>
#include <SpeciesCatalog.h>

#include <chrono>
#include <functional>
#include <random>
#include <vector>

#include "FishBody.h"
#include "LargeFaunaBody.h"
#include "OctopusBody.h"
#include "SnakeBody.h"
#include "StarBody.h"
#include "TurtleBody.h"

class BodyFactory {
 private:
  Matrix* matrix;
  std::mt19937 gen;

 public:
  BodyFactory(Matrix* m)
      : matrix(m),
        gen(std::chrono::system_clock::now().time_since_epoch().count()) {}

  Head* createHead(const String& type) {
    if (type == "TriangleHead") return new TriangleHead(matrix);
    if (type == "FrogHead") return new FrogHead(matrix);
    if (type == "NeedleHead") return new NeedleHead(matrix);
    return new TriangleHead(matrix);
  }

  Tail* createTail(const String& type) {
    if (type == "noTail") return new noTail(matrix);
    if (type == "TriangleTail") return new TriangleTail(matrix);
    if (type == "CurvyTail") return new CurvyTail(matrix);
    if (type == "WavyTail") return new WavyTail(matrix);
    return new noTail(matrix);
  }

  Fin* createFin(const String& type) {
    if (type == "TriangleFin") return new TriangleFin(matrix);
    if (type == "EllipseFin") return new EllipseFin(matrix);
    if (type == "LegFin") return new LegFin(matrix);
    if (type == "RoundFin") return new RoundFin(matrix);
    return new TriangleFin(matrix);
  }

  Body* createBodyForSpecies(SpeciesId species) {
    Head* head = new TriangleHead(matrix);
    Tail* tail = new noTail(matrix);
    Fin* fin = new TriangleFin(matrix);
    const SilhouetteKind sil = speciesInfo(species).silhouette;

    switch (sil) {
      case SilhouetteKind::Turtle:
        return new TurtleBody(matrix, head, tail, fin);
      case SilhouetteKind::Star:
        return new StarBody(matrix, head, tail, fin);
      case SilhouetteKind::Snake:
        return new SnakeBody(matrix, head, tail, fin);
      case SilhouetteKind::Octopus:
        return new OctopusBody(matrix, head, tail, fin);
      case SilhouetteKind::Shark:
      case SilhouetteKind::WhaleShark:
      case SilhouetteKind::Manta:
      case SilhouetteKind::Whale:
        return new LargeFaunaBody(matrix, head, tail, fin, species);
      case SilhouetteKind::Schooler:
      case SilhouetteKind::ChromisShoal:
      case SilhouetteKind::Fusilier:
      case SilhouetteKind::Anthias:
      case SilhouetteKind::Cardinal:
      case SilhouetteKind::Sergeant:
      case SilhouetteKind::Sweeper:
      case SilhouetteKind::Silverside:
      case SilhouetteKind::Dartfish:
      case SilhouetteKind::Surgeon:
      case SilhouetteKind::Unicorn:
      case SilhouetteKind::Clownfish:
      case SilhouetteKind::BlueTang:
      case SilhouetteKind::YellowTang:
      case SilhouetteKind::MoorishIdol:
      default:
        return new FishBody(matrix, head, tail, fin, species);
    }
  }

  Body* createBody(const String& type, const String& headType,
                   const String& tailType, const String& finType) {
    (void)headType;
    (void)tailType;
    (void)finType;
    return createBodyForSpecies(speciesIdFromBodyType(type));
  }

  Body* createBody(const String& type) {
    return createBodyForSpecies(speciesIdFromBodyType(type));
  }

  Body* createBody(SpeciesId species) {
    return createBodyForSpecies(species);
  }
};

#endif  // BODYFACTORY_H
