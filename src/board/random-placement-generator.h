#ifndef SRC_BOARD_RANDOM_PLACEMENT_GENERATOR_H
#define SRC_BOARD_RANDOM_PLACEMENT_GENERATOR_H

#include <random>

#include "placement-generator.h"

class RandomPlacementGenerator : public PlacementGenerator {
public:
  RandomPlacementGenerator();

  Orientation GenerateOrientation() override;
  Location GenerateLocation(const int width, const int height) override;
  Location ChooseLocation(const std::vector<Location>& choices) override;

private:
  int RandomNumber(const int start, const int end);

  std::mt19937_64 random;
};

#endif // SRC_BOARD_RANDOM_PLACEMENT_GENERATOR_H
