#ifndef SRC_BOARD_BOARD_H
#define SRC_BOARD_BOARD_H

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

#include "configuration/configuration.h"

class LetterIndex {
public:
  explicit LetterIndex(const std::string_view& value) : value(value) {}

  int ToInt() const;

private:
  std::string_view value;
};

struct Location {
  Location() : x(0), y(0) {};

  Location(const LetterIndex x, const int y) : Location(x.ToInt(), y) {};
  Location(const int x, const int y) : x(x), y(y) {};

  bool operator==(const Location rhs) const {
    return (rhs.x == x) && (rhs.y == y);
  }

  int x;
  int y;
};

#define BoardLetterIndex(columnLetter, row) Location(LetterIndex(#columnLetter), row)
#define BoardLocation(column, row) Location(LetterIndex(column), row)

enum class Orientation {
  Horizontal,
  Vertical
};

class Boat {
public:
  Boat(ShipType type, Orientation orientation)
      : type(std::move(type)),
        orientation(orientation) {}

  bool operator==(const Boat& rhs) const;

  const std::string& GetName() const;
  int GetSize() const;
  Orientation GetOrientation() const;
  ShipType GetShipType() const;

private:
  ShipType type;
  Orientation orientation;
};

class Board {
public:
  Board(const int width, const int height)
    : width(width), height(height) {}

  bool AddBoat(const ShipType& ship, const Location start_location, const Orientation orientation);
  bool MoveBoat(const ShipType& ship, const Location new_location, const Orientation new_orientation);
  bool Shoot(const Location location);
  void Reset();
  void AddMine(const Location location);
  void AddRandomMines(class PlacementGenerator& placement_generator);

  int GetWidth() const;
  int GetHeight() const;
  int PlacedBoatsCount() const;
  std::optional<Boat> GetBoat(const Location location) const;
  bool HasShot(const Location location) const;
  bool IsHit(const Location location) const;
  bool AreAllShipsSunk() const;
  bool IsMine(const Location location) const;
  std::vector<Location> NotFiredLocations() const;
  std::vector<ShipType> GetRemainingShips() const;

private:
  bool IsInRange(const Location location) const;
  bool HasBoat(const Location location) const;
  bool HasBeenKilled(const ShipType& ship_type) const;

  class LocationHasher {
  public:
    std::size_t operator()(const Location location) const {
      std::size_t xHash = std::hash<int>()(location.x);
      std::size_t yHash = std::hash<int>()(location.y);
      return xHash ^ (yHash << 1);
    }
  };

  class LocationEquals {
  public:
    bool operator()(const Location lhs, const Location rhs) const {
      return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }
  };

  class LocationLessThan {
  public:
    bool operator()(const Location lhs, const Location rhs) const {
      if (lhs.x != rhs.x) {
        return lhs.x < rhs.x;
      }

      return lhs.y < rhs.y;
    }
  };

  int width;
  int height;
  std::unordered_map<std::string, Location> boat_name_to_location;
  std::unordered_map<Location, Boat, LocationHasher, LocationEquals> boats;
  std::set<Location, LocationLessThan> shot_locations;
  std::set<Location, LocationLessThan> mine_locations;
};

#endif // SRC_BOARD_BOARD_H
