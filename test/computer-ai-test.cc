#include <gtest/gtest.h>

#include <queue>

#include "computer-ai.h"
#include "board-renderer/board-renderer.h"

class CustomPlacementGenerator : public PlacementGenerator {
private:
  Board& board;
  std::queue<Location>& locations_to_choose;

public:
  CustomPlacementGenerator(Board& board, std::queue<Location>& locations)
    : board(board), locations_to_choose(locations) {}

  Orientation GenerateOrientation() override {
    return Orientation::Horizontal;
  }

  Location GenerateLocation(const int width, const int height) override {
    return Location();
  }

  Location ChooseLocation(const std::vector<Location>& choices) override {
    if (locations_to_choose.empty()) {
      return board.NotFiredLocations().front();
    }

    const Location location = locations_to_choose.front();
    locations_to_choose.pop();
    return location;
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
  std::queue<Location> locations_to_shoot;
  locations_to_shoot.push(BoardLetterIndex(F, 6));
  CustomPlacementGenerator placement_generator(board, locations_to_shoot);
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

TEST(ComputerAiTest, FullGame) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board_renderer.SetMode(TARGET);
  board.AddBoat(ShipType{ "Carrier", 5 }, BoardLetterIndex(F, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Battleship", 4 }, BoardLetterIndex(D, 7), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Patrol", 3 }, BoardLetterIndex(A, 1), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Small Patrol", 1 }, BoardLetterIndex(A, 1), Orientation::Horizontal);
  board.AddMine(BoardLetterIndex(F, 6));
  board.AddMine(BoardLetterIndex(E, 6));
  std::queue<Location> target_locations;
  target_locations.push(BoardLetterIndex(F, 6));
  target_locations.push(BoardLetterIndex(J, 1));
  target_locations.push(BoardLetterIndex(J, 10));
  target_locations.push(BoardLetterIndex(A, 10));
  target_locations.push(BoardLetterIndex(A, 1));
  CustomPlacementGenerator placement_generator(board, target_locations);
  ComputerAi computer_ai(board, placement_generator);

  while (!board.AreAllShipsSunk()) {
    EXPECT_TRUE(board.Shoot(computer_ai.ChooseNextShot()));

  }

  EXPECT_EQ("   A B C D E F G H I J\n"
            "1  ● ● ●     X       X\n"
            "2          X ● X      \n"
            "3          X ● X      \n"
            "4          X ● X      \n"
            "5        X X ● X      \n"
            "6        X X ● X      \n"
            "7      X ● ● ● ● X    \n"
            "8        X X X X      \n"
            "9                     \n"
            "10 X                 X\n", board_renderer.Render());
}