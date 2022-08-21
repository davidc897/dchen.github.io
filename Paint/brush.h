#include "color_tool.h"
#include "cpputils/graphics/image.h"
#include "path_tool.h"

#ifndef BRUSH_H
#define BRUSH_H

class Brush : public ColorTool, public PathTool {
 public:
  Brush() = default;
  ~Brush() = default;

  // Create a circle at (x, y).
  void Start(int x, int y, graphics::Image& image) override;

  // Thick line from previous (x, y) to this (x, y).
  void MoveTo(int x, int y, graphics::Image& image) override;

  // Change the thickness of the brush.
  void SetWidth(int width);

 private:
  int width_ = 10;
};

#endif  // BRUSH_H
