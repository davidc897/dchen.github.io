#include "brush.h"

void Brush::Start(int x, int y, graphics::Image& image) {
  image.DrawCircle(x, y, width_ / 2, GetColor());
  PathTool::Start(x, y, image);
}

void Brush::MoveTo(int x, int y, graphics::Image& image) {
  image.DrawLine(GetX(), GetY(), x, y, GetColor(), width_);
  image.DrawCircle(x, y, width_ / 2, GetColor());
  PathTool::Start(x, y, image);
}

void Brush::SetWidth(int width) { width_ = width; }
