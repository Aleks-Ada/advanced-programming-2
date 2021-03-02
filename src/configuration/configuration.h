#ifndef SRC_CONFIGURATION_CONFIGURATION_H
#define SRC_CONFIGURATION_CONFIGURATION_H

#include <string>
#include <vector>

struct ShipType {
  std::string name;
  int size;

  bool operator==(const ShipType& shipType) const {
    return (shipType.size == size) && (shipType.name == name);
  }
};

struct Configuration {
  int board_width;
  int board_height;

  std::vector<ShipType> ship_types;
};


#endif // SRC_CONFIGURATION_CONFIGURATION_H
