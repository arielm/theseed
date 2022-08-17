#pragma once

#include "PlaybackGroup.h"
#include "PlaybackScreen.h"

#include "chr/gl/BatchMap.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/xf/FontManager.h"

class Sketch;
class TreeManager;

class UI : public std::enable_shared_from_this<UI>
{
public:
  struct Mouse
  {
    bool pressed;
    glm::vec2 position;
  };

  UI(Sketch *sketch);

  Sketch *sketch;
  float size;
  glm::vec2 windowSize;
  float scale;
  Mouse mouse;
  Button::Style defaultStyle;

  chr::gl::shaders::ColorShader &colorShader;
  chr::gl::shaders::TextureAlphaShader &textureAlphaShader;

  chr::xf::FontManager fontManager;

  std::shared_ptr<chr::xf::Font> mainFont;
  chr::xf::FontSequence mainFontSequence;

  std::shared_ptr<chr::xf::Font> subFont;
  chr::xf::FontSequence subFontSequence;

  chr::gl::IndexedVertexBatch<chr::gl::XYZ.RGBA> flatBatch;
  chr::gl::IndexedBatchMap<chr::gl::XYZ.UV.RGBA> batchMap;
  chr::gl::VertexBatch<chr::gl::XYZ.RGBA> lineBatch;

  std::shared_ptr<PlaybackScreen> playbackScreen;
  std::shared_ptr<PlaybackGroup> playbackGroup;

  std::shared_ptr<TreeManager> treeManager;

  std::map<std::string, chr::gl::Texture> textures;

  float mainFontSize = 20;
  float subFontSize = 25;
  float subFontShift = 2.5f;
  float gap = 5;

  void setup();
  void enter(int artistIndex);
  void resize(float width, float height);

  void run();
  void draw();

  void mouseMoved(float x, float y);
  void mousePressed(float x, float y);

  void beginGroupDraw();
  void endGroupDraw();

  void drawTextInRect(const std::u16string &text, const chr::math::Rectf &bounds, float shiftY = 0);
  void drawFrame(const chr::math::Rectf &bounds, const glm::vec4 &color);

  glm::mat4 getScreenMatrix();

protected:
  void loadTexture(const std::string &name, const chr::InputSource &inputSource);
  glm::vec2 transformCoordinates(float x, float y) const;
};
