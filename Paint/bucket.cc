#include "bucket.h"

#include <queue>

class Point {
 public:
  Point(int x, int y) : x(x), y(y){};
  int x;
  int y;
};

void Bucket::Fill(int x, int y, graphics::Image& image) {
  // RecursiveFill(x, y, image.GetColor(x, y), GetColor(), image);
  IterativeFill(x, y, image.GetColor(x, y), GetColor(), image);
}

void Bucket::RecursiveFill(int x, int y, graphics::Color start,
                           graphics::Color fill, graphics::Image& image) {
  if (start == fill) return;
  // Base case: check bounds.
  if (x < 0 || y < 0 || x >= image.GetWidth() || y >= image.GetHeight()) return;

  // Base case: check starting color.
  if (image.GetColor(x, y) != start) return;

  image.SetColor(x, y, fill);
  RecursiveFill(x + 1, y, start, fill, image);
  RecursiveFill(x - 1, y, start, fill, image);
  RecursiveFill(x, y + 1, start, fill, image);
  RecursiveFill(x, y - 1, start, fill, image);
}

void Bucket::IterativeFill(int x, int y, graphics::Color start,
                           graphics::Color fill, graphics::Image& image) {
  if (start == fill) return;
  std::queue<Point> pixels_to_check;
  Point initial(x, y);
  pixels_to_check.push(initial);
  while (pixels_to_check.size() > 0) {
    Point point = pixels_to_check.front();
    pixels_to_check.pop();
    if (point.x < 0 || point.y < 0 || point.x >= image.GetWidth() ||
        point.y >= image.GetWidth()) {
      continue;
    }
    if (image.GetColor(point.x, point.y) != start) {
      continue;
    }
    image.SetColor(point.x, point.y, fill);
    pixels_to_check.push(Point(point.x - 1, point.y));
    pixels_to_check.push(Point(point.x + 1, point.y));
    pixels_to_check.push(Point(point.x, point.y - 1));
    pixels_to_check.push(Point(point.x, point.y + 1));
  }
}
