#include "button.h"

#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

class ColorButton : public Button {
 public:
  ColorButton(int x, int y, int width, int height, ButtonListener* listener,
              const graphics::Color& color);
  void Draw(graphics::Image& image) override;
  void DoAction() override;
  graphics::Color GetColor() const;

 private:
  graphics::Color color_;
};

#endif  // COLOR_BUTTON_H
