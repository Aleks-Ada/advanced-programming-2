#ifndef SRC_BOARD_RENDERER_H
#define SRC_BOARD_RENDERER_H

#include <string>
#include <utility>

#include "board/board.h"

enum RenderMode {
  SELF,
  TARGET
};

class BoardRenderer {
public:
  explicit BoardRenderer(const Board& board) : board(board), render_mode(SELF) {}

  void SetMode(const RenderMode render_mode);
  std::string Render() const;

private:
  bool ShouldRenderWide(const int column) const;

  const Board& board;
  RenderMode render_mode;
};

#endif // SRC_BOARD_RENDERER_H
