#include "brush.h"
#include "bucket.h"
#include "cpputils/graphics/image.h"
#include "pencil.h"
#include "tool_type.h"
#include "button_listener.h"
#include "tool_button.h"
#include "color_button.h"
#include <vector>
#include "eraser.h"

#ifndef PAINT_PROGRAM_H
#define PAINT_PROGRAM_H

class PaintProgram : public graphics::MouseEventListener, public ButtonListener {
 public:
  PaintProgram();
  ~PaintProgram();

  // Does all the set-up work.
  void Initialize();

  // Begins drawing.
  void Start();

  // Changes the currently active tool type.
  void SetActiveTool(ToolType tool_type, Button* tool_button) override;

  // Changes the color of all the tools.
  void SetActiveColor(const graphics::Color& color, Button* color_button) override;

  // Overridden from graphics::MouseEventListener interface
  void OnMouseEvent(const graphics::MouseEvent& event) override;

  //GetButtonForTesting Function
  std::vector<std::unique_ptr<Button>>* GetButtonsForTesting() {return &Button_vector;}

  graphics::Image* GetImageForTesting() { return &image_; }

 private:
  // Helper function making use of the Polymorphism of PaintPencil and
  // PaintBrush.
  void SendEventToPathTool(PathTool& tool, const graphics::MouseEvent& event);

  // The image_ which will be the canvas for the PaintProgram.
  graphics::Image image_;

  // The tools.
  Pencil pencil_;
  Bucket bucket_;
  Brush brush_;
  Eraser eraser_;


  // Unique_ptr vector for buttons.
  std::vector<std::unique_ptr<Button>> Button_vector;

  // Represents which tool is active.
  ToolType active_tool_type_;
};

#endif  // PAINT_PROGRAM_H
