#include "path_tool.h"

void PathTool::Start(int x, int y, graphics::Image& image) {
  last_x_ = x;
  last_y_ = y;
}

void PathTool::MoveTo(int x, int y, graphics::Image& image) {
  last_x_ = x;
  last_y_ = y;
}
