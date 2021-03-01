#include "board.h"

void Board::SetWidth(const int value) {
  width = value;
}

void Board::SetHeight(const int value) {
  height = value;
}

void Board::AddBoat(const Boat& boat) {
  Location start = boat.GetLocation();

  if (boat.GetOrientation() == Orientation::Vertical) {
    for (int index = 0; index < boat.GetSize(); ++index) {
      boats.emplace(Location(start.x, start.y + index), boat);
    }
  } else {
    for (int index = 0; index < boat.GetSize(); ++index) {
      boats.emplace(Location(start.x + index, start.y), boat);
    }
  }
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
