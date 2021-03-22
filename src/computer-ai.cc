#include "computer-ai.h"

std::vector<Location> All8LocationsAround(const Location location) {
  std::vector<Location> locations;

  const Location top(location.x, location.y - 1);
  const Location bottom(location.x, location.y + 1);
  const Location left(location.x - 1, location.y);
  const Location right(location.x + 1, location.y);
  const Location top_left(location.x - 1, location.y - 1);
  const Location top_right(location.x + 1, location.y - 1);
  const Location bottom_left(location.x - 1, location.y + 1);
  const Location bottom_right(location.x + 1, location.y + 1);

  locations.push_back(top);
  locations.push_back(bottom);
  locations.push_back(left);
  locations.push_back(right);
  locations.push_back(top_left);
  locations.push_back(top_right);
  locations.push_back(bottom_left);
  locations.push_back(bottom_right);

  return locations;
}

std::vector<Location> All4LocationsAround(const Location location) {
  std::vector<Location> locations;

  const Location top(location.x, location.y - 1);
  const Location bottom(location.x, location.y + 1);
  const Location left(location.x - 1, location.y);
  const Location right(location.x + 1, location.y);

  locations.push_back(top);
  locations.push_back(bottom);
  locations.push_back(left);
  locations.push_back(right);

  return locations;
}

bool ComputerAi::IsValidLocation(const Location location) const {
  return board.IsWithinBounds(location) && !board.HasShot(location);
}

bool ComputerAi::AlreadyTargetedLocation(const Location location) const {
  return already_targeted_locations.find(location) != already_targeted_locations.end();
}

Location ComputerAi::ChooseNextShot() {
  TargetAllLocationsAroundShotIfHit(last_shot);

  Location target;

  while (true) {
    target = ChooseTarget();

    if (board.HasShot(target)) {
      already_targeted_locations.emplace(target);
    } else {
      break;
    }
  }

  last_shot = target;
  already_targeted_locations.emplace(target);

  return target;
}

Location ComputerAi::ChooseTarget() {
  Location target;

  if (next_targets.empty()) {
    target = placement_generator.ChooseLocation(board.NotFiredLocations());
  } else {
    target = next_targets.top();
    next_targets.pop();
  }

  return target;
}

void ComputerAi::TargetAllLocationsAroundShotIfHit(const Location location) {
  if (board.HasShot(location)) {
    if (board.IsMine(location)) {
      for (const Location sub_location : All8LocationsAround(location)) {
        if (!AlreadyTargetedLocation(sub_location)) {
          TargetAllLocationsAroundShotIfHit(sub_location);
        }
      }
    } else if (board.IsHit(location)) {
      TargetLocationsAround(location);
    }
  }
}

void ComputerAi::TargetLocationsAround(const Location location) {
  for (const Location sub_location : All4LocationsAround(location)) {
    AddTargetLocation(sub_location);
  }
}

void ComputerAi::AddTargetLocation(const Location location) {
  if (IsValidLocation(location) && !AlreadyTargetedLocation(location)) {
    next_targets.push(location);
  }
}
