#ifndef SRC_BOARD_PLACEMENT_GENERATOR_H
#define SRC_BOARD_PLACEMENT_GENERATOR_H

#include "board.h"

class PlacementGenerator {
public:
  virtual Orientation GenerateOrientation() = 0;
  virtual Location GenerateLocation(const int width, const int height) = 0;
};

#endif // SRC_BOARD_PLACEMENT_GENERATOR_H
