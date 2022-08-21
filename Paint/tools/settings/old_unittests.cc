
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "../../brush.h"
#include "../../bucket.h"
#include "../../button.h"
#include "../../button_listener.h"
#include "../../color_button.h"
#include "../../color_tool.h"
#include "../../cpputils/graphics/test/test_event_generator.h"
#include "../../paint_program.h"
#include "../../path_tool.h"
#include "../../pencil.h"
#include "../../tool_button.h"
#include "../cppaudit/gtest_ext.h"
#include "../cppaudit/image_test_utils.h"

const graphics::Color blue(0, 0, 255);
const graphics::Color green(0, 255, 0);
const graphics::Color red(255, 0, 0);
const graphics::Color white(255, 255, 255);
const graphics::Color yellow(255, 255, 0);

bool ImageIsColor(graphics::Image& image, const graphics::Color& color) {
  for (int i = 0 ; i < image.GetWidth(); i++) {
    for (int j = 0; j < image.GetHeight(); j++) {
      if (color != image.GetColor(i, j)) return false;
    }
  }
  return true;
}

void FillWhiteOnWhite() {
  graphics::Image image(4, 4);
  image.SetColor(3, 3, blue);
  Bucket bucket;
  bucket.SetColor(white);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(0, 0, image);
  });
  EXPECT_EQ(image.GetColor(3, 3), blue) << "    Shouldn't take any action "
      "when fill starts at a pixel that's already the right color.";
}

TEST(PencilTest, Constructor) {
  Pencil pencil;
}

TEST(PencilTest, GetAndSetColor) {
  Pencil pencil;
  pencil.SetColor(blue);
  EXPECT_EQ(pencil.GetColor(), blue) << "    Should return the same color for "
      "GetColor that was passed for SetColor";
}

TEST(PencilTest, DrawPoint) {
  Pencil pencil;
  graphics::Image image(50, 50);
  pencil.SetColor(blue);
  pencil.Start(0, 0, image);

  EXPECT_EQ(image.GetColor(0, 0), blue) << "    Should draw a single blue "
      "pixel at x = 0, y = 0 when Start is called with (0, 0) after color "
      "was set to blue.";

  pencil.Start(10, 10, image);
  EXPECT_EQ(image.GetColor(10, 10), blue) << "    Should draw a single blue "
      "pixel at x = 10, y = 10 when Start is called with (10, 10) after color "
      "was set to blue.";

  pencil.Start(10, 20, image);
  EXPECT_EQ(image.GetColor(10, 10), blue) << "    Should draw a single blue "
      "pixel at x = 10, y = 20 when Start is called with (10, 20) after color "
      "was set to blue.";
}

TEST(PencilTest, DrawLine) {
  std::string filename = "PencilTest_DrawLine_expected_vs_actual.bmp";
  remove(filename.c_str());
  Pencil pencil;
  graphics::Image image(100, 100);
  pencil.SetColor(blue);

  pencil.Start(5, 5, image);
  pencil.MoveTo(45, 45, image);
  pencil.Start(5, 45, image);
  pencil.MoveTo(45, 5, image);

  graphics::Image expected(100, 100);
  expected.DrawLine(5, 5, 45, 45, blue);
  expected.DrawLine(5, 45, 45, 5, blue);

  EXPECT_TRUE(ImagesMatch(&expected, &image, filename,
      DiffType::kTypeSideBySide));
}

TEST(PencilTest, MultipleColors) {
  std::string filename = "PencilTest_MultipleColors_expected_vs_actual.bmp";
  remove(filename.c_str());

  Pencil pencil;
  graphics::Image image(100, 100);

  pencil.SetColor(blue);
  pencil.Start(0, 99, image);
  pencil.MoveTo(50, 50, image);
  pencil.SetColor(green);
  pencil.MoveTo(99, 0, image);

  graphics::Image expected(100, 100);
  expected.DrawLine(0, 99, 50, 50, blue);
  expected.DrawLine(50, 50, 99, 0, green);

  EXPECT_TRUE(ImagesMatch(&expected, &image, filename, DiffType::kTypeSideBySide));
}

TEST(BrushTest, Constructor) {
  Brush brush;
}

TEST(BrushTest, GetAndSetColor) {
  Brush brush;
  brush.SetColor(blue);
  EXPECT_EQ(brush.GetColor(), blue) << "    Should return the same color for "
      "GetColor that was passed for SetColor";
}

TEST(BrushTest, DrawDot) {
  std::string filename = "BrushTest_DrawDot_expected_vs_actual.bmp";
  Brush brush;
  graphics::Image image(50, 50);
  brush.SetColor(blue);
  brush.SetWidth(20);
  brush.Start(30, 20, image);

  ASSERT_EQ(image.GetColor(30, 20), blue) << "    Should draw a single blue "
      "dot at x = 30, y = 20 when Start is called with (30, 20) after color "
      "was set to blue.";

  graphics::Image expected(50, 50);
  expected.DrawCircle(30, 20, 10, blue);
  EXPECT_TRUE(ImagesMatch(&expected, &image, filename,
      DiffType::kTypeSideBySide)) << "    Should draw a circle of radius 10 "
      "when Start was called with width set to 20 and color set to blue.";
}

TEST(BrushTest, DrawLine) {
  std::string filename = "BrushTest_DrawLine_expected_vs_actual.bmp";
  remove(filename.c_str());
  Brush brush;
  graphics::Image image(100, 100);
  brush.SetColor(blue);
  brush.SetWidth(10);

  // Go from corner to corner, we can ignore whether they put the endcap on.
  brush.Start(0, 0, image);
  brush.MoveTo(99, 99, image);
  brush.Start(99, 0, image);
  brush.MoveTo(0, 99, image);

  graphics::Image expected(100, 100);
  expected.DrawLine(0, 0, 99, 99, blue, 10);
  expected.DrawLine(0, 99, 99, 0, blue, 10);

  EXPECT_TRUE(ImagesMatch(&expected, &image, filename,
      DiffType::kTypeSideBySide)) << "    Draws a stroke of width 10 when "
      "SetWidth was called with 10.";
}

TEST(BrushTest, DrawSegmentedLine) {
  std::string filename = "BrushTest_DrawSegmentedLine_expected_vs_actual.bmp";
  remove(filename.c_str());
  Brush brush;
  graphics::Image image(100, 100);
  brush.SetColor(green);
  brush.SetWidth(20);

  // Go from corner to corner, we can ignore whether they put the endcap on.
  brush.Start(20, 20, image);
  brush.MoveTo(80, 80, image);
  brush.MoveTo(20, 80, image);
  brush.MoveTo(80, 20, image);

  graphics::Image expected(100, 100);
  expected.DrawLine(20, 20, 80, 80, green, 20);
  expected.DrawLine(80, 80, 20, 80, green, 20);
  expected.DrawLine(80, 20, 20, 80, green, 20);
  expected.DrawCircle(20, 20, 10, green);
  expected.DrawCircle(80, 80, 10, green);
  expected.DrawCircle(20, 80, 10, green);
  expected.DrawCircle(80, 20, 10, green);

  EXPECT_TRUE(ImagesMatch(&expected, &image, filename,
      DiffType::kTypeSideBySide)) << "    Draws a stroke of width 20 when "
      "SetWidth was called with 20. Includes circular endcaps after each "
      "MoveTo line segment.";
}

class BucketTest : public testing::Test {
public:
  void SetUp() override {
    // https://stackoverflow.com/questions/3803465/how-to-capture-stdout-stderr-with-googletest
    original = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());
  }

  void TearDown() override {
    std::cout.rdbuf(original);
  }

protected:
  std::streambuf* original;
  std::stringstream stream;
};

TEST_F(BucketTest, Constructor) {
  Bucket bucket;
}

TEST_F(BucketTest, GetAndSetColor) {
  Bucket bucket;
  bucket.SetColor(blue);
  EXPECT_EQ(bucket.GetColor(), blue) << "    Should return the same color for "
      "GetColor that was passed for SetColor";
}

void Fills1x1Image() {
  graphics::Image image(1, 1);
  Bucket bucket;
  bucket.SetColor(green);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(0, 0, image);
  });
  EXPECT_TRUE(ImageIsColor(image, green)) << "    Should fill a 1x1 image with a "
      "solid color.";
}

TEST_F(BucketTest, Fills1x1Image) {
  ASSERT_EXIT((Fills1x1Image(),exit(0)),::testing::ExitedWithCode(0),".*")
      << "    Should not crash or infinitely loop when filling a 1x1 image";
  Fills1x1Image();
}

void FillDoesNotGoOutOfBounds(std::stringstream& stream) {
  graphics::Image image(1, 1);
  Bucket bucket;
  bucket.SetColor(green);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(0, 0, image);
  });
  EXPECT_THAT(stream.str(), Not(testing::HasSubstr("is out of bounds.")))
      << "    Your bucket should not try to fill pixels out of the image bounds.";
}

TEST_F(BucketTest, FillDoesNotGoOutOfBounds) {
  ASSERT_EXIT((FillDoesNotGoOutOfBounds(stream),exit(0)),::testing::ExitedWithCode(0),".*")
      << "    Iterative fill shouldn't be recursive and shouldn't crash.";
  FillDoesNotGoOutOfBounds(stream);
}

TEST_F(BucketTest, FillOnExistingColor) {
  ASSERT_EXIT((FillWhiteOnWhite(),exit(0)),::testing::ExitedWithCode(0),".*")
      << "    Should not take any action when filling starts at a pixel that's already the target color. Add a base case?";
  FillWhiteOnWhite();
}

void Fills10x10Image() {
  graphics::Image image(10, 10);
  Bucket bucket;
  bucket.SetColor(green);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(0, 0, image);
  });
  EXPECT_TRUE(ImageIsColor(image, green)) << "    Should fill a 10x10 image with a "
      "solid color from the top left corner.";

  bucket.SetColor(blue);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(9, 9, image);
  });
  EXPECT_TRUE(ImageIsColor(image, blue)) << "    Should fill a 10x10 image with a "
      "solid color from the bottom right corner.";

  bucket.SetColor(green);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(3, 6, image);
  });
  EXPECT_TRUE(ImageIsColor(image, green)) << "    Should fill a 10x10 image with a "
      "solid color from anywhere in the center.";

  ASSERT_DURATION_LE(3, {
    bucket.Fill(6, 6, image);
  });
  // Nothing should have happened.
  EXPECT_TRUE(ImageIsColor(image, green));
}

TEST_F(BucketTest, Fills10x10Image) {
  ASSERT_EXIT((Fills10x10Image(), exit(0)),::testing::ExitedWithCode(0),".*")
  << "    Fill shouldn't be recursive, loop infinitely and shouldn't crash.";
  Fills10x10Image();
}

void FillRegion() {
  std::string filename = "FillRegion_expected_vs_actual.bmp";
  remove(filename.c_str());
  graphics::Image image(50, 50);
  image.DrawCircle(25, 25, 25, blue);
  image.DrawCircle(25, 25, 22, white);

  Bucket bucket;
  bucket.SetColor(green);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(25, 25, image);
  });

  graphics::Image expected(50, 50);
  expected.DrawCircle(25, 25, 25, blue);
  expected.DrawCircle(25, 25, 22, green);

  EXPECT_TRUE(ImagesMatch(&expected, &image, filename,
      DiffType::kTypeSideBySide))
      << "    Should fill an outlined white circle with green.";
}

TEST_F(BucketTest, FillRegion) {
  ASSERT_EXIT((FillRegion(), exit(0)),::testing::ExitedWithCode(0),".*")
      << "    Fill shouldn't be recursive, loop infinitely and shouldn't crash.";
  FillRegion();
}

void FillOverflow() {
  graphics::Image image(3, 3);
  image.SetColor(0, 0, blue);
  image.SetColor(2, 2, blue);

  Bucket bucket;
  bucket.SetColor(green);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(0, 2, image);
  });
  EXPECT_EQ(image.GetColor(0, 2), green) << "    Should fill all connected pixels";
  EXPECT_EQ(image.GetColor(0, 1), green) << "    Should fill all connected pixels";
  EXPECT_EQ(image.GetColor(1, 2), green) << "    Should fill all connected pixels";
  EXPECT_EQ(image.GetColor(0, 0), blue) << "    Should not fill pixels different from the start";
  EXPECT_EQ(image.GetColor(1, 1), green) << "    Should fill all connected pixels";
  EXPECT_EQ(image.GetColor(2, 2), blue) << "    Should not fill pixels different from the start";
  EXPECT_EQ(image.GetColor(1, 0), green) << "    Should fill all connected pixels";
  EXPECT_EQ(image.GetColor(2, 0), green) << "    Should fill all connected pixels";
  EXPECT_EQ(image.GetColor(2, 1), green) << "    Should fill all connected pixels";
}

TEST_F(BucketTest, FillOverflow) {
  ASSERT_EXIT((FillOverflow(), exit(0)),::testing::ExitedWithCode(0),".*")
      << "    Fill shouldn't be recursive, loop infinitely and shouldn't crash.";
  FillOverflow();
}

void FillStopsAtDiagonals() {
  graphics::Image image(3, 3);
  image.SetColor(0, 0, blue);
  image.SetColor(1, 1, blue);
  image.SetColor(2, 2, blue);

  Bucket bucket;
  bucket.SetColor(green);
  ASSERT_DURATION_LE(3, {
    bucket.Fill(0, 2, image);
  });
  EXPECT_EQ(image.GetColor(0, 2), green);
  EXPECT_EQ(image.GetColor(0, 1), green);
  EXPECT_EQ(image.GetColor(1, 2), green);
  EXPECT_EQ(image.GetColor(0, 0), blue) << "    Should not fill pixels different from the start";
  EXPECT_EQ(image.GetColor(1, 1), blue) << "    Should not fill pixels different from the start";
  EXPECT_EQ(image.GetColor(2, 2), blue) << "    Should not fill pixels different from the start";
  EXPECT_EQ(image.GetColor(1, 0), white) << "    Should not fill across diagonals";
  EXPECT_EQ(image.GetColor(2, 0), white) << "    Should not fill across diagonals";
  EXPECT_EQ(image.GetColor(2, 1), white) << "    Should not fill across diagonals";
}

TEST_F(BucketTest, FillStopsAtDiagonals) {
  ASSERT_EXIT((FillStopsAtDiagonals(), exit(0)),::testing::ExitedWithCode(0),".*")
      << "    Fill shouldn't be recursive, loop infinitely and shouldn't crash.";
  FillStopsAtDiagonals();
}

TEST(ColorToolTest, IsBaseClass) {
  bool is_pencil = std::is_base_of<ColorTool, Pencil>::value;
  EXPECT_TRUE(is_pencil) << "    Pencil should inherit from ColorTool";
  bool is_brush = std::is_base_of<ColorTool, Brush>::value;
  EXPECT_TRUE(is_brush) << "    Brush should inherit from ColorTool";
  bool is_bucket = std::is_base_of<ColorTool, Bucket>::value;
  EXPECT_TRUE(is_bucket) << "    Bucket should inherit from ColorTool";
}

TEST(ColorToolTest, HasSetColorFunctionFromBaseClass) {
  Pencil pencil;
  Brush brush;
  Bucket bucket;
  std::vector<ColorTool*> tools;
  tools.push_back(&pencil);
  tools.push_back(&brush);
  tools.push_back(&bucket);
  for (auto tool : tools) {
    tool->SetColor(blue);
  }
  for (auto tool : tools) {
    EXPECT_EQ(tool->GetColor(), blue) << "    ColorTool class should have "
        "GetColor which gets the color which was set in SetColor";
  }
}

TEST(PathToolTest, IsBaseClass) {
  bool is_pencil = std::is_base_of<PathTool, Pencil>::value;
  EXPECT_TRUE(is_pencil) << "    Pencil should inherit from PathTool";
  bool is_brush = std::is_base_of<PathTool, Brush>::value;
  EXPECT_TRUE(is_brush) << "    Brush should inherit from PathTool";
}

TEST(ColorToolTest, HasDrawingFunctionsFromBaseClass) {
  graphics::Image image(10, 10);
  Pencil pencil;
  Brush brush;
  std::vector<PathTool*> tools;
  tools.push_back(&pencil);
  tools.push_back(&brush);
  for (auto tool : tools) {
    tool->Start(2, 2, image);
    tool->MoveTo(4, 4, image);
  }
}

TEST(PaintProgramTest, ImageIs500By500) {
  PaintProgram paint_program;
  paint_program.Initialize();
  ASSERT_EQ(paint_program.GetImageForTesting()->GetWidth(), 500);
  ASSERT_EQ(paint_program.GetImageForTesting()->GetHeight(), 500);
}

TEST(PaintProgramTest, StartShowsImage) {
  int secs = 1;
  std::promise<bool> completed;
  auto stmt_future = completed.get_future();
  std::thread([&](std::promise<bool>& completed) {
    PaintProgram paint_program;
    paint_program.Initialize();
    paint_program.Start();
    completed.set_value(true);
  }, std::ref(completed)).detach();
  if(stmt_future.wait_for(std::chrono::seconds(secs)) == std::future_status::timeout) {
    if (::testing::Test::HasFatalFailure()) FAIL();
  } else {
    FAIL() << "    Start() should call image_.ShowUntilClosed().";
  }
}

bool RegionHasNoWhite(graphics::Image& image, int x, int y, int width,
                      int height) {
  for (int i = x; i < x + width; i++) {
    for (int j = y; j < y + height; j++) {
      if (white == image.GetColor(i, j)) {
        return false;
      }
    }
  }
  return true;
}

bool RegionHasColor(graphics::Image& image, const graphics::Color& color, int x,
                    int y, int width, int height) {
  for (int i = x; i < x + width; i++) {
    for (int j = y; j < y + height; j++) {
      if (color == image.GetColor(i, j)) return true;
    }
  }
  return false;
}

class TestButtonListener : public ButtonListener {
 public:
  // Overridden from ButtonListener
  void SetActiveColor(const graphics::Color& color,
                      Button* color_button) override {
    last_button_ = color_button;
    last_color_ = color;
  }

  // Overridden from ButtonListener
  void SetActiveTool(ToolType type, Button* tool_button) override {
    last_button_ = tool_button;
    last_tool_ = type;
  }

  Button* GetLastButton() { return last_button_; }
  ToolType GetLastToolType() { return last_tool_; }
  graphics::Color GetLastColor() { return last_color_; }

 private:
  Button* last_button_ = nullptr;
  ToolType last_tool_ = ToolType::kBucket;
  graphics::Color last_color_;
};

class ConcreteButton : public Button {
 public:
  ConcreteButton(int x, int y, int width, int height,
                 TestButtonListener* listener)
      : Button(x, y, width, height, listener) {}
  void DoAction() override { action_count_++; }
  int GetActionCount() const { return action_count_; }

 private:
  int action_count_ = 0;
};

TEST(SimpleButtonTest, ConstructorAndGetters) {
  TestButtonListener listener;
  ConcreteButton button(20, 5, 15, 10, &listener);
  ASSERT_EQ(20, button.GetX())
      << "    Button class should return the x coordinate passed as the first "
         "parameter into the constructor with GetX().";
  ASSERT_EQ(5, button.GetY())
      << "    Button class should return the y coordinate passed as the second "
         "parameter into the constructor with GetY().";
  ASSERT_EQ(15, button.GetWidth())
      << "    Button class should return the width passed as the third "
         "parameter into the constructor with GetWidth().";
  ASSERT_EQ(10, button.GetHeight())
      << "    Button class should return the height passed as the fourth "
         "parameter into the constructor with GetHeight().";
}

TEST(SimpleButtonTest, DrawsEntireButtonRegion) {
  remove("DrawsEntireButtonRegion.bmp");
  TestButtonListener listener;
  ConcreteButton button(10, 20, 50, 30, &listener);
  graphics::Image image(60, 60);
  button.Draw(image);
  bool has_no_white = RegionHasNoWhite(image, 10, 20, 50, 30);
  EXPECT_TRUE(has_no_white)
      << "    The Button::Draw function should cover the region from (x, y) in "
         "the top left with width and height in a color that is not white. At "
         "least one white pixel was found. See DrawsEntireButtonRegion.bmp for "
         "an image of your button.";
  if (!has_no_white) {
    image.SaveImageBmp("DrawsEntireButtonRegion.bmp");
  }
}

TEST(SimpleButtonTest, DrawDoesNotDrawOutsideButtonBounds) {
  TestButtonListener listener;
  ConcreteButton button(0, 0, 40, 40, &listener);
  graphics::Image image(41, 41);

  std::streambuf* original_out = std::cout.rdbuf();
  std::stringstream stream_out;
  std::cout.rdbuf(stream_out.rdbuf());

  button.Draw(image);

  std::cout.rdbuf(original_out);

  ASSERT_THAT(stream_out.str(), Not(testing::HasSubstr("is out of bounds")))
      << "    Your Button should not draw outside its specified bounds.";

  image.Initialize(42, 42);
  button.Draw(image);

  for (int i = 0; i < 42; i++) {
    ASSERT_EQ(white, image.GetColor(41, i))
        << "    Button::Draw should not draw pixels larger than the height, "
           "but found a pixel colored at height + 1.";
    ASSERT_EQ(white, image.GetColor(i, 41))
        << "    Button::Draw should not draw pixels larger than the width, but "
           "found a pixel colored at width + 1.";
  }
}

TEST(ToolButtonTest, IsBaseClass) {
  bool is_button = std::is_base_of<Button, ToolButton>::value;
  EXPECT_TRUE(is_button) << "    ToolButton should inherit from Button";
}

TEST(ToolButtonTest, GetToolType) {
  TestButtonListener listener;
  ToolButton button(0, 0, 5, 5, &listener, ToolType::kBrush);
  ASSERT_EQ(ToolType::kBrush, button.GetToolType())
      << "    ToolButton::GetToolType should return the tool type passed into "
         "the constructor.";

  ToolButton button1(0, 0, 5, 5, &listener, ToolType::kBucket);
  ASSERT_EQ(ToolType::kBucket, button1.GetToolType())
      << "    ToolButton::GetToolType should return the tool type passed into "
         "the constructor.";
}

TEST(ToolButtonTest, DrawsDifferentlyFromBaseClass) {
  TestButtonListener listener;
  ToolButton tb(10, 10, 30, 20, &listener, ToolType::kPencil);
  ConcreteButton cb(10, 10, 30, 20, &listener);
  graphics::Image image1(50, 50);
  graphics::Image image2(50, 50);
  tb.Draw(image1);
  cb.Draw(image2);
  bool identical = true;
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      if (image1.GetColor(i, j) != image2.GetColor(i, j)) {
        identical = false;
        break;
      }
    }
  }
  ASSERT_FALSE(identical)
      << "    ToolButton should override the Draw function to draw the name of "
         "the button over top of the Button::Draw.";
}

TEST(ToolButtonTest, DrawsEntireButtonRegion) {
  remove("ToolButton_DrawsEntireButtonRegion.bmp");
  TestButtonListener listener;
  ToolButton button(10, 20, 50, 30, &listener, ToolType::kBucket);
  graphics::Image image(60, 60);
  button.Draw(image);
  bool has_no_white = RegionHasNoWhite(image, 10, 20, 50, 30);
  EXPECT_TRUE(has_no_white)
      << "    The ToolButton::Draw function should cover the region from (x, "
         "y) in the top left with width and height in a color that is not "
         "white. At least one white pixel was found. See "
         "ToolButton_DrawsEntireButtonRegion.bmp for an image of your button.";
  if (!has_no_white) {
    image.SaveImageBmp("ToolButton_DrawsEntireButtonRegion.bmp");
  }
}

TEST(ToolButtonTest, DoAction) {
  TestButtonListener listener;
  ToolButton tb(40, 50, 10, 10, &listener, ToolType::kBrush);
  tb.DoAction();
  ASSERT_EQ(ToolType::kBrush, listener.GetLastToolType())
      << "    ToolButton::DoAction should pass the tool type from the "
         "constructor to the ButtonListener (stored in a member variable).";
  ASSERT_EQ(&tb, listener.GetLastButton())
      << "    ToolButton::DoAction should pass a pointer to itself (i.e. "
         "`this`) as a parameter to the ButtonListener in DoAction.";

  ToolButton tb2(4, 8, 12, 16, &listener, ToolType::kPencil);
  tb2.DoAction();
  ASSERT_EQ(ToolType::kPencil, listener.GetLastToolType())
      << "    ToolButton::DoAction should pass the tool type from the "
         "constructor to the ButtonListener (stored in a member variable).";
  ASSERT_EQ(&tb2, listener.GetLastButton())
      << "    ToolButton::DoAction should pass a pointer to itself (i.e. "
         "`this`) as a parameter to the ButtonListener in DoAction.";
}

TEST(ColorButtonTest, IsBaseClass) {
  bool is_button = std::is_base_of<Button, ColorButton>::value;
  EXPECT_TRUE(is_button) << "    ColorButton should inherit from Button";
}

TEST(ColorButtonTest, GetColor) {
  TestButtonListener listener;
  ColorButton button(0, 0, 5, 5, &listener, yellow);
  ASSERT_EQ(yellow, button.GetColor())
      << "    ColorButton::GetToolType should return the color passed into the "
         "constructor.";

  ColorButton button1(0, 0, 5, 5, &listener, blue);
  ASSERT_EQ(blue, button1.GetColor())
      << "    ColorButton::GetToolType should return the color passed into the "
         "constructor.";
}

TEST(ColorButtonTest, DrawsDifferentlyFromBaseClass) {
  TestButtonListener listener;
  ColorButton cb(10, 10, 20, 30, &listener, red);
  ConcreteButton b(10, 10, 20, 30, &listener);
  graphics::Image image1(50, 50);
  graphics::Image image2(50, 50);
  cb.Draw(image1);
  b.Draw(image2);
  bool identical = true;
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < 50; j++) {
      if (image1.GetColor(i, j) != image2.GetColor(i, j)) {
        identical = false;
        break;
      }
    }
  }
  ASSERT_FALSE(identical)
      << "    ColorButton should override the Draw function to draw the name "
         "of the button over top of the Button::Draw.";
  ASSERT_TRUE(RegionHasColor(image1, red, 10, 10, 20, 30))
      << "    ColorButton::Draw should draw at least one pixel that is the "
         "color passed in the constructor within the button.";
}

TEST(ColorButtonTest, DrawsEntireButtonRegion) {
  remove("ColorButtonTest_DrawsEntireButtonRegion.bmp");
  TestButtonListener listener;
  ColorButton button(10, 15, 40, 30, &listener, blue);
  graphics::Image image(60, 60);
  button.Draw(image);
  bool has_no_white = RegionHasNoWhite(image, 10, 15, 40, 30);
  EXPECT_TRUE(has_no_white)
      << "    The ColorButton::Draw function should cover the region from (x, "
         "y) in the top left with width and height in a color that is not "
         "white. At least one white pixel was found. See "
         "ColorButton_DrawsEntireButtonRegion.bmp for an image of your button.";
  if (!has_no_white) {
    image.SaveImageBmp("ColorButton_DrawsEntireButtonRegion.bmp");
  }
  ASSERT_TRUE(RegionHasColor(image, blue, 10, 15, 40, 30))
      << "    ColorButton::Draw should draw at least one pixel that is the "
         "color passed in the constructor within the button.";
}

TEST(ColorButtonTest, DoAction) {
  TestButtonListener listener;
  ColorButton b1(10, 15, 40, 30, &listener, green);
  b1.DoAction();
  ASSERT_EQ(green, listener.GetLastColor())
      << "    ColorButton::DoAction should pass the color from the constructor "
         "to the ButtonListener (stored in a member variable).";
  ASSERT_EQ(&b1, listener.GetLastButton())
      << "    ColorButton::DoAction should pass a pointer to itself (i.e. "
         "`this`) as a parameter to the ButtonListener in DoAction.";

  ColorButton b2(10, 15, 40, 30, &listener, yellow);
  b2.DoAction();
  ASSERT_EQ(yellow, listener.GetLastColor())
      << "    ColorButton::DoAction should pass the color from the constructor "
         "to the ButtonListener (stored in a member variable).";
  ASSERT_EQ(&b2, listener.GetLastButton())
      << "    ColorButton::DoAction should pass a pointer to itself (i.e. "
         "`this`) as a parameter to the ButtonListener in DoAction.";
}

TEST(DidHandleEvent, SimpleClickOnButton) {
  TestButtonListener listener;
  ConcreteButton button(20, 30, 10, 10, &listener);
  ASSERT_EQ(0, button.GetActionCount())
      << "   Something's wrong with this test if this line fails.";
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(25, 35, graphics::MouseAction::kPressed)))
      << "    Button::DidHandleEvent should return true for a kPressed event "
         "that occurs over the button boundary.";
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(25, 35, graphics::MouseAction::kReleased)))
      << "    Button::DidHandleEvent should return true for a kReleased event "
         "that occurs over the button boundary after it was pressed.";
  ASSERT_EQ(1, button.GetActionCount())
      << "    Button::DidHandleEvent should call DoAction when a press and "
         "release occur within the button bounds.";
}

TEST(DidHandleEvent, ClickOnButtonAndDragOutsideButton) {
  TestButtonListener listener;
  ConcreteButton button(20, 30, 10, 10, &listener);
  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(25, 35, graphics::MouseAction::kMoved)))
      << "    Button::DidHandleEvent should return false for a kMoved event "
         "because nothing is pushed.";
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(25, 35, graphics::MouseAction::kPressed)))
      << "    Button::DidHandleEvent should return true for a kPressed event "
         "that occurs over the button boundary.";
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(1, 1, graphics::MouseAction::kDragged)))
      << "    Button::DidHandleEvent should return true for a kDragged event "
         "that occurs outside of the button boundary after it was pressed "
         "within the boundary.";
  // Not specified whether this should be captured or not, so decided not to
  // test it.
  button.DidHandleEvent(
      graphics::MouseEvent(1, 1, graphics::MouseAction::kReleased));
  ASSERT_EQ(0, button.GetActionCount())
      << "    Button::DidHandleEvent should not call DoAction when a press "
         "occurs within the button bounds and a release out of bounds.";
}

TEST(DidHandleEvent, ClickOutsideButtonAndDragOverButton) {
  TestButtonListener listener;
  ConcreteButton button(20, 30, 10, 10, &listener);
  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(2, 2, graphics::MouseAction::kPressed)))
      << "    Button::DidHandleEvent should return false for a kPressed event "
         "that occurs outside the button boundary.";
  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(25, 35, graphics::MouseAction::kDragged)))
      << "    Button::DidHandleEvent should return false for a kDragged event "
         "that occurs inside the button boundary after it was pressed outside "
         "the boundary.";
  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(1, 1, graphics::MouseAction::kReleased)))
      << "    Button::DidHandleEvent should return false for a kReleased event "
         "that occurs inside the button boundary after the press started "
         "outside the boundary.";
  ASSERT_EQ(0, button.GetActionCount())
      << "    Button::DidHandleEvent should not call DoAction when a press "
         "occurs outside the button bounds and a release inside of bounds.";
}

TEST(DidHandleEvent, ClickOnButtonAndDragOffAndBackOn) {
  TestButtonListener listener;
  ConcreteButton button(20, 30, 10, 10, &listener);
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(25, 35, graphics::MouseAction::kPressed)))
      << "    Button::DidHandleEvent should return true for a kPressed event "
         "that occurs over the button boundary.";
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(1, 1, graphics::MouseAction::kDragged)))
      << "    Button::DidHandleEvent should return true for a kDragged event "
         "that occurs outside of the button boundary after it was pressed "
         "within the boundary.";
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(26, 36, graphics::MouseAction::kDragged)))
      << "    Button::DidHandleEvent should return true for a kDragged event "
         "that occurs inside of the button boundary after it was pressed "
         "within the boundary.";
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(26, 36, graphics::MouseAction::kReleased)))
      << "    Button::DidHandleEvent should return true for a kReleased event "
         "that occurs inside the button boundary after the press started "
         "inside the boundary.";
  ASSERT_EQ(1, button.GetActionCount())
      << "    Button::DidHandleEvent should call DoAction when a press occurs "
         "within the button bounds and a release is also within bounds.";
}

TEST(DidHandleEvent, ClicksOffButtonAndDragsBackOnAfterClick) {
  TestButtonListener listener;
  ConcreteButton button(50, 30, 10, 10, &listener);
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(55, 35, graphics::MouseAction::kPressed)))
      << "    Button::DidHandleEvent should return true for a kPressed event "
         "that occurs over the button boundary.";
  ASSERT_TRUE(button.DidHandleEvent(
      graphics::MouseEvent(56, 36, graphics::MouseAction::kReleased)))
      << "    Button::DidHandleEvent should return true for a kReleased event "
         "that occurs inside the button boundary after the press started "
         "inside the boundary.";
  ASSERT_EQ(1, button.GetActionCount())
      << "    Button::DidHandleEvent should call DoAction when a press occurs "
         "within the button bounds and a release is also within bounds.";

  // Move off the button.
  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(51, 31, graphics::MouseAction::kMoved)))
      << "    Button::DidHandleEvent should return false for any kMoved event.";
  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(1, 1, graphics::MouseAction::kMoved)))
      << "    Button::DidHandleEvent should return false for any kMoved event.";

  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(1, 1, graphics::MouseAction::kPressed)))
      << "    Button::DidHandleEvent should return false for a kPressed event "
         "that occurs outside the button boundary.";
  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(54, 31, graphics::MouseAction::kDragged)))
      << "    Button::DidHandleEvent should return false for a kDragged event "
         "that occurs over the button boundary when the press was outside the "
         "button.";
  ASSERT_FALSE(button.DidHandleEvent(
      graphics::MouseEvent(54, 31, graphics::MouseAction::kReleased)))
      << "    Button::DidHandleEvent should return false for a kReleased event "
         "that occurs over the button boundary when the press was outside the "
         "button.";
  ASSERT_EQ(1, button.GetActionCount())
      << "    Button::DidHandleEvent should not call DoAction when a press "
         "occurs "
         "outside the button bounds and a release is within bounds.";
}

TEST(FullButtonTest, TestsBothTypesOfButtonsDoActions) {
  TestButtonListener listener;
  ColorButton cb(0, 0, 5, 5, &listener, red);
  ToolButton tb(10, 10, 5, 5, &listener, ToolType::kBrush);

  ASSERT_TRUE(cb.DidHandleEvent(
      graphics::MouseEvent(2, 2, graphics::MouseAction::kPressed)))
      << "    Button::DidHandleEvent should return true for a kPressed event "
         "that occurs over the button boundary.";
  ASSERT_TRUE(cb.DidHandleEvent(
      graphics::MouseEvent(3, 3, graphics::MouseAction::kReleased)))
      << "    Button::DidHandleEvent should return true for a kReleased event "
         "that occurs inside the button boundary after the press started "
         "inside the boundary.";
  ASSERT_EQ(red, listener.GetLastColor())
      << "    Button::DidHandleEvent should call DoAction which in the case of "
         "a ColorButton calls SetActiveColor on the listener.";
  ASSERT_EQ(listener.GetLastButton(), &cb)
      << "    Button::DidHandleEvent should call DoAction which in the case of "
         "a ColorButton calls SetActiveColor on the listener.";

  ASSERT_TRUE((&tb)->DidHandleEvent(
      graphics::MouseEvent(11, 12, graphics::MouseAction::kPressed)))
      << "    Button::DidHandleEvent should return true for a kPressed event "
         "that occurs over the button boundary.";
  ASSERT_TRUE((&tb)->DidHandleEvent(
      graphics::MouseEvent(13, 13, graphics::MouseAction::kReleased)))
      << "    Button::DidHandleEvent should return true for a kReleased event "
         "that occurs inside the button boundary after the press started "
         "inside the boundary.";
  ASSERT_EQ(ToolType::kBrush, listener.GetLastToolType())
      << "    Button::DidHandleEvent should call DoAction which in the case of "
         "a ToolButton calls SetActiveTool on the listener.";
  ASSERT_EQ(listener.GetLastButton(), &tb)
      << "    Button::DidHandleEvent should call DoAction which in the case of "
         "a ColorButton calls SetActiveColor on the listener.";
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  bool skip = true;
  for (int i = 0; i < argc; i++)
  {
    if (std::string(argv[i]) == "--noskip")
    {
      skip = false;
    }
  }
  ::testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
