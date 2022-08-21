#include "cpputils/graphics/image.h"

#ifndef PATH_TOOL_H
#define PATH_TOOL_H

// A class to track the current (x, y) coordinates of a tool. Like a real-life
// pencil, pen or brush, a PathTool will always have one (x, y) location on the
// canvas that it tracks.
class PathTool {
 public:
  // Keeps track of the starting coordinate for the drawing.
  virtual void Start(int x, int y, graphics::Image& image);

  // Updates the current coordinate of the tool.
  virtual void MoveTo(int x, int y, graphics::Image& image);

protected:
  int GetX() { return last_x_; }
  int GetY() { return last_y_; }

 private:
  int last_x_ = 0;
  int last_y_ = 0;
};

#endif  // PATH_TOOL_H
