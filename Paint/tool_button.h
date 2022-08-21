#include "button.h"

#ifndef TOOL_BUTTON_H
#define TOOL_BUTTON_H

class ToolButton : public Button {
 public:
  ToolButton(int x, int y, int width, int height, ButtonListener* listener,
             ToolType type);
  void Draw(graphics::Image& image) override;
  void DoAction() override;
  ToolType GetToolType() const;

 private:
  ToolType type_;
  std::string text_;
};

#endif  // TOOL_BUTTON_H
