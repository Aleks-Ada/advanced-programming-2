#include "boat.h"

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
