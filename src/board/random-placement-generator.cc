#include "random-placement-generator.h"

RandomPlacementGenerator::RandomPlacementGenerator() : random(std::random_device()()) {}

Orientation RandomPlacementGenerator::GenerateOrientation() {
  if (RandomNumber(0, 1) == 0) {
    return Orientation::Horizontal;
  }

  return Orientation::Vertical;
}

Location RandomPlacementGenerator::GenerateLocation(const int width, const int height) {
  return Location(RandomNumber(1, width), RandomNumber(1, height));
}

int RandomPlacementGenerator::RandomNumber(const int start, const int end) {
  std::uniform_int_distribution<int> distribution(start, end);
  return distribution(random);
}

Location RandomPlacementGenerator::ChooseLocation(const std::vector<Location>& choices) {
  return choices.at(RandomNumber(0, choices.size() - 1));
}
