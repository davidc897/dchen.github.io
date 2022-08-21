#include "color_tool.h"
#include "cpputils/graphics/image.h"

#ifndef BUCKET_H
#define BUCKET_H

class Bucket : public ColorTool {
 public:
  Bucket() = default;
  ~Bucket() = default;

  // Fill an image starting at (x, y).
  void Fill(int x, int y, graphics::Image& image);

  // Recursive Fill helper.
  void RecursiveFill(int x, int y, graphics::Color start, graphics::Color fill,
                     graphics::Image& image);

  // Iterative Fill helper.
  void IterativeFill(int x, int y, graphics::Color start, graphics::Color fill,
                     graphics::Image& image);
};

#endif  // BUCKET_H
