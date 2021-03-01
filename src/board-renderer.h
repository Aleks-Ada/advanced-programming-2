#ifndef SRC_BOARD_RENDERER_H
#define SRC_BOARD_RENDERER_H

#include <string>
#include <utility>

#include "board/board.h"

class BoardRenderer {
public:
  explicit BoardRenderer(Board board) : board(std::move(board)) {}

  std::string Render() const;

private:
  Board board;
};

#endif // SRC_BOARD_RENDERER_H
