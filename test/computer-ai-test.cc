#include <gtest/gtest.h>

#include "computer-ai.h"
#include "board-renderer/board-renderer.h"

class CustomPlacementGenerator : public PlacementGenerator {
private:
  Board& board;
  Location first_shot;

public:
  CustomPlacementGenerator(
      Board& board,
      Location first_shot)
      : board(board), first_shot(first_shot) {}

  Orientation GenerateOrientation() override {
    return Orientation::Horizontal;
  }

  Location GenerateLocation(const int width, const int height) override {
    return Location();
  }

  Location ChooseLocation(const std::vector<Location> &choices) override {
    return first_shot;
  }
};

// Testing this scenario:
// https://www.datagenetics.com/blog/december32011/s029.png
TEST(ComputerAiTest, HuntMode) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board_renderer.SetMode(TARGET);
  board.AddBoat(ShipType{ "Carrier", 5 }, BoardLetterIndex(F, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Battleship", 4 }, BoardLetterIndex(D, 7), Orientation::Horizontal);
  Location first_shot = BoardLetterIndex(F, 6);
  CustomPlacementGenerator placement_generator(board, first_shot);
  ComputerAi computer_ai(board, placement_generator);

  for (int i = 0; i < 27; ++i) {
    board.Shoot(computer_ai.ChooseNextShot());
  }

  EXPECT_EQ("   A B C D E F G H I J\n"
            "1            X        \n"
            "2          X ● X      \n"
            "3          X ● X      \n"
            "4          X ● X      \n"
            "5          X ● X      \n"
            "6        X X ● X      \n"
            "7      X ● ● ● ● X    \n"
            "8        X X X X      \n"
            "9                     \n"
            "10                    \n", board_renderer.Render());
}