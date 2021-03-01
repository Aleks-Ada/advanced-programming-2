#ifndef SRC_BOARD_BOARD_H
#define SRC_BOARD_BOARD_H

#include <optional>
#include <unordered_map>
#include <vector>

#include "boat.h"

class Board {
public:
  Board() : width(0), height(0), boats() {}

  void SetWidth(const int value);
  void SetHeight(const int value);
  void AddBoat(const Boat& boat);

  int GetWidth() const;
  int GetHeight() const;
  std::optional<Boat> GetBoat(const Location location) const;

private:
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

  int width;
  int height;
  std::unordered_map<Location, Boat, LocationHasher, LocationEquals> boats;
};

#endif // SRC_BOARD_BOARD_H
