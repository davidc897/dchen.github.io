#include "cpputils/graphics/image.h"
#include "paint_program.h"
#include "tool_type.h"

const graphics::Color red = graphics::Color(255, 0, 0);

int main() {
  PaintProgram paint_program;
  paint_program.Initialize();

  //paint_program.SetActiveColor(red);

  // Set the active tool to whichever you want to try out.
  // paint_program.SetActiveTool(ToolType::kBucket);
  // paint_program.SetActiveTool(ToolType::kPencil);
  // paint_program.SetActiveTool(ToolType::kBrush);

  paint_program.Start();
  return 0;
}
