#ifndef SRC_BOARD_COMPUTER_AI_H
#define SRC_BOARD_COMPUTER_AI_H

#include <stack>

#include "board/random-placement-generator.h"

class ComputerAi {
public:
  explicit ComputerAi(Board& board, PlacementGenerator& placement_generator)
  : board(board), placement_generator(placement_generator) {}

  Location ChooseNextShot();

private:
  void TargetAllLocationsAround(const Location location);
  void AddTargetLocation(const Location location);

  enum TargetingMode {
    HUNT,
    FIRE
  };

  Board& board;
  PlacementGenerator& placement_generator;

  TargetingMode mode = HUNT;

  bool has_last_shot;
  Location last_shot;

  std::stack<Location> next_targets;
};

#endif // SRC_BOARD_COMPUTER_AI_H
