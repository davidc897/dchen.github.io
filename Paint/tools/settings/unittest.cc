#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "../../brush.h"
#include "../../bucket.h"
#include "../../color_button.h"
#include "../../color_tool.h"
#include "../../cpputils/graphics/test/test_event_generator.h"
#include "../../paint_program.h"
#include "../../path_tool.h"
#include "../../pencil.h"
#include "../../tool_button.h"
#include "../cppaudit/gtest_ext.h"
#include "../cppaudit/image_test_utils.h"

const graphics::Color white(255, 255, 255);

struct ColorComp {
  bool operator()(const graphics::Color& a, const graphics::Color& b) const {
    return a.Red() * 255 * 255 + a.Green() * 255 + a.Blue() <
           b.Red() * 255 * 255 + b.Green() * 255 + b.Blue();
  }
};

struct Rect {
  Rect(int x, int y, int width, int height)
      : x(x),
        y(y),
        width(width),
        height(height),
        right(x + width),
        bottom(y + height) {}
  bool Empty() const { return x == right || y == bottom; }
  bool Intersects(const Rect& other) const {
    return !(Empty() || other.Empty() || other.x >= right || other.right <= x ||
             other.y >= bottom || other.bottom <= y);
  }
  bool Contains(int ptx, int pty) {
    return (x <= ptx && ptx <= right && y <= pty && pty <= bottom);
  }
  int x, y, width, height, right, bottom;
};

class PaintProgramTest : public testing::Test {
 public:
  void SetUp() override {
    // https://stackoverflow.com/questions/3803465/how-to-capture-stdout-stderr-with-googletest
    original = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());

    paint_program.Initialize();

    // Populate buttons.
    std::vector<std::unique_ptr<Button>>* buttons =
        paint_program.GetButtonsForTesting();
    ASSERT_TRUE(buttons->size() >= 8)
        << "    PaintProgram must return a vector of its buttons in the method "
           "GetButtonsForTesting. There should be at least 8 buttons: 3 tool "
           "buttons and 5 color buttons.";
    for (int i = 0; i < buttons->size(); i++) {
      button_rects.push_back(
          Rect(buttons->at(i)->GetX(), buttons->at(i)->GetY(),
               buttons->at(i)->GetWidth(), buttons->at(i)->GetHeight()));
      if (dynamic_cast<ToolButton*>(buttons->at(i).get()) != NULL) {
        ToolButton* tb_ptr = dynamic_cast<ToolButton*>(buttons->at(i).get());
        ToolType type = tb_ptr->GetToolType();
        if (tool_buttons[type]) {
          FAIL() << "    Found more than one button for tool type " << type
                 << " (see tool_type.h for translation to enum)";
        }
        tool_buttons[type] = tb_ptr;
      } else if (dynamic_cast<ColorButton*>(buttons->at(i).get()) != NULL) {
        ColorButton* cb_ptr = dynamic_cast<ColorButton*>(buttons->at(i).get());
        const graphics::Color color = cb_ptr->GetColor();
        if (color_buttons[color]) {
          FAIL() << "    Found more than one button for color (" << color.Red()
                 << ", " << color.Green() << ", " << color.Blue() << ")";
        }
        color_buttons[color] = cb_ptr;
      }
    }
  }

  bool RegionHasColor(const graphics::Color& color, int x, int y, int width,
                      int height) {
    graphics::Image* image = paint_program.GetImageForTesting();
    for (int i = x; i < x + width; i++) {
      for (int j = y; j < y + height; j++) {
        if (color == image->GetColor(i, j)) return true;
      }
    }
    return false;
  }

  bool PointIsOverButton(int x, int y) {
    for (int k = 0; k < button_rects.size(); k++) {
      if (button_rects[k].Contains(x, y)) {
        return true;
      }
    }
    return false;
  }

  void PrepareToTestMouseEvents() {
    graphics::Image* image = paint_program.GetImageForTesting();
    generator = std::make_unique<graphics::TestEventGenerator>(image);
    image->Show();
  }

  void ClickButton(Button* button) {
    if (!generator) {
      PrepareToTestMouseEvents();
    }
    // Click a random point over the button, drag a little ways, and release.
    int x1 = button->GetX() + rand() % 10;
    int y1 = button->GetY() + rand() % 10;
    generator->MoveMouseTo(x1, y1);
    generator->MouseDown(x1, y1);
    generator->MoveMouseTo(button->GetX() + rand() % 10,
                           button->GetY() + rand() % 10);
    generator->MouseUp();
  }

  bool ImageIsColorExceptForButtons(const graphics::Color& color) {
    for (int i = 0; i < paint_program.GetImageForTesting()->GetWidth(); i++) {
      for (int j = 0; j < paint_program.GetImageForTesting()->GetHeight();
           j++) {
        if (color != paint_program.GetImageForTesting()->GetColor(i, j)) {
          if (!PointIsOverButton(i, j)) {
            return false;
          }
        }
      }
    }
    return true;
  }

  bool ImagesMatchExceptForButtons(const graphics::Image& expected, int* x,
                                   int* y) {
    graphics::Image* actual = paint_program.GetImageForTesting();
    if (expected.GetWidth() != actual->GetWidth() ||
        expected.GetHeight() != actual->GetHeight()) {
      *x = -1;
      *y = -1;
      return false;
    }
    for (int i = 0; i < actual->GetWidth(); i++) {
      for (int j = 0; j < actual->GetHeight(); j++) {
        if (expected.GetColor(i, j) != actual->GetColor(i, j)) {
          if (!PointIsOverButton(i, j)) {
            *x = i;
            *y = j;
            return false;
          }
        }
      }
    }
    return true;
  }

  void TearDown() override { std::cout.rdbuf(original); }

 protected:
  std::streambuf* original;
  std::stringstream stream;
  std::map<ToolType, ToolButton*> tool_buttons;
  std::map<graphics::Color, ColorButton*, ColorComp> color_buttons;
  std::vector<Rect> button_rects;
  PaintProgram paint_program;
  std::unique_ptr<graphics::TestEventGenerator> generator;
};

TEST(PaintProgramInheritanceTest, InheritsFromButtonListener) {
  bool is_buttonlistener = std::is_base_of<ButtonListener, PaintProgram>::value;
  EXPECT_TRUE(is_buttonlistener)
      << "    PaintProgram should inherit from ButtonListener";
}

TEST_F(PaintProgramTest, HasEnoughButtons) {
  ASSERT_TRUE(tool_buttons.size() >= 3)
      << "    You must have at least 3 tool buttons";
  ASSERT_TRUE(color_buttons.size() >= 5)
      << "    You must have at least 5 color buttons";
}

TEST_F(PaintProgramTest, HasValidButtonTypes) {
  bool has_brush = tool_buttons[ToolType::kBrush] != nullptr;
  bool has_pencil = tool_buttons[ToolType::kPencil] != nullptr;
  bool has_bucket = tool_buttons[ToolType::kBucket] != nullptr;
  ASSERT_TRUE(has_pencil) << "    Could not find button for ToolType::kPencil";
  ASSERT_TRUE(has_brush) << "    Could not find button for ToolType::kBrush";
  ASSERT_TRUE(has_bucket) << "    Could not find button for ToolType::kBucket";

  for (auto pair : color_buttons) {
    ASSERT_NE(white, pair.first)
        << "    No color buttons should have color white, (255, 255, 255)";
  }
}

TEST_F(PaintProgramTest, ButtonSizesAndPositions) {
  for (int i = 0; i < button_rects.size(); i++) {
    ASSERT_TRUE(button_rects[i].width >= 10)
        << "    Each button must have a width at least 10 pixels, but button "
           "at index "
        << i << " was only " << button_rects[i].width << "px wide";
    ASSERT_TRUE(button_rects[i].height >= 10)
        << "    Each button must have a height at least 10 pixels but button "
           "at index "
        << i << " was only " << button_rects[i].height << "px high";
    for (int j = 0; j < button_rects.size(); j++) {
      if (i == j) continue;
      ASSERT_FALSE(button_rects[i].Intersects(button_rects[j]))
          << "    No buttons should intersect. Found intersection between "
             "button at index "
          << i << " and button at " << j;
    }
  }
}

TEST_F(PaintProgramTest, ColorButtonsHaveAtLeastOnePixelOfCorrectColor) {
  for (auto pair : color_buttons) {
    ASSERT_TRUE(RegionHasColor(pair.first, pair.second->GetX(),
                               pair.second->GetY(), pair.second->GetWidth(),
                               pair.second->GetHeight()))
        << "    ColorButton::Draw should draw at least one pixel that is the "
           "color passed in the constructor within the button. In this case "
           " we could not find a pixel with color ("
        << pair.first.Red() << ", " << pair.first.Green() << ", "
        << pair.first.Blue() << ") in the region with upper left corner at ("
        << pair.second->GetX() << ", " << pair.second->GetY() << ") with width "
        << pair.second->GetWidth() << " and height "
        << pair.second->GetHeight();
  }
}

TEST_F(PaintProgramTest, ImageIsInitiallyWhiteExceptButtons) {
  ASSERT_TRUE(ImageIsColorExceptForButtons(white));
}

TEST_F(PaintProgramTest, BucketFillsWithColorOnMouseDown) {
  remove("BucketFillsWithColor_actual.bmp");

  // Select the bucket button
  ClickButton(tool_buttons[ToolType::kBucket]);

  // Try with all the color buttons
  for (auto pair : color_buttons) {
    graphics::Color color = pair.first;
    ClickButton(pair.second);

    // Now click a random non-button part of the screen.
    int x = rand() % 400 + 50;
    int y = rand() % 400 + 50;
    while (PointIsOverButton(x, y)) {
      x = rand() % 400 + 50;
      y = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x, y);
    generator->MouseDown(x, y);
    bool is_color = ImageIsColorExceptForButtons(color);
    generator->MouseUp();

    ASSERT_TRUE(is_color)
        << "    After clicking the Bucket tool button and the (" << color.Red()
        << ", " << color.Green() << ", " << color.Blue()
        << ") color button, a kPressed event at (" << x << ", " << y
        << ") should fill the whole image with that color (except the "
           "buttons). Some of "
           "your image was not the right color, see "
           "BucketFillsWithColor_actual.bmp.";
    if (!is_color) {
      paint_program.GetImageForTesting()->SaveImageBmp(
          "BucketFillsWithColor_actual.bmp");
    }
  }
}

TEST_F(PaintProgramTest, PencilDrawsAPixelOnClick) {
  // Select the pencil button
  ClickButton(tool_buttons[ToolType::kPencil]);

  // Try with all the color buttons
  for (auto pair : color_buttons) {
    graphics::Color color = pair.first;
    ClickButton(pair.second);

    // Now click a random non-button part of the screen.
    int x = rand() % 400 + 50;
    int y = rand() % 400 + 50;
    while (PointIsOverButton(x, y)) {
      x = rand() % 400 + 50;
      y = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x, y);
    generator->MouseDown(x, y);
    bool is_color = ImageIsColorExceptForButtons(color);
    generator->MouseUp();

    ASSERT_EQ(color, paint_program.GetImageForTesting()->GetColor(x, y))
        << "    After clicking the Pencil tool button and the (" << color.Red()
        << ", " << color.Green() << ", " << color.Blue()
        << ") color button, a kPressed event at (" << x << ", " << y
        << ") should draw a single pixel with that color.";

    // Change the one pixel back to white and test the image is now all white.
    paint_program.GetImageForTesting()->SetColor(x, y, white);
    EXPECT_TRUE(ImageIsColorExceptForButtons(white))
        << "    Your Pencil should only draw at the (x, y) coordinate of a "
           "kPressed MouseEvent. No other pixels should be colored besides the "
           "buttons, but other colored pixels were found.";
  }
}

TEST_F(PaintProgramTest, PencilDrawsALineOnClickAndDrag) {
  remove("PencilDrawsALineOnClickAndDrag.bmp");
  graphics::Image expected(500, 500);

  // Select the pencil button
  ClickButton(tool_buttons[ToolType::kPencil]);

  // Image should still be white.
  bool is_white = ImageIsColorExceptForButtons(white);
  ASSERT_TRUE(is_white)
      << "    A click on the pencil button should not cause "
         "any drawing besides changes to buttons. Try clicking the "
         "PaintProgram "
         "pencil button and ensure that no pixels are changed.";

  // Try with all the color buttons
  for (auto pair : color_buttons) {
    graphics::Color color = pair.first;
    ClickButton(pair.second);

    // Now click a random non-button part of the screen.
    int x = rand() % 400 + 50;
    int y = rand() % 400 + 50;
    while (PointIsOverButton(x, y)) {
      x = rand() % 400 + 50;
      y = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x, y);
    generator->MouseDown(x, y);
    ASSERT_EQ(color, paint_program.GetImageForTesting()->GetColor(x, y))
        << "    After clicking the Pencil tool button and the (" << color.Red()
        << ", " << color.Green() << ", " << color.Blue()
        << ") color button, a kPressed event at (" << x << ", " << y
        << ") should draw a single pixel with that color.";

    // Drag to a random point of the screen.
    int x2 = rand() % 400 + 50;
    int y2 = rand() % 400 + 50;
    while (PointIsOverButton(x2, y2)) {
      x2 = rand() % 400 + 50;
      y2 = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x2, y2);

    // Drag to a third point of the screen.
    int x3 = rand() % 400 + 50;
    int y3 = rand() % 400 + 50;
    while (PointIsOverButton(x3, y3)) {
      x3 = rand() % 400 + 50;
      y3 = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x3, y3);

    expected.DrawLine(x, y, x2, y2, color);
    expected.DrawLine(x2, y2, x3, y3, color);
    int x_diff = 0;
    int y_diff = 0;
    bool match = ImagesMatchExceptForButtons(expected, &x_diff, &y_diff);
    if (!match) {
      graphics::Image output(1000, 500);
      graphics::Image* actual = paint_program.GetImageForTesting();
      for (int i = 0; i < 500; i++) {
        for (int j = 0; j < 500; j++) {
          output.SetColor(i, j, expected.GetColor(i, j));
          output.SetColor(i + 500, j, actual->GetColor(i, j));
        }
      }
      output.SaveImageBmp("PencilDrawsALineOnClickAndDrag.bmp");
    }
    ASSERT_TRUE(match)
        << "   The PaintProgram did not send a MoveTo to the "
           "pencil when a kDragged happened, or it did not send the MoveTo "
           "using the correct coordinates from the dragged MouseEvent. See the "
           "comparison of your PaintProgram output (right) and the expected "
           "output (left) at PencilDrawsALineOnClickAndDrag.bmp. A differing "
           "pixel was found at ("
        << x_diff << ", " << y_diff
        << "). Note that pixels belonging to buttons were ignored.";

    generator->MouseUp();
    match = ImagesMatchExceptForButtons(expected, &x_diff, &y_diff);
    ASSERT_TRUE(match)
        << "   The PaintProgram should not draw with the "
           "pencil when a kReleased happened, or it did not send the MoveTo "
           "using the correct coordinates from the dragged MouseEvent. Try "
           "clicking, dragging and releasing with the pencil tool active and "
           "ensure no drawing happens on release.";
  }
}

TEST_F(PaintProgramTest, BrushDrawsACircleOnClick) {
  // Select the brush button
  ClickButton(tool_buttons[ToolType::kBrush]);

  // Try with all the color buttons
  for (auto pair : color_buttons) {
    graphics::Color color = pair.first;
    ClickButton(pair.second);

    // Now click a random non-button part of the screen.
    int x = rand() % 400 + 50;
    int y = rand() % 400 + 50;
    while (PointIsOverButton(x, y)) {
      x = rand() % 400 + 50;
      y = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x, y);
    generator->MouseDown(x, y);
    bool is_color = ImageIsColorExceptForButtons(color);
    generator->MouseUp();

    ASSERT_EQ(color, paint_program.GetImageForTesting()->GetColor(x, y))
        << "    After clicking the Brush tool button and the (" << color.Red()
        << ", " << color.Green() << ", " << color.Blue()
        << ") color button, a kPressed event at (" << x << ", " << y
        << ") should draw a single circle, width 20, with that color.";

    // Change the one circle back to white and test the image is now all white.
    paint_program.GetImageForTesting()->DrawCircle(x, y, 10, white);
    EXPECT_TRUE(ImageIsColorExceptForButtons(white))
        << "    Your Brush should only draw at the (x, y) coordinate of a "
           "kPressed MouseEvent with a width of 20 (radius 10). No other "
           "pixels should be colored besides the buttons, but other colored "
           "pixels were found.";
  }
}

TEST_F(PaintProgramTest, BrushDrawsAThickLineOnDrag) {
  remove("BrushDrawsAThickLineOnDrag.bmp");
  graphics::Image expected(500, 500);

  // Select the pencil button
  ClickButton(tool_buttons[ToolType::kBrush]);

  // Image should still be white.
  bool is_white = ImageIsColorExceptForButtons(white);
  ASSERT_TRUE(is_white)
      << "    A click on the brush button should not cause "
         "any drawing besides changes to buttons. Try clicking the "
         "PaintProgram "
         "brush button and ensure that no pixels are changed.";

  // Try with all the color buttons
  for (auto pair : color_buttons) {
    graphics::Color color = pair.first;
    ClickButton(pair.second);

    // Now click a random non-button part of the screen.
    int x = rand() % 400 + 50;
    int y = rand() % 400 + 50;
    while (PointIsOverButton(x, y)) {
      x = rand() % 400 + 50;
      y = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x, y);
    generator->MouseDown(x, y);
    ASSERT_EQ(color, paint_program.GetImageForTesting()->GetColor(x, y))
        << "    After clicking the Brush tool button and the (" << color.Red()
        << ", " << color.Green() << ", " << color.Blue()
        << ") color button, a kPressed event at (" << x << ", " << y
        << ") should draw a single circle with that color.";

    // Drag to a random point of the screen.
    int x2 = rand() % 400 + 50;
    int y2 = rand() % 400 + 50;
    while (PointIsOverButton(x2, y2)) {
      x2 = rand() % 400 + 50;
      y2 = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x2, y2);

    // Drag to a third point of the screen.
    int x3 = rand() % 400 + 50;
    int y3 = rand() % 400 + 50;
    while (PointIsOverButton(x3, y3)) {
      x3 = rand() % 400 + 50;
      y3 = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x3, y3);

    expected.DrawCircle(x, y, 10, color);
    expected.DrawLine(x, y, x2, y2, color, 20);
    expected.DrawCircle(x2, y2, 10, color);
    expected.DrawLine(x2, y2, x3, y3, color, 20);
    expected.DrawCircle(x3, y3, 10, color);

    int x_diff = 0;
    int y_diff = 0;
    bool match = ImagesMatchExceptForButtons(expected, &x_diff, &y_diff);
    if (!match) {
      graphics::Image output(1000, 500);
      graphics::Image* actual = paint_program.GetImageForTesting();
      for (int i = 0; i < 500; i++) {
        for (int j = 0; j < 500; j++) {
          output.SetColor(i, j, expected.GetColor(i, j));
          output.SetColor(i + 500, j, actual->GetColor(i, j));
        }
      }
      output.SaveImageBmp("BrushDrawsAThickLineOnDrag.bmp");
    }
    ASSERT_TRUE(match)
        << "   The PaintProgram did not send a MoveTo to the "
           "brush when a kDragged happened, or it did not send the MoveTo "
           "using the correct coordinates from the dragged MouseEvent. See the "
           "comparison of your PaintProgram output and the expected output at "
           "BrushDrawsAThickLineOnDrag.bmp. A differing pixel was found at ("
        << x_diff << ", " << y_diff
        << "). Note that pixels belonging to "
           "buttons were ignored.";

    generator->MouseUp();
    match = ImagesMatchExceptForButtons(expected, &x_diff, &y_diff);
    ASSERT_TRUE(match)
        << "   The PaintProgram should not draw with the "
           "brush when a kReleased happened, or it did not send the MoveTo "
           "using the correct coordinates from the dragged MouseEvent. Try "
           "clicking, dragging and releasing with the brush tool active and "
           "ensure no drawing happens on release.";
  }
}

TEST_F(PaintProgramTest, ButtonsDrawOnTopOfTools) {
  remove("ButtonsDrawOnTopOfTools.bmp");
  Button* brush_button = tool_buttons[ToolType::kBrush];
  ClickButton(brush_button);
  for (auto pair : color_buttons) {
    graphics::Color color = pair.first;
    ClickButton(pair.second);
    break;
  }
  // Draw across the Brush button.
  int x = rand() % 400 + 50;
  int y = rand() % 400 + 50;
  while (PointIsOverButton(x, y)) {
    x = rand() % 400 + 50;
    y = rand() % 400 + 50;
  }
  generator->MoveMouseTo(x, y);
  generator->MouseDown(x, y);

  generator->MoveMouseTo(brush_button->GetX() - 1, brush_button->GetY() - 1);
  generator->MoveMouseTo(brush_button->GetX() + brush_button->GetWidth() + 1,
                         brush_button->GetY() + brush_button->GetHeight() + 1);
  generator->MouseUp();

  // The button should look clean.
  graphics::Image expected(brush_button->GetWidth() + brush_button->GetX(),
                           brush_button->GetHeight() + brush_button->GetY());
  brush_button->Draw(expected);

  graphics::Image* actual = paint_program.GetImageForTesting();
  for(int i = brush_button->GetX(); i < brush_button->GetX() + brush_button->GetWidth(); i++) {
    for (int j = brush_button->GetY(); j < brush_button->GetY() + brush_button->GetHeight(); j++) {
      if (expected.GetColor(i, j) != actual->GetColor(i, j)) {
        actual->SaveImageBmp("ButtonsDrawOnTopOfTools.bmp");
      }
      ASSERT_EQ(expected.GetColor(i, j), actual->GetColor(i, j))
          << "    Buttons should always be drawn on top of the image after "
          "each mouse event (redraw all buttons last in OnMouseEvent). Found "
          "an issue where drawing over the Brush button and the button didn't "
          "look right. See your paint program's output at "
          "ButtonsDrawOnTopOfTools.bmp";
    }
  }
}

TEST_F(PaintProgramTest, EraserDrawsACircleOnClick) {
  bool has_eraser = tool_buttons[ToolType::kEraser] != nullptr;
  ASSERT_TRUE(has_eraser) << "    Could not find button for ToolType::kEraser";

  // Select the bucket button
  ClickButton(tool_buttons[ToolType::kBucket]);
  graphics::Color color;

  // Click one of the color buttons and fill the screen with the bucket.
  for (auto pair : color_buttons) {
    color = pair.first;
    ClickButton(pair.second);
    // Now click a random non-button part of the screen to use the bucket.
    int x = rand() % 400 + 50;
    int y = rand() % 400 + 50;
    while (PointIsOverButton(x, y)) {
      x = rand() % 400 + 50;
      y = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x, y);
    generator->MouseDown(x, y);
    bool is_color = ImageIsColorExceptForButtons(color);
    ASSERT_TRUE(is_color)
        << "    Bucket tool should fill the image with the selected color.";
    generator->MouseUp();
    break;
  }

  ClickButton(tool_buttons[ToolType::kEraser]);

  // Now click a random non-button part of the screen.
  int x = rand() % 400 + 50;
  int y = rand() % 400 + 50;
  while (PointIsOverButton(x, y)) {
    x = rand() % 400 + 50;
    y = rand() % 400 + 50;
  }
  generator->MoveMouseTo(x, y);
  generator->MouseDown(x, y);
  bool is_color = ImageIsColorExceptForButtons(color);
  generator->MouseUp();

  ASSERT_EQ(white, paint_program.GetImageForTesting()->GetColor(x, y))
      << "    After clicking the eraser tool button, a kPressed event at (" << x
      << ", " << y << ") should draw a single circle, width 20, with white.";

  // Change the one circle back to color and test the image is now all color.
  paint_program.GetImageForTesting()->DrawCircle(x, y, 10, color);
  EXPECT_TRUE(ImageIsColorExceptForButtons(color))
      << "    Your eraser should only draw at the (x, y) coordinate of a "
         "kPressed MouseEvent with a width of 20 (radius 10). No other "
         "pixels should be white besides the buttons, but other white "
         "pixels were found.";
}

TEST_F(PaintProgramTest, EraserDrawsAThickLineOnDrag) {
  remove("EraserDrawsAThickLineOnDrag.bmp");
  bool has_eraser = tool_buttons[ToolType::kEraser] != nullptr;
  ASSERT_TRUE(has_eraser) << "    Could not find button for ToolType::kEraser";

  // Select the bucket button
  ClickButton(tool_buttons[ToolType::kBucket]);
  graphics::Color color;

  // Click one of the color buttons and fill the screen with the bucket.
  for (auto pair : color_buttons) {
    color = pair.first;
    ClickButton(pair.second);
    // Now click a random non-button part of the screen to use the bucket.
    int x = rand() % 400 + 50;
    int y = rand() % 400 + 50;
    while (PointIsOverButton(x, y)) {
      x = rand() % 400 + 50;
      y = rand() % 400 + 50;
    }
    generator->MoveMouseTo(x, y);
    generator->MouseDown(x, y);
    bool is_color = ImageIsColorExceptForButtons(color);
    ASSERT_TRUE(is_color)
        << "    Bucket tool should fill the image with the selected color.";
    generator->MouseUp();
    break;
  }

  ClickButton(tool_buttons[ToolType::kEraser]);

  // Now click a random non-button part of the screen.
  int x = rand() % 400 + 50;
  int y = rand() % 400 + 50;
  while (PointIsOverButton(x, y)) {
    x = rand() % 400 + 50;
    y = rand() % 400 + 50;
  }
  generator->MoveMouseTo(x, y);
  generator->MouseDown(x, y);
  ASSERT_EQ(white, paint_program.GetImageForTesting()->GetColor(x, y))
      << "    After clicking the Brush tool button and the (" << color.Red()
      << ", " << color.Green() << ", " << color.Blue()
      << ") color button, a kPressed event at (" << x << ", " << y
      << ") should draw a single circle with that color.";

  // Drag to a random point of the screen.
  int x2 = rand() % 400 + 50;
  int y2 = rand() % 400 + 50;
  while (PointIsOverButton(x2, y2)) {
    x2 = rand() % 400 + 50;
    y2 = rand() % 400 + 50;
  }
  generator->MoveMouseTo(x2, y2);

  // Drag to a third point of the screen.
  int x3 = rand() % 400 + 50;
  int y3 = rand() % 400 + 50;
  while (PointIsOverButton(x3, y3)) {
    x3 = rand() % 400 + 50;
    y3 = rand() % 400 + 50;
  }
  generator->MoveMouseTo(x3, y3);

  graphics::Image expected(500, 500);
  expected.DrawRectangle(0, 0, 500, 500, color);
  expected.DrawCircle(x, y, 10, white);
  expected.DrawLine(x, y, x2, y2, white, 20);
  expected.DrawCircle(x2, y2, 10, white);
  expected.DrawLine(x2, y2, x3, y3, white, 20);
  expected.DrawCircle(x3, y3, 10, white);

  int x_diff = 0;
  int y_diff = 0;
  bool match = ImagesMatchExceptForButtons(expected, &x_diff, &y_diff);
  if (!match) {
    graphics::Image output(1000, 500);
    graphics::Image* actual = paint_program.GetImageForTesting();
    for (int i = 0; i < 500; i++) {
      for (int j = 0; j < 500; j++) {
        output.SetColor(i, j, expected.GetColor(i, j));
        output.SetColor(i + 500, j, actual->GetColor(i, j));
      }
    }
    output.SaveImageBmp("EraserDrawsAThickLineOnDrag.bmp");
  }
  ASSERT_TRUE(match)
      << "   The PaintProgram did not send a MoveTo to the "
         "eraser when a kDragged happened, or it did not send the MoveTo "
         "using the correct coordinates from the dragged MouseEvent. See the "
         "comparison of your PaintProgram output and the expected output at "
         "EraserDrawsAThickLineOnDrag.bmp. A differing pixel was found at ("
      << x_diff << ", " << y_diff
      << "). Note that pixels belonging to "
         "buttons were ignored.";

  generator->MouseUp();
  match = ImagesMatchExceptForButtons(expected, &x_diff, &y_diff);
  ASSERT_TRUE(match)
      << "   The PaintProgram should not draw with the "
         "eraser when a kReleased happened, or it did not send the MoveTo "
         "using the correct coordinates from the dragged MouseEvent. Try "
         "clicking, dragging and releasing with the brush tool active and "
         "ensure no drawing happens on release.";
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  bool skip = true;
  for (int i = 0; i < argc; i++) {
    if (std::string(argv[i]) == "--noskip") {
      skip = false;
    }
  }
  if (skip) {
    ::testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  }
  return RUN_ALL_TESTS();
}
