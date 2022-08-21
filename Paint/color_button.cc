#include "color_button.h"

ColorButton::ColorButton(int x, int y, int width, int height,
                         ButtonListener* listener, const graphics::Color& color)
    : Button(x, y, width, height, listener), color_(color) {}

void ColorButton::Draw(graphics::Image& image) {
  Button::Draw(image);
  image.DrawRectangle(GetX() + 4, GetY() + 4, GetWidth() - 8, GetHeight() - 8,
                      color_);
}

void ColorButton::DoAction() { GetListener()->SetActiveColor(color_, this); }

graphics::Color ColorButton::GetColor() const { return color_; }
