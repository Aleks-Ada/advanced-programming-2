#include "auto-placer.h"

bool AutoPlacer::AutoPlace(const std::vector<ShipType>& boats) {
  Board test_board(board);

  for (int index = 0; index < 10000; ++index) {
    bool success = false;

    for (const ShipType& ship_type : boats) {
      success = test_board.AddBoat(
          ship_type,
          placement_generator.GenerateLocation(test_board.GetWidth(), test_board.GetHeight()),
          placement_generator.GenerateOrientation());

      if (!success) {
        test_board = Board(board);
        break;
      }
    }

    if (success) {
      board = test_board;
      return true;
    }
  }

  board = test_board;
  return false;
}
