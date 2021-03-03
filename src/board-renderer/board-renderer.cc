#include "board-renderer.h"

std::string CellSeparator() {
  return " ";
}

std::string CoordinateIndexToLetter(const int index) {
  char letter = 'A';
  letter += static_cast<char>(index); // {index} letters further down the alphabet from A
  return std::string() + letter;
}

std::string Pad(const std::string& string, const int requiredSize) {
  if (string.size() >= requiredSize) {
    return string;
  }

  const int paddingLettersRequired = requiredSize - string.size();
  return string + std::string(paddingLettersRequired, ' ');
}

std::string BoatTypeToString(const BoatType boatType) {
  switch (boatType) {
    case BoatType::Carrier:
      return "C";
    case BoatType::Battleship:
      return "B";
    case BoatType::Destroyer:
      return "D";
    case BoatType::Submarine:
      return "S";
    case BoatType::PatrolBoat:
      return "P";
  }
}

std::string BoardRenderer::Render() const {
  std::string render;

  const int width = board.GetWidth();
  const int height = board.GetHeight();

  const int maxRowIdentifierChars = std::to_string(height).size();
  render += std::string(maxRowIdentifierChars, ' ');

  for (int columnIndex = 0; columnIndex < width; ++columnIndex) {
    render += CellSeparator();
    render += CoordinateIndexToLetter(columnIndex);
  }

  render += "\n";

  for (int rowIndex = 1; rowIndex <= height; ++rowIndex) {
    render += Pad(std::to_string(rowIndex), maxRowIdentifierChars);

    for (int columnIndex = 0; columnIndex < width; ++columnIndex) {
      render += CellSeparator();

      std::optional<Boat> optionalBoat = board.GetBoat(Location(columnIndex, rowIndex));

      if (optionalBoat.has_value()) {
        render += BoatTypeToString(optionalBoat.value().GetType());
      } else {
        render += " ";
      }
    }

    render += "\n";
  }

  return render;
}
