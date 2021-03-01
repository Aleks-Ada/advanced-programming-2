#ifndef SRC_BOARD_BOAT_H
#define SRC_BOARD_BOAT_H

#include <string>
#include <stdexcept>

enum class BoatType {
  Carrier,
  Battleship,
  Destroyer,
  Submarine,
  PatrolBoat
};

enum class Orientation {
  Horizontal,
  Vertical
};

class LetterIndex {
public:
  explicit LetterIndex(const std::string_view& value) : value(value) {}

  int ToInt() const {
    if (value.size() == 1) {
      char character = value[0];
      // Subtract ASCII alphabet to get pure character index in the alphabet
      return character - 65;
    }

    throw std::runtime_error("Undefined letter index");
  }

private:
  std::string_view value;
};

class Location {
public:
  Location(const LetterIndex x, const int y) : Location(x.ToInt(), y) {};
  Location(const int x, const int y) : x(x), y(y) {};

public:
  int x;
  int y;
};

#define BoardLetterIndex(rowLetter, column) Location(LetterIndex(#rowLetter), column)

class Boat {
public:
  Boat(BoatType type, Orientation orientation, Location location);

  BoatType GetType() const;
  Location GetLocation() const;
  Orientation GetOrientation() const;
  int GetSize() const;

private:
  BoatType type;
  Orientation orientation;
  Location location;
};

#endif // SRC_BOARD_BOAT_H
