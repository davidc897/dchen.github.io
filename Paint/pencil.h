#include "color_tool.h"
#include "cpputils/graphics/image.h"
#include "path_tool.h"

#ifndef PENCIL_H
#define PENCIL_H

class Pencil : public ColorTool, public PathTool {
 public:
  Pencil() = default;
  ~Pencil() = default;

  // Create a single colored dot (1 pixel) at (x, y).
  void Start(int x, int y, graphics::Image& image) override;

  // Draw a 1px line from prevous (x, y) to this (x, y).
  void MoveTo(int x, int y, graphics::Image& image) override;
};

#endif  // PENCIL_H
