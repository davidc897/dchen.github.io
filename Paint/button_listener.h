#include "cpputils/graphics/image.h"
#include "tool_type.h"

#ifndef BUTTON_LISTENER_H
#define BUTTON_LISTENER_H

// Forward declare the Button class so that we can make use of it in
// ButtonListener. Because ButtonListener only needs a pointer to Button, and
// all pointers are the same size in memory, it is possible to define the
// ButtonListener interface without including the Button class defined in
// button.h.
class Button;

// The ButtonListener interface. You do not need to edit this class.
class ButtonListener {
 public:
  // This method is called when a Button wants to change the active color. The
  // implementor of ButtonListener could use the |color| parameter to update
  // any relevant tool's colors.
  virtual void SetActiveColor(const graphics::Color& color,
                              Button* color_button) = 0;

  // This method is called when a Button wants to change the active tool. The
  // implementor of ButtonListener could use the |type| parameter to update
  // the currently active tool.
  virtual void SetActiveTool(ToolType type, Button* tool_button) = 0;
};

#endif  // BUTTON_LISTENER_H
