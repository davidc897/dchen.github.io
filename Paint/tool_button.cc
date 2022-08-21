#include "tool_button.h"

const int kFontSize = 18;

ToolButton::ToolButton(int x, int y, int width, int height,
                       ButtonListener* listener, ToolType type)
    : Button(x, y, width, height, listener), type_(type) {
  switch (type_) {
    case ToolType::kBrush:
      text_ = "Brush";
      break;
    case ToolType::kPencil:
      text_ = "Pencil";
      break;
    case ToolType::kBucket:
      text_ = "Bucket";
      break;
    case ToolType::kEraser:
      text_ = "Eraser";
      break;
    default:
      text_ = "Unknown";
  }
}

void ToolButton::Draw(graphics::Image& image) {
  Button::Draw(image);
  image.DrawText(GetX() + kFontSize, GetY() + (GetHeight() - kFontSize) / 2,
                 text_, 12, 0, 0, 0);
}
void ToolButton::DoAction() { GetListener()->SetActiveTool(type_, this); }

ToolType ToolButton::GetToolType() const { return type_; }
