#include "paint_program.h"
#include <iostream>

constexpr int kBrushWidth = 20;
constexpr int kImageSize = 500;

PaintProgram::PaintProgram() : image_(kImageSize, kImageSize) {}

// Destructor cleans up by removing itself as a MouseEventListener.
PaintProgram::~PaintProgram() { image_.RemoveMouseEventListener(*this); }

void PaintProgram::Initialize() {
  image_.AddMouseEventListener(*this);
  brush_.SetWidth(kBrushWidth);
  eraser_.SetWidth(20);

  // Some initial color setups.
  const graphics::Color Red(255, 0, 0);
  const graphics::Color Purple(155, 118, 204);
  const graphics::Color Green(50, 250, 10);
  const graphics::Color Blue(40, 20, 230);
  const graphics::Color Teal(20, 225, 250);


  // Creating ColorButtons.
  std::unique_ptr<ColorButton> red = std::make_unique<ColorButton> (10, 10, 30, 30, this, Red);
  Button_vector.push_back(std::move(red));

  std::unique_ptr<ColorButton> purple = std::make_unique<ColorButton> (60, 10, 30, 30, this, Purple);
  Button_vector.push_back(std::move(purple));

  std::unique_ptr<ColorButton> green = std::make_unique<ColorButton> (110, 10, 30, 30, this, Green);
  Button_vector.push_back(std::move(green));

  std::unique_ptr<ColorButton> blue = std::make_unique<ColorButton> (160, 10, 30, 30, this, Blue);
  Button_vector.push_back(std::move(blue));

  std::unique_ptr<ColorButton> teal = std::make_unique<ColorButton> (210, 10, 30, 30, this, Teal);
  Button_vector.push_back(std::move(teal));

  // Creating ToolButton
  std::unique_ptr<ToolButton> pencil = std::make_unique<ToolButton> (10, 100, 70, 50, this, ToolType::kPencil);
  Button_vector.push_back(std::move(pencil));

  std::unique_ptr<ToolButton> brush = std::make_unique<ToolButton> (100, 100, 70, 50, this, ToolType::kBrush);
  Button_vector.push_back(std::move(brush));

  std::unique_ptr<ToolButton> bucket = std::make_unique<ToolButton> (190, 100, 70, 50, this, ToolType::kBucket);
  Button_vector.push_back(std::move(bucket));

  std::unique_ptr<ToolButton> eraser = std::make_unique<ToolButton> (280, 100, 70, 50, this, ToolType::kEraser);
  Button_vector.push_back(std::move(eraser));


  SetActiveTool(kBrush, nullptr);
  SetActiveColor(Teal, nullptr);

  // Drawing the buttons.
  for(int i = 0; i < Button_vector.size(); i++){
    Button_vector[i]->Draw(image_);
    }
}

void PaintProgram::Start() { image_.ShowUntilClosed("TuffyPaint Program"); }

  // SetActiveTool Function
void PaintProgram::SetActiveTool(ToolType type, Button* tool_button) {
  std::string ToolType;
  if(type == kBucket){
  active_tool_type_ = kBucket;
  ToolType = "Bucket";
  }else if(type == kPencil){
  active_tool_type_ = kPencil;
  ToolType = "Pencil";
  }else if(type == kBrush){
  active_tool_type_ = kBrush;
  ToolType = "Brush";
  }else if(type == kEraser){
  active_tool_type_ = kEraser;
  ToolType = "Eraser";
  }
  //std::cout << "active button is now " << ToolType << std::endl;
}

// SetActiveTool Function
void PaintProgram::SetActiveColor(const graphics::Color& color, Button* color_button) {
  brush_.SetColor(color);
  bucket_.SetColor(color);
  pencil_.SetColor(color);
  eraser_.SetColor(graphics::Color(255, 255, 255));
}

// Updated OnmouseEvent with DidHandleEvent check at first.
void PaintProgram::OnMouseEvent(const graphics::MouseEvent& event) {
  for(int i = 0; i < Button_vector.size(); i++){
    if(Button_vector[i]->DidHandleEvent(event) == true)
    return;
  }
  switch (active_tool_type_) {
    case ToolType::kBucket:
      // Bucket paints on mouse down
      if (event.GetMouseAction() == graphics::MouseAction::kPressed) {
        bucket_.Fill(event.GetX(), event.GetY(), image_);
      }
      break;
    case ToolType::kPencil:
      SendEventToPathTool(pencil_, event);
      break;
    case ToolType::kBrush:
      SendEventToPathTool(brush_, event);
      break;
    case ToolType::kEraser:
      SendEventToPathTool(eraser_, event);
      break;
  }
  for(int i = 0; i < Button_vector.size(); i++){
    Button_vector[i]->Draw(image_);
    }
  image_.Flush();
}

void PaintProgram::SendEventToPathTool(PathTool& tool,
                                       const graphics::MouseEvent& event) {
  if (event.GetMouseAction() == graphics::MouseAction::kPressed) {
    tool.Start(event.GetX(), event.GetY(), image_);
  } else if (event.GetMouseAction() == graphics::MouseAction::kDragged) {
    tool.MoveTo(event.GetX(), event.GetY(), image_);
  }
}
