#include <gtest/gtest.h>

#include "board/board.h"
#include "board/boat.h"
#include "board-renderer.h"

TEST(BoardRendererTest, SimpleRender) {
  Board board;
  board.SetWidth(10);
  board.SetHeight(10);
  board.AddBoat(Boat(BoatType::Carrier, Orientation::Vertical, BoardLetterIndex(A, 2)));
  board.AddBoat(Boat(BoatType::Destroyer, Orientation::Vertical, BoardLetterIndex(I, 2)));
  board.AddBoat(Boat(BoatType::Battleship, Orientation::Horizontal, BoardLetterIndex(E, 6)));
  board.AddBoat(Boat(BoatType::Submarine, Orientation::Horizontal, BoardLetterIndex(C, 9)));
  board.AddBoat(Boat(BoatType::PatrolBoat, Orientation::Vertical, BoardLetterIndex(I, 9)));

  std::string render = BoardRenderer(board).Render();

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
