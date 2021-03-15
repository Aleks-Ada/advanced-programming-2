#ifndef SRC_CONFIGURATION_CONFIGURATION_H
#define SRC_CONFIGURATION_CONFIGURATION_H

#include <string>
#include <vector>

struct ShipType {
  std::string name;
  int size;

  bool operator==(const ShipType& ship_type) const {
    return (ship_type.size == size) && (ship_type.name == name);
  }

  bool operator<(const ShipType& ship_type) const {
    if (ship_type.size == size) {
      return name < ship_type.name;
    }

    return size < ship_type.size;
  }
};

struct Configuration {
  int board_width;
  int board_height;

  std::vector<ShipType> ship_types;
};

#endif // SRC_CONFIGURATION_CONFIGURATION_H
