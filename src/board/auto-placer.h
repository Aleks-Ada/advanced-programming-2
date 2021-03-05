#ifndef SRC_BOARD_AUTO_PLACER_H
#define SRC_BOARD_AUTO_PLACER_H

#include "board.h"
#include "placement-generator.h"

class AutoPlacer {
public:
  explicit AutoPlacer(Board& board, PlacementGenerator& placement_generator)
    : board(board), placement_generator(placement_generator) {}

  bool AutoPlace(const std::vector<ShipType>& boats);

private:
  Board& board;
  PlacementGenerator& placement_generator;
};

#endif // SRC_BOARD_AUTO_PLACER_H
