#include "color_tool.h"

void ColorTool::SetColor(const graphics::Color& color) { color_ = color; }

graphics::Color ColorTool::GetColor() const { return color_; }
