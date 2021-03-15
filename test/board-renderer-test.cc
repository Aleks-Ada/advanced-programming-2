#include <gtest/gtest.h>

#include "board-renderer/board-renderer.h"

TEST(BoardRendererTest, SelfBoardRender) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board.AddBoat(ShipType{ "Carrier", 5 }, BoardLetterIndex(A, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Destroyer", 3 }, BoardLetterIndex(I, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Battleship", 4 }, BoardLetterIndex(E, 6), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Submarine", 3 }, BoardLetterIndex(C, 9), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Patrol Boat", 2 }, BoardLetterIndex(I, 9), Orientation::Vertical);

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

TEST(BoardRendererTest, SelfBoardRenderWithMines) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board.AddBoat(ShipType{ "Carrier", 5 }, BoardLetterIndex(A, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Destroyer", 3 }, BoardLetterIndex(I, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Battleship", 4 }, BoardLetterIndex(E, 6), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Submarine", 3 }, BoardLetterIndex(C, 9), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Patrol Boat", 2 }, BoardLetterIndex(I, 9), Orientation::Vertical);
  board.AddMine(BoardLetterIndex(A, 2));
  board.AddMine(BoardLetterIndex(E, 2));
  board.AddMine(BoardLetterIndex(I, 2));
  board.AddMine(BoardLetterIndex(I, 3));
  board.AddMine(BoardLetterIndex(I, 4));
  board.AddMine(BoardLetterIndex(I, 5));
  board.AddMine(BoardLetterIndex(G, 8));
  board.AddMine(BoardLetterIndex(H, 9));

  const std::string render = board_renderer.Render();

  EXPECT_EQ("   A   B C D E F G H I   J\n"
            "1                         \n"
            "2  C+M       M       D+M  \n"
            "3  C                 D+M  \n"
            "4  C                 D+M  \n"
            "5  C                 M    \n"
            "6  C         B B B B      \n"
            "7                         \n"
            "8                M        \n"
            "9        S S S     M P    \n"
            "10                   P    \n", render);
}

TEST(BoardRendererTest, TargetBoardRenderMineHit) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board_renderer.SetMode(TARGET);
  board.AddBoat(ShipType{ "Carrier", 5 }, BoardLetterIndex(A, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Destroyer", 3 }, BoardLetterIndex(I, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Battleship", 4 }, BoardLetterIndex(E, 6), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Submarine", 3 }, BoardLetterIndex(C, 9), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Patrol Boat", 2 }, BoardLetterIndex(I, 9), Orientation::Vertical);
  board.AddMine(BoardLetterIndex(A, 2));
  board.AddMine(BoardLetterIndex(E, 2));
  board.AddMine(BoardLetterIndex(I, 2));
  board.AddMine(BoardLetterIndex(I, 3));
  board.AddMine(BoardLetterIndex(I, 4));
  board.AddMine(BoardLetterIndex(I, 5));
  board.AddMine(BoardLetterIndex(G, 8));
  board.AddMine(BoardLetterIndex(H, 9));
  board.Shoot(BoardLetterIndex(A, 2));
  board.Shoot(BoardLetterIndex(E, 2));
  board.Shoot(BoardLetterIndex(I, 2));

  const std::string render = board_renderer.Render();

  EXPECT_EQ("   A B C D E F G H I J\n"
            "1  X X   X X X   X X X\n"
            "2  ● X   X X X   X ● X\n"
            "3  ● X   X X X   X ● X\n"
            "4                X ● X\n"
            "5                X X X\n"
            "6                ● X X\n"
            "7                     \n"
            "8                     \n"
            "9                     \n"
            "10                    \n", render);
}

TEST(BoardRendererTest, TargetBoardRender) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board_renderer.SetMode(TARGET);
  board.AddBoat(ShipType{ "Carrier", 5 }, BoardLetterIndex(A, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Destroyer", 3 }, BoardLetterIndex(I, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Battleship", 4 }, BoardLetterIndex(E, 6), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Submarine", 3 }, BoardLetterIndex(C, 9), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Patrol Boat", 2 }, BoardLetterIndex(I, 9), Orientation::Vertical);
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

TEST(BoardRendererTest, SelfBoardRenderWithHits) {
  Board board(10, 10);
  BoardRenderer board_renderer(board);
  board.AddBoat(ShipType{ "Carrier", 5 }, BoardLetterIndex(A, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Destroyer", 3 }, BoardLetterIndex(I, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Battleship", 4 }, BoardLetterIndex(E, 6), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Submarine", 3 }, BoardLetterIndex(C, 9), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Patrol Boat", 2 }, BoardLetterIndex(I, 9), Orientation::Vertical);
  board.Shoot(BoardLetterIndex(A, 1));
  board.Shoot(BoardLetterIndex(A, 2));

  const std::string render = board_renderer.Render();

  EXPECT_EQ("   A B C D E F G H I J\n"
            "1  X                  \n"
            "2  ●               D  \n"
            "3  C               D  \n"
            "4  C               D  \n"
            "5  C                  \n"
            "6  C       B B B B    \n"
            "7                     \n"
            "8                     \n"
            "9      S S S       P  \n"
            "10                 P  \n", render);
}

TEST(BoardRendererTest, MaxBoardSize) {
  Board board(80, 80);
  BoardRenderer board_renderer(board);
  board.AddBoat(ShipType{ "Carrier", 5 }, BoardLetterIndex(CB, 2), Orientation::Vertical);
  board.AddBoat(ShipType{ "Destroyer", 3 }, BoardLetterIndex(CA, 1), Orientation::Vertical);
  board.AddBoat(ShipType{ "Battleship", 4 }, BoardLetterIndex(BD, 10), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Submarine", 3 }, BoardLetterIndex(BD, 11), Orientation::Horizontal);
  board.AddBoat(ShipType{ "Patrol Boat", 2 }, BoardLetterIndex(A, 1), Orientation::Vertical);
  board.Shoot(BoardLetterIndex(BF, 10));
  board.Shoot(BoardLetterIndex(CB, 10));
  board.AddMine(BoardLetterIndex(A, 1));
  board.AddMine(BoardLetterIndex(CA, 1));

  const std::string render = board_renderer.Render();

  EXPECT_EQ("   A   B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  AA AB AC AD AE AF AG AH AI AJ AK AL AM AN AO AP AQ AR AS AT AU AV AW AX AY AZ BA BB BC BD BE BF BG BH BI BJ BK BL BM BN BO BP BQ BR BS BT BU BV BW BX BY BZ CA  CB\n"
            "1  P+M                                                                                                                                                                                                                                        D+M   \n"
            "2  P                                                                                                                                                                                                                                          D   C \n"
            "3                                                                                                                                                                                                                                             D   C \n"
            "4                                                                                                                                                                                                                                                 C \n"
            "5                                                                                                                                                                                                                                                 C \n"
            "6                                                                                                                                                                                                                                                 C \n"
            "7                                                                                                                                                                                                                                                   \n"
            "8                                                                                                                                                                                                                                                   \n"
            "9                                                                                                                                                                                                                                                   \n"
            "10                                                                                                                                                                       B  B  ●  B                                                               X \n"
            "11                                                                                                                                                                       S  S  S                                                                    \n"
            "12                                                                                                                                                                                                                                                  \n"
            "13                                                                                                                                                                                                                                                  \n"
            "14                                                                                                                                                                                                                                                  \n"
            "15                                                                                                                                                                                                                                                  \n"
            "16                                                                                                                                                                                                                                                  \n"
            "17                                                                                                                                                                                                                                                  \n"
            "18                                                                                                                                                                                                                                                  \n"
            "19                                                                                                                                                                                                                                                  \n"
            "20                                                                                                                                                                                                                                                  \n"
            "21                                                                                                                                                                                                                                                  \n"
            "22                                                                                                                                                                                                                                                  \n"
            "23                                                                                                                                                                                                                                                  \n"
            "24                                                                                                                                                                                                                                                  \n"
            "25                                                                                                                                                                                                                                                  \n"
            "26                                                                                                                                                                                                                                                  \n"
            "27                                                                                                                                                                                                                                                  \n"
            "28                                                                                                                                                                                                                                                  \n"
            "29                                                                                                                                                                                                                                                  \n"
            "30                                                                                                                                                                                                                                                  \n"
            "31                                                                                                                                                                                                                                                  \n"
            "32                                                                                                                                                                                                                                                  \n"
            "33                                                                                                                                                                                                                                                  \n"
            "34                                                                                                                                                                                                                                                  \n"
            "35                                                                                                                                                                                                                                                  \n"
            "36                                                                                                                                                                                                                                                  \n"
            "37                                                                                                                                                                                                                                                  \n"
            "38                                                                                                                                                                                                                                                  \n"
            "39                                                                                                                                                                                                                                                  \n"
            "40                                                                                                                                                                                                                                                  \n"
            "41                                                                                                                                                                                                                                                  \n"
            "42                                                                                                                                                                                                                                                  \n"
            "43                                                                                                                                                                                                                                                  \n"
            "44                                                                                                                                                                                                                                                  \n"
            "45                                                                                                                                                                                                                                                  \n"
            "46                                                                                                                                                                                                                                                  \n"
            "47                                                                                                                                                                                                                                                  \n"
            "48                                                                                                                                                                                                                                                  \n"
            "49                                                                                                                                                                                                                                                  \n"
            "50                                                                                                                                                                                                                                                  \n"
            "51                                                                                                                                                                                                                                                  \n"
            "52                                                                                                                                                                                                                                                  \n"
            "53                                                                                                                                                                                                                                                  \n"
            "54                                                                                                                                                                                                                                                  \n"
            "55                                                                                                                                                                                                                                                  \n"
            "56                                                                                                                                                                                                                                                  \n"
            "57                                                                                                                                                                                                                                                  \n"
            "58                                                                                                                                                                                                                                                  \n"
            "59                                                                                                                                                                                                                                                  \n"
            "60                                                                                                                                                                                                                                                  \n"
            "61                                                                                                                                                                                                                                                  \n"
            "62                                                                                                                                                                                                                                                  \n"
            "63                                                                                                                                                                                                                                                  \n"
            "64                                                                                                                                                                                                                                                  \n"
            "65                                                                                                                                                                                                                                                  \n"
            "66                                                                                                                                                                                                                                                  \n"
            "67                                                                                                                                                                                                                                                  \n"
            "68                                                                                                                                                                                                                                                  \n"
            "69                                                                                                                                                                                                                                                  \n"
            "70                                                                                                                                                                                                                                                  \n"
            "71                                                                                                                                                                                                                                                  \n"
            "72                                                                                                                                                                                                                                                  \n"
            "73                                                                                                                                                                                                                                                  \n"
            "74                                                                                                                                                                                                                                                  \n"
            "75                                                                                                                                                                                                                                                  \n"
            "76                                                                                                                                                                                                                                                  \n"
            "77                                                                                                                                                                                                                                                  \n"
            "78                                                                                                                                                                                                                                                  \n"
            "79                                                                                                                                                                                                                                                  \n"
            "80                                                                                                                                                                                                                                                  \n"
            , render);
}
