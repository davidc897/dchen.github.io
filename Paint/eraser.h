#include "cpputils/graphics/image.h"
#include "brush.h"

#ifndef ERASER_H
#define ERASER_H

class Eraser : public Brush {
  // Your code here
public:
  Eraser() = default;
  ~Eraser() = default;

// Eraer GetColor() function.
 graphics::Color GetColor() const override;

private:
  graphics::Color color_for_earser = graphics::Color(255, 255, 255);

};

#endif  // ERASER_H
