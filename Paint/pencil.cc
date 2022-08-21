#include "pencil.h"

void Pencil::Start(int x, int y, graphics::Image& image) {
  image.SetColor(x, y, GetColor());
  PathTool::Start(x, y, image);
}

void Pencil::MoveTo(int x, int y, graphics::Image& image) {
  image.DrawLine(GetX(), GetY(), x, y, GetColor());
  PathTool::Start(x, y, image);
}
