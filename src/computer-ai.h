#ifndef SRC_BOARD_COMPUTER_AI_H
#define SRC_BOARD_COMPUTER_AI_H

#include <stack>
#include <set>

#include "board/random-placement-generator.h"

class ComputerAi {
public:
  explicit ComputerAi(Board& board, PlacementGenerator& placement_generator)
  : board(board), placement_generator(placement_generator) {}

  Location ChooseNextShot();

private:
  bool IsValidLocation(const Location location) const;
  bool AlreadyTargetedLocation(const Location location) const;

  Location ChooseTarget();

  void TargetAllLocationsAroundShotIfHit(const Location location);
  void TargetLocationsAround(const Location location);
  void AddTargetLocation(const Location location);

private:
  Board& board;
  PlacementGenerator& placement_generator;

  Location last_shot;
  std::set<Location> already_targeted_locations;
  std::stack<Location> next_targets;
};

#endif // SRC_BOARD_COMPUTER_AI_H
