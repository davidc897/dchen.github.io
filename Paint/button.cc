#include "button.h"

const graphics::Color kPressedColor(50, 50, 200);
const graphics::Color kNotPressedColor(150, 150, 150);

Button::Button(int x, int y, int width, int height, ButtonListener* listener)
    : x_(x), y_(y), width_(width), height_(height), listener_(listener) {}

// void Button::SetSelected(bool is_selected) { is_selected_ = is_selected; }
//
// bool Button::GetSelected() const { return is_selected_; }

int Button::GetX() const { return x_; }

int Button::GetY() const { return y_; }

int Button::GetWidth() const { return width_; }

int Button::GetHeight() const { return height_; }

ButtonListener* Button::GetListener() const { return listener_; }

bool Button::DidHandleEvent(const graphics::MouseEvent& event) {
  if (event.GetMouseAction() == graphics::MouseAction::kDragged) {
    // Nothing changes during a drag.
    return is_pressed_;
  }
  if (event.GetX() >= x_ && event.GetX() <= x_ + width_ && event.GetY() >= y_ &&
      event.GetY() <= y_ + height_) {
    if (event.GetMouseAction() == graphics::MouseAction::kReleased &&
        is_pressed_) {
      DoAction();
      is_pressed_ = false;
      return true;
    } else if (event.GetMouseAction() == graphics::MouseAction::kPressed) {
      is_pressed_ = true;
      return true;
    } else if (event.GetMouseAction() == graphics::MouseAction::kDragged) {
      return true;
    }
  }
  is_pressed_ = false;
  return false;
}

void Button::Draw(graphics::Image& image) {
  image.DrawRectangle(x_, y_, width_, height_,
                      is_pressed_ ? kPressedColor : kNotPressedColor);
}
