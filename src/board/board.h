#ifndef SRC_BOARD_BOARD_H
#define SRC_BOARD_BOARD_H

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

#include "boat.h"
#include "configuration/configuration.h"

class LetterIndex {
public:
  explicit LetterIndex(const std::string_view& value) : value(value) {}

  int ToInt() const {
    if (value.size() == 1) {
      char character = value[0];
      // Subtract ASCII alphabet to get pure character index in the alphabet
      return character - ('A' - 1);
    }

    throw std::runtime_error("Undefined letter index");
  }

private:
  std::string_view value;
};

struct Location {
  Location() : x(0), y(0) {};

  Location(const LetterIndex x, const int y) : Location(x.ToInt(), y) {};
  Location(const int x, const int y) : x(x), y(y) {};

  int x;
  int y;
};

#define BoardLetterIndex(columnLetter, row) Location(LetterIndex(#columnLetter), row)
#define BoardLocation(column, row) Location(LetterIndex(column), row)

class Board {
public:
  Board() : width(0), height(0) {}

  Board(const int width, const int height)
    : width(width), height(height) {}

  void SetWidth(const int value);
  void SetHeight(const int value);
  bool AddBoat(const Boat& boat, const Location start_location);
  bool MoveBoat(const Boat& boat, const Location new_location);
  bool Shoot(const Location location);

  int GetWidth() const;
  int GetHeight() const;
  int PlacedBoatsCount() const;
  std::optional<Boat> GetBoat(const Location location) const;
  bool HasShot(const Location location) const;
  bool IsHit(const Location location) const;
  bool AreAllShipsSunk() const;

private:
  bool IsInRange(const Location location) const;
  bool HasBoat(const Location location) const;

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
};

#endif // SRC_BOARD_BOARD_H
