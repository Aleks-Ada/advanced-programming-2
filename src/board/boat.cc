#include "boat.h"

Boat::Boat(BoatType type, Orientation orientation, Location location) : type(type),
                                                                        orientation(orientation),
                                                                        location(location) { }

BoatType Boat::GetType() const {
  return type;
}

Location Boat::GetLocation() const {
  return location;
}

Orientation Boat::GetOrientation() const {
  return orientation;
}

int Boat::GetSize() const {
  switch (type) {
    case BoatType::Carrier:
      return 5;
    case BoatType::Battleship:
      return 4;
    case BoatType::Destroyer:
      return 3;
    case BoatType::Submarine:
      return 3;
    case BoatType::PatrolBoat:
      return 2;
  }
}
