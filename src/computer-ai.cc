#include "computer-ai.h"

Location ComputerAi::ChooseNextShot() {
  if (has_last_shot && board.IsHit(last_shot)) {
    TargetAllLocationsAround(last_shot);
    mode = FIRE;
  } else if (next_targets.empty()) {
    mode = HUNT;
  }

  if (mode == HUNT) {
    const Location location_to_shoot = placement_generator.ChooseLocation(board.NotFiredLocations());

    has_last_shot = true;
    last_shot = location_to_shoot;

    return location_to_shoot;
  } else {
    const Location target = next_targets.top();
    last_shot = target;
    next_targets.pop();
    return target;
  }
}

void ComputerAi::TargetAllLocationsAround(const Location location) {
  const Location top(location.x, location.y - 1);
  const Location bottom(location.x, location.y + 1);
  const Location left(location.x - 1, location.y);
  const Location right(location.x + 1, location.y);

  AddTargetLocation(top);
  AddTargetLocation(bottom);
  AddTargetLocation(left);
  AddTargetLocation(right);
}

void ComputerAi::AddTargetLocation(const Location location) {
  if (!board.HasShot(location)) {
    next_targets.push(location);
  }
}
