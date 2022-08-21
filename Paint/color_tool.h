#include "cpputils/graphics/image.h"

#ifndef COLOR_TOOL_H
#define COLOR_TOOL_H

// A class to track the color of a tool, for tools that only have one color
// at a time.
class ColorTool {
 public:
  // Change the color.
  void SetColor(const graphics::Color& color);

  // Get the color.
  virtual graphics::Color GetColor() const;

 private:
  graphics::Color color_ = graphics::Color(0, 0, 0);
};

#endif  // COLOR_TOOL_H
