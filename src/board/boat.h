#ifndef SRC_BOARD_BOAT_H
#define SRC_BOARD_BOAT_H

#include <string>
#include <stdexcept>
#include <utility>

#include "configuration/configuration.h"

enum class Orientation {
  Horizontal,
  Vertical
};

class Boat {
public:
  Boat(ShipType type, const Orientation orientation)
    : type(std::move(type)),
      orientation(orientation) {}

  bool operator==(const Boat& rhs) const;

  const std::string& GetName() const;
  int GetSize() const;
  Orientation GetOrientation() const;

private:
  ShipType type;
  Orientation orientation;
};

#endif // SRC_BOARD_BOAT_H
