#include "button_listener.h"
#include "cpputils/graphics/image.h"

#ifndef BUTTON_H
#define BUTTON_H

class Button {
 public:
  Button(int x, int y, int width, int height, ButtonListener* listener);
  virtual ~Button() = default;
  int GetX() const;
  int GetY() const;
  int GetWidth() const;
  int GetHeight() const;
  // bool GetSelected() const;
  // void SetSelected(bool is_selected);

  bool DidHandleEvent(const graphics::MouseEvent& event);
  virtual void Draw(graphics::Image& image);
  virtual void DoAction() = 0;

 protected:
  ButtonListener* GetListener() const;

 private:
  int x_;
  int y_;
  int width_;
  int height_;
  ButtonListener* listener_;
  bool is_selected_ = false;
  bool is_pressed_ = false;
};

#endif  // BUTTON_H
