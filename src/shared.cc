#include "shared.h"

char IntToChar(const int value) {
  char letter = 'A';
  letter += static_cast<char>(value); // {value} letters further down the alphabet from A
  return letter;
}

std::string CoordinateIndexToLetter(const int index) {
  if (index > 25) {
    const int character1_value = index / 26;
    const int character2_value = index % 26;

    return std::string() + IntToChar(character1_value - 1) + IntToChar(character2_value);
  }

  return std::string() + IntToChar(index);
}

std::string CoordinateToLetter(const int coordinate) {
  return CoordinateIndexToLetter(coordinate - 1);
}
