#include <memory>
#include "board-renderer.h"

void BoardRenderer::SetMode(const RenderMode render_mode) {
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

std::string_view HitMarker() {
  return "●";
}

std::string_view MissMarker() {
  return "X";
}

std::string_view MineMarker() {
  return "M";
}

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

std::string Pad(const std::string& string, const int required_size) {
  int actual_size = string.size();

  if (string == "●") { // unicode wide strings converted to narrow strings have extra chars
    actual_size = 1;
  }

  if (actual_size >= required_size) {
    return string;
  }

  const int padding_letters_required = required_size - actual_size;
  return string + std::string(padding_letters_required, ' ');
}

std::string BoatToString(const Boat& boat) {
  return std::string() + boat.GetName().at(0);
}

bool BoardRenderer::ShouldRenderWide(const int column) const {
  if (render_mode != SELF) {
    return false;
  }

  for (int row = 1; row <= board.GetHeight(); ++row) {
    const Location location(column, row);

    const bool is_wide_cell = board.IsMine(location) && board.GetBoat(location).has_value();
    const bool will_render_wide = !board.IsHit(location);

    if (is_wide_cell && will_render_wide) {
      return true;
    }
  }

  return false;
}

std::string BoardRenderer::Render() const {
  const int width = board.GetWidth();
  const int height = board.GetHeight();

  const int max_column_identifier_chars = CoordinateIndexToLetter(board.GetWidth()).size();
  const int max_row_identifier_chars = std::to_string(height).size();

  auto rows = std::make_unique<std::string[]>(height + 1);

  // Column 0, Row 0
  rows[0] += std::string(max_row_identifier_chars, ' ');

  // Column 0
  for (int row = 1; row <= height; ++row) {
    rows[row] += Pad(std::to_string(row), max_row_identifier_chars);
  }

  for (int column = 1; column <= width; ++column) {
    constexpr static int wide_render_chars = 3;
    const int max_render_width = ShouldRenderWide(column) ?
        std::max(max_column_identifier_chars, wide_render_chars) :
        max_column_identifier_chars;

    // Row 0
    rows[0] += CellSeparator();
    rows[0] += Pad(CoordinateIndexToLetter(column - 1), max_render_width);

    for (int row = 1; row <= height; ++row) {
      const Location location(column, row);

      std::string cell_marker;

      if (board.HasShot(location)) {
        if (board.IsHit(location)) {
          cell_marker = HitMarker();
        } else {
          cell_marker = MissMarker();
        }
      } else if (render_mode == SELF) {
        std::optional<Boat> boat = board.GetBoat(location);

        if (boat.has_value()) {
          cell_marker = BoatToString(boat.value());

          if (board.IsMine(location)) {
            cell_marker += "+";
            cell_marker += MineMarker();
          }
        } else if (board.IsMine(location)) {
          cell_marker = MineMarker();
        } else {
          cell_marker = BlankCell();
        }
      } else {
        cell_marker = BlankCell();
      }

      rows[row] += CellSeparator();
      rows[row] += Pad(cell_marker, max_render_width);
    }
  }

  std::string render;

  for (int row = 0; row <= height; ++row) {
    render += rows[row];
    render += "\n";
  }

  return render;
}
