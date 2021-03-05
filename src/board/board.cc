#include "board.h"

#include <algorithm>

std::vector<Location> AllLocationsFor(const Boat& boat, const Location start_location) {
  std::vector<Location> locations;

  if (boat.GetOrientation() == Orientation::Vertical) {
    for (int index = 0; index < boat.GetSize(); ++index) {
      locations.emplace_back(Location(start_location.x, start_location.y + index));
    }
  } else {
    for (int index = 0; index < boat.GetSize(); ++index) {
      locations.emplace_back(Location(start_location.x + index, start_location.y));
    }
  }

  return locations;
}

void Board::SetWidth(const int value) {
  width = value;
}

void Board::SetHeight(const int value) {
  height = value;
}

bool Board::AddBoat(const Boat& boat, const Location start_location) {
  const std::vector<Location>& boat_target_locations = AllLocationsFor(boat, start_location);

  for (const Location location : boat_target_locations) {
    if (HasBoat(location) || !IsInRange(location)) {
      return false;
    }
  }

  for (const Location location : boat_target_locations) {
    boats.emplace(location, boat);
  }

  boat_name_to_location.emplace(boat.GetName(), start_location);

  return true;
}

int Board::GetWidth() const {
  return width;
}

int Board::GetHeight() const {
  return height;
}

std::optional<Boat> Board::GetBoat(const Location location) const {
  auto search = boats.find(location);

  if (search != boats.end()) {
    return search->second;
  }

  return std::nullopt;
}

int Board::PlacedBoatsCount() const {
  return boat_name_to_location.size();
}

bool Board::MoveBoat(const Boat& boat, const Location new_location) {
  const std::vector<Location> all_new_locations = AllLocationsFor(boat, new_location);

  for (const Location location : all_new_locations) {
    if (!IsInRange(location)) {
      return false;
    }
  }

  const Location& old_starting_location = boat_name_to_location.at(boat.GetName());

  for (const Location location : AllLocationsFor(boat, old_starting_location)) {
    boats.erase(location);
  }

  for (const Location location : all_new_locations) {
    boats.emplace(location, boat);
  }

  boat_name_to_location[boat.GetName()] = new_location;

  return true;
}

bool Board::Shoot(const Location location) {
  if (HasShot(location) || !IsInRange(location)) {
    return false;
  }

  shot_locations.emplace(location);
  return true;
}

bool Board::HasShot(const Location location) const {
  return shot_locations.find(location) != shot_locations.end();
}

bool Board::IsHit(const Location location) const {
  return HasShot(location) && HasBoat(location);
}

bool Board::IsInRange(const Location location) const {
  return (location.x > 0) && (location.y > 0) && (location.x <= width) && (location.y <= height);
}

bool Board::AreAllShipsSunk() const {
  return std::all_of(boats.begin(), boats.end(), [this](const auto& pair)
  {
    return IsHit(pair.first);
  });
}

bool Board::HasBoat(const Location location) const {
  return boats.find(location) != boats.end();
}
