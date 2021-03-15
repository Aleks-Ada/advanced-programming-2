#include "board.h"

#include <algorithm>

#include "placement-generator.h"

char VerifyInRange(const char character) {
  if ((character < 'A') || (character > 'Z')) {
    throw std::runtime_error("Letter index out of range");
  }

  return character;
}

int Base26Value(const char character)  {
  // Subtract ASCII alphabet to get pure character index in the alphabet
  return character - ('A' - 1);
}

int LetterIndex::ToInt() const {
  const int size = value.size();

  if (size == 0) {
    throw std::runtime_error("Letter index empty");
  }

  if (size > 2) {
    throw std::runtime_error("Letter index undefined");
  }

  const int character1_value = Base26Value(VerifyInRange(value[0]));

  if (size == 1) {
    return character1_value;
  }

  const int character2_value = Base26Value(VerifyInRange(value[1]));

  const int total_value = (26 * character1_value) + character2_value;

  if (total_value > 80)  {
    throw std::runtime_error("Letter index value too big");
  }

  return total_value;
}

bool Boat::operator==(const Boat& rhs) const {
  return (orientation == rhs.orientation) && (type == rhs.type);
}

const std::string& Boat::GetName() const {
  return type.name;
}

int Boat::GetSize() const {
  return type.size;
}

Orientation Boat::GetOrientation() const {
  return orientation;
}

ShipType Boat::GetShipType() const {
  return type;
}

std::vector<Location> AllLocationsFor(const ShipType& ship, const Location start_location,
                                      const Orientation orientation) {
  std::vector<Location> locations;

  if (orientation == Orientation::Vertical) {
    for (int index = 0; index < ship.size; ++index) {
      locations.emplace_back(Location(start_location.x, start_location.y + index));
    }
  } else {
    for (int index = 0; index < ship.size; ++index) {
      locations.emplace_back(Location(start_location.x + index, start_location.y));
    }
  }

  return locations;
}

bool Board::AddBoat(const ShipType& ship, const Location start_location,
                    const Orientation orientation) {
  const std::vector<Location>& boat_target_locations = AllLocationsFor(ship, start_location,
                                                                       orientation);

  for (const Location location : boat_target_locations) {
    if (HasBoat(location) || !IsInRange(location)) {
      return false;
    }
  }

  for (const Location location : boat_target_locations) {
    boats.emplace(location, Boat(ship, orientation));
  }

  boat_name_to_location.emplace(ship.name, start_location);

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

bool Board::MoveBoat(const ShipType& ship, const Location new_location,
                     const Orientation new_orientation) {
  const std::vector<Location> all_new_locations = AllLocationsFor(ship, new_location,
                                                                  new_orientation);

  for (const Location location : all_new_locations) {
    if (!IsInRange(location)) {
      return false;
    }
  }

  if (boat_name_to_location.find(ship.name) == boat_name_to_location.end()) {
    return false;
  }

  const Location& old_starting_location = boat_name_to_location.at(ship.name);
  const Boat& boat = boats.at(old_starting_location);

  for (const Location location : AllLocationsFor(ship, old_starting_location,
                                                 boat.GetOrientation())) {
    boats.erase(location);
  }

  boat_name_to_location.erase(ship.name);

  return AddBoat(ship, new_location, new_orientation);
}

bool Board::Shoot(const Location location) {
  if (HasShot(location) || !IsInRange(location)) {
    return false;
  }

  shot_locations.emplace(location);

  if (IsMine(location)) {
    const Location above(location.x, location.y - 1);
    const Location below(location.x, location.y + 1);
    const Location left(location.x - 1, location.y);
    const Location right(location.x +  1, location.y);
    const Location above_left(location.x - 1, location.y - 1);
    const Location above_right(location.x + 1, location.y - 1);
    const Location below_left(location.x - 1, location.y + 1);
    const Location below_right(location.x + 1, location.y + 1);

    Shoot(above);
    Shoot(below);
    Shoot(left);
    Shoot(right);
    Shoot(above_left);
    Shoot(above_right);
    Shoot(below_left);
    Shoot(below_right);
  }

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

void Board::Reset() {
  boats.clear();
  boat_name_to_location.clear();
}

std::vector<Location> Board::NotFiredLocations() const {
  std::vector<Location> locations;

  for (int x = 1; x <= width; ++x) {
    for (int y = 1; y <= height; ++y) {
      Location location(x, y);

      if (shot_locations.find(location) == shot_locations.end()) {
        locations.emplace_back(location);
      }
    }
  }

  return locations;
}

bool Board::HasBeenKilled(const ShipType& ship_type) const {
  const Location initial_location = boat_name_to_location.at(ship_type.name);
  const Orientation orientation = boats.at(initial_location).GetOrientation();

  for (const Location location : AllLocationsFor(ship_type, initial_location, orientation)) {
    if (!IsHit(location)) {
      return  false;
    }
  }

  return true;
}

std::vector<ShipType> Board::GetRemainingShips() const {
  std::set<ShipType> remaining_ships;

  for (const auto& pair : boats) {
    remaining_ships.emplace(pair.second.GetShipType());
  }

  for (auto current = remaining_ships.begin(); current != remaining_ships.end(); ) {
    if (HasBeenKilled(*current)) {
      current = remaining_ships.erase(current);
    } else {
      ++current;
    }
  }

  return std::vector<ShipType>(remaining_ships.begin(), remaining_ships.end());
}

void Board::AddMine(const Location location) {
  mine_locations.emplace(location);
}

bool Board::IsMine(const Location location) const {
  return mine_locations.find(location) != mine_locations.end();
}

void Board::AddRandomMines(PlacementGenerator& placement_generator) {
  for (int i = 0; i < 5; ++i) {
    const Location location = placement_generator.GenerateLocation(width, height);

    if (IsMine(location)) {
      --i;
    } else {
      AddMine(location);
    }
  }
}
