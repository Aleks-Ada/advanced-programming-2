#include "board-renderer.h"

void BoardRenderer::SetMode(RenderMode render_mode) {
  this->render_mode = render_mode;
}

std::string_view NewLine() {
  return "\n";
}

std::string_view CellSeparator() {
  return " ";
}

std::string_view BlankCell() {
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

std::string BoatToString(const Boat& boat) {
  return std::string() + boat.GetName().at(0);
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

  render += NewLine();

  for (int rowIndex = 1; rowIndex <= height; ++rowIndex) {
    render += Pad(std::to_string(rowIndex), maxRowIdentifierChars);

    for (int columnIndex = 1; columnIndex <= width; ++columnIndex) {
      render += CellSeparator();

      const Location location(columnIndex, rowIndex);

      if (render_mode == SELF) {
        std::optional<Boat> optionalBoat = board.GetBoat(location);

        if (optionalBoat.has_value()) {
          render += BoatToString(optionalBoat.value());
        } else {
          render += BlankCell();
        }
      } else if (render_mode == TARGET) {
        if (board.HasShot(location)) {
          if (board.IsHit(location)) {
            render += "●";
          } else {
            render += "X";
          }
        } else {
          render += BlankCell();
        }
      }
    }

    render += NewLine();
  }

  return render;
}
