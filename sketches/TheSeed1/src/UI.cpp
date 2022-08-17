#include "UI.h"
#include "Sketch.h"
#include "TreeManager.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

UI::UI(Sketch *sketch)
:
sketch(sketch),
colorShader(sketch->colorShader),
textureAlphaShader(sketch->textureAlphaShader)
{}

void UI::setup()
{
  mainFont = sketch->fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties2d()); // XXX
  subFont = sketch->fontManager.getFont(InputSource::resource("Helvetica_Regular_64.fnt"), XFont::Properties2d());

  loadTexture("frame", InputSource::resource("frame.png"));
  loadTexture("cross", InputSource::resource("cross.png"));
  loadTexture("rewind", InputSource::resource("button_rewind.png"));
  loadTexture("play", InputSource::resource("button_play.png"));
  loadTexture("pause", InputSource::resource("button_pause.png"));

  Button::StyleShortcut::load(defaultStyle,
    glm::vec4(0xcc / 255.0f, 0xcc / 255.0f, 0xcc / 255.0f, 1),
    glm::vec4(0, 0, 0, 1),
    glm::vec4(0xff / 255.0f, 0xff / 255.0f, 0x99 / 255.0f, 1),
    glm::vec4(0x60 / 255.0f, 0x60 / 255.0f, 0x60 / 255.0f, 1));

  playbackScreen = make_shared<PlaybackScreen>(shared_from_this());
  playbackGroup = make_shared<PlaybackGroup>(shared_from_this());

  treeManager = make_shared<TreeManager>();

  // ---

  lineBatch.setPrimitive(GL_LINES);
}

void UI::enter(int artistIndex)
{
    playbackScreen->setCurrentTree(treeManager->createLoadedTree(shared_from_this(), sketch->artists[artistIndex].recordSource).get());
    playbackScreen->enter();
}

void UI::resize(float width, float height)
{
  windowSize = glm::vec2(width, height);
  scale = height / sketch->size;
  size = sketch->size;

  playbackScreen->resize();
}

void UI::run()
{
  playbackScreen->run();

  mouse.pressed = false;
}

void UI::draw()
{
  playbackScreen->draw();
}

void UI::mouseMoved(float x, float y)
{
  mouse.position = transformCoordinates(x, y);
}

void UI::mousePressed(float x, float y)
{
  mouse.pressed = true;
}

void UI::beginGroupDraw()
{
  lineBatch.clear();
  flatBatch.clear();
  batchMap.begin();
  subFont->beginSequence(subFontSequence, true);
}

void UI::endGroupDraw()
{
  subFont->endSequence();

  State()
    .setShader(colorShader)
    .setShaderMatrix(getScreenMatrix())
    .glLineWidth(1)
    .apply();

  lineBatch.flush();
  flatBatch.flush();

  State()
    .setShader(textureAlphaShader)
    .setShaderMatrix(getScreenMatrix())
    .apply();

  batchMap.flush();
  subFont->replaySequence(subFontSequence);
}

void UI::drawTextInRect(const u16string &text, const Rectf &bounds, float shiftY)
{
  float offsetX = subFont->getOffsetX(text, XFont::ALIGN_MIDDLE);
  float offsetY = subFont->getOffsetY(XFont::ALIGN_MIDDLE);

  float x = offsetX + bounds.x1 + bounds.width() * 0.5f;
  float y = offsetY + bounds.y1 + bounds.height() * 0.5f + shiftY;

  for (auto c : text)
  {
    auto glyphIndex = subFont->getGlyphIndex(c);
    subFont->addGlyph(glyphIndex, x, y);
    x += subFont->getGlyphAdvance(glyphIndex);
  }
}

void UI::drawFrame(const Rectf &bounds, const glm::vec4 &color)
{
  const auto &texture = textures["frame"];
  auto &batch = batchMap.getBatch(texture);

  float scale = bounds.height() / texture.height;

  //

  Rectf leftBounds(bounds.x1, bounds.y1, 8 * scale, bounds.height());
  glm::vec2 leftCoords1(0, 0);
  glm::vec2 leftCoords2(8 / texture.width, 1);

  batch
    .addVertex(leftBounds.x1, leftBounds.y1, 0, leftCoords1.x, leftCoords1.y, color)
    .addVertex(leftBounds.x1, leftBounds.y2, 0, leftCoords1.x, leftCoords2.y, color)
    .addVertex(leftBounds.x2, leftBounds.y2, 0, leftCoords2.x, leftCoords2.y, color)
    .addVertex(leftBounds.x2, leftBounds.y1, 0, leftCoords2.x, leftCoords1.y, color);

  batch
    .addIndices(0, 1, 2, 2, 3, 0)
    .incrementIndices(4);

  //

  Rectf rightBounds(bounds.x2 - 8 * scale, bounds.y1, 8 * scale, bounds.height());
  glm::vec2 rightCoords1(1 - 8 / texture.width, 0);
  glm::vec2 rightCoords2(1, 1);

  batch
    .addVertex(rightBounds.x1, rightBounds.y1, 0, rightCoords1.x, rightCoords1.y, color)
    .addVertex(rightBounds.x1, rightBounds.y2, 0, rightCoords1.x, rightCoords2.y, color)
    .addVertex(rightBounds.x2, rightBounds.y2, 0, rightCoords2.x, rightCoords2.y, color)
    .addVertex(rightBounds.x2, rightBounds.y1, 0, rightCoords2.x, rightCoords1.y, color);

  batch
    .addIndices(0, 1, 2, 2, 3, 0)
    .incrementIndices(4);

  //

  Rectf middleBounds(bounds.x1 + 8 * scale, bounds.y1, bounds.width() - 2 * 8 * scale, bounds.height());
  glm::vec2 middleCoords1(8 / texture.width, 0);
  glm::vec2 middleCoords2(1 - 8 / texture.width, 1);

  batch
    .addVertex(middleBounds.x1, middleBounds.y1, 0, middleCoords1.x, middleCoords1.y, color)
    .addVertex(middleBounds.x1, middleBounds.y2, 0, middleCoords1.x, middleCoords2.y, color)
    .addVertex(middleBounds.x2, middleBounds.y2, 0, middleCoords2.x, middleCoords2.y, color)
    .addVertex(middleBounds.x2, middleBounds.y1, 0, middleCoords2.x, middleCoords1.y, color);

  batch
    .addIndices(0, 1, 2, 2, 3, 0)
    .incrementIndices(4);
}

glm::mat4 UI::getScreenMatrix()
{
  glm::mat4 projectionMatrix = glm::ortho(0.0f, windowSize.x, 0.0f, windowSize.y);

  Matrix viewMatrix;
  viewMatrix
    .setTranslate(windowSize * 0.5f)
    .scale(+scale, -scale);

  return viewMatrix * projectionMatrix;
}

void UI::loadTexture(const string &name, const InputSource &inputSource)
{
  textures[name] = Texture(Texture::ImageRequest(inputSource)
    .setFlags(image::FLAGS_TRANSLUCENT)
    .setMipmap(true));
}

glm::vec2 UI::transformCoordinates(float x, float y) const
{
  return (glm::vec2(x, y) - windowSize * 0.5f) / scale;
}
