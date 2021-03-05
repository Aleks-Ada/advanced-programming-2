#include <gtest/gtest.h>

#include "board-renderer/board-renderer.h"

TEST(BoardRendererTest, SelfBoardRender) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board.AddBoat(Boat(ShipType{ "Carrier", 5 }, Orientation::Vertical), BoardLetterIndex(A, 2));
  board.AddBoat(Boat(ShipType{ "Destroyer", 3 }, Orientation::Vertical), BoardLetterIndex(I, 2));
  board.AddBoat(Boat(ShipType{ "Battleship", 4 }, Orientation::Horizontal), BoardLetterIndex(E, 6));
  board.AddBoat(Boat(ShipType{ "Submarine", 3 }, Orientation::Horizontal), BoardLetterIndex(C, 9));
  board.AddBoat(Boat(ShipType{ "Patrol Boat", 2 }, Orientation::Vertical), BoardLetterIndex(I, 9));

  const std::string render = board_renderer.Render();

  EXPECT_EQ("   A B C D E F G H I J\n"
            "1                     \n"
            "2  C               D  \n"
            "3  C               D  \n"
            "4  C               D  \n"
            "5  C                  \n"
            "6  C       B B B B    \n"
            "7                     \n"
            "8                     \n"
            "9      S S S       P  \n"
            "10                 P  \n", render);
}

TEST(BoardRendererTest, TargetBoardRender) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board_renderer.SetMode(TARGET);
  board.AddBoat(Boat(ShipType{ "Carrier", 5 }, Orientation::Vertical), BoardLetterIndex(A, 2));
  board.AddBoat(Boat(ShipType{ "Destroyer", 3 }, Orientation::Vertical), BoardLetterIndex(I, 2));
  board.AddBoat(Boat(ShipType{ "Battleship", 4 }, Orientation::Horizontal), BoardLetterIndex(E, 6));
  board.AddBoat(Boat(ShipType{ "Submarine", 3 }, Orientation::Horizontal), BoardLetterIndex(C, 9));
  board.AddBoat(Boat(ShipType{ "Patrol Boat", 2 }, Orientation::Vertical), BoardLetterIndex(I, 9));
  board.Shoot(BoardLetterIndex(A, 1));
  board.Shoot(BoardLetterIndex(A, 2));
  board.Shoot(BoardLetterIndex(I, 2));
  board.Shoot(BoardLetterIndex(E, 6));
  board.Shoot(BoardLetterIndex(C, 9));
  board.Shoot(BoardLetterIndex(I, 9));

  const std::string render = board_renderer.Render();

  EXPECT_EQ("   A B C D E F G H I J\n"
            "1  X                  \n"
            "2  ●               ●  \n"
            "3                     \n"
            "4                     \n"
            "5                     \n"
            "6          ●          \n"
            "7                     \n"
            "8                     \n"
            "9      ●           ●  \n"
            "10                    \n", render);
}
