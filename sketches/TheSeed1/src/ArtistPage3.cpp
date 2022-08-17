#include "ArtistPage3.h"
#include "Sketch.h"

#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;

static constexpr float AMPLITUDE = 24;
static constexpr float PERIOD = 500; // DISTANCE BETWEEN CRESTS IN PIXELS
static constexpr float VELOCITY = 10; // TIME BETWEEN CRESTS IN PIXELS/SECOND

static constexpr float FONT_SIZE = 17;
static constexpr float INDENT = 10;

ArtistPage3::ArtistPage3(Sketch *sketch)
:
Page(sketch)
{}

void ArtistPage3::setup()
{
    font1 = sketch->fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties2d());
    font1->setShader(sketch->textureAlphaShader);

    font3 = sketch->fontManager.getFont(InputSource::resource("Menlo_Regular_64.fnt"), XFont::Properties2d());
    font3->setShader(sketch->textureAlphaShader);
    font3->setSize(FONT_SIZE);

    overlayBatch1
        .setShader(sketch->colorShader)
        .setShaderColor(0, 0, 0, 1);

    strokeBatch
        .setPrimitive(GL_LINES)
        .setShader(sketch->colorShader)
        .setShaderColor(0, 0, 0, 0.2f);

    setupNavigation();
}

void ArtistPage3::enter(int seed, int artistIndex)
{
    this->artistIndex = artistIndex;

    //

    text = utils::readText<u16string>(sketch->artists[artistIndex].codeSource);

    wrap();

    clock->restart();

    //

    overlayBatch2
        .setShader(sketch->colorShader)
        .setShaderColor(sketch->artists[artistIndex].overlayColor);

    setupSideInfo();
}

void ArtistPage3::resize(float width, float height)
{
    this->width = width;
    this->height = height;

    scale = height / sketch->size;

    wrap();
}

void ArtistPage3::draw()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    auto projectionMatrix = glm::ortho(0.0f, width, 0.0f, height);

    Matrix modelViewMatrix1;
    modelViewMatrix1
        .translate(width * 0.5f, height * 0.5f)
        .scale(+scale, -scale);

    gl::State()
        .setShaderMatrix(modelViewMatrix1 * projectionMatrix)
        .apply();

    //

    double now = clock->getTime();
    float dy = cosf(now * 4) * AMPLITUDE;

    vector<FollowablePath2D> paths;
    for (float my = -0.25f; my < 0.6f; my += 0.028f)
    {
        paths.push_back(getPath(sketch->size * my + dy, now));
    }

    //

    strokeBatch.clear();

    font3->beginSequence(sequence3);

    int lineIndex = 0;
    for (auto &path : paths)
    {
        drawPath(path, lineIndex++);
    }

    strokeBatch.flush();

    font3->setColor(0, 0, 0, 0.85f);
    font3->endSequence();
    font3->replaySequence(sequence3);

    //

    overlayBatch1.flush();

    font1->setColor(1, 1, 1, 1);
    font1->replaySequence(sequence1);

    // ---

    Matrix viewMatrix2;
    viewMatrix2
        .translate(width * 0.5f, height * 0.5f)
        .scale(+scale, -scale)
        .rotateZ(-HALF_PI)
        .translate(0, 500);

    gl::State()
        .setShaderMatrix(viewMatrix2 * projectionMatrix)
        .apply();

    overlayBatch2.flush();

    font1->setColor(sketch->artists[artistIndex].primaryColor);
    font1->replaySequence(sequence2);
}

void ArtistPage3::mousePressed(float x, float y)
{
    auto transformed = transformCoordinates(x, y);

    if (navigationRect[0].contains(transformed))
    {
        sketch->gotoPage(ARTIST2, artistIndex);
    }
    else if (navigationRect[1].contains(transformed))
    {
        sketch->gotoPage(BACK, artistIndex);
    }
}

glm::vec2 ArtistPage3::transformCoordinates(float x, float y) const
{
    return glm::vec2(x - width * 0.5f, y - height * 0.5f) / scale;
}

void ArtistPage3::setupSideInfo()
{
    font1->setSize(30);

    auto coderNameText = u"code by " + sketch->artists[artistIndex].coderName;
    auto nameText = sketch->artists[artistIndex].name;

    float coderNameTextX;
    float nameTextX;

    if (!sketch->artists[artistIndex].coderName.empty())
    {
        coderNameTextX = 500 - 64 - 16 - font1->getStringAdvance(coderNameText);
        nameTextX = coderNameTextX - 64 - 16 * 2 - font1->getStringAdvance(nameText);
    }
    else
    {
        nameTextX = 500 - 64 - 16 - font1->getStringAdvance(nameText);
    }

    font1->beginSequence(sequence2);
    if (!sketch->artists[artistIndex].coderName.empty()) Sketch::drawText(*font1, coderNameText, coderNameTextX, -16, XFont::ALIGN_BOTTOM);
    Sketch::drawText(*font1, nameText, nameTextX, -16, XFont::ALIGN_BOTTOM);
    font1->endSequence();

    // ---

    overlayBatch2.clear();

    draw::Rect overlayCoderName;
    draw::Rect overlayName;

    if (!sketch->artists[artistIndex].coderName.empty())
    {
        overlayCoderName
            .setBounds(coderNameTextX - 16, -16 - 30 - 16, font1->getStringAdvance(coderNameText) + 16 * 2, 16 + 30 + 16)
            .append(overlayBatch2);
    }

    overlayName
        .setBounds(nameTextX - 16, -16 - 30 - 16, font1->getStringAdvance(nameText) + 16 * 2, 16 + 30 + 16)
        .append(overlayBatch2);
}

void ArtistPage3::setupNavigation()
{
    font1->setSize(20);

    auto backText = u"back";
    float backTextX = -500 + 16 + 12;
    float backTextY = -500 + 12;

    auto nextText = u"next";
    float nextTextX = backTextX + font1->getStringAdvance(backText) + 12 + 16 + 12;
    float nextTextY = -500 + 12;

    font1->beginSequence(sequence1);
    Sketch::drawText(*font1, backText, backTextX, backTextY, XFont::ALIGN_TOP);
    Sketch::drawText(*font1, nextText, nextTextX, nextTextY, XFont::ALIGN_TOP);
    font1->endSequence();

    // ---

    navigationRect[0] = math::Rectf(backTextX - 12, backTextY - 12, font1->getStringAdvance(backText) + 12 * 2, 12 + 20 + 12);
    navigationRect[1] = math::Rectf(nextTextX - 12, nextTextY - 12, font1->getStringAdvance(nextText) + 12 * 2, 12 + 20 + 12);

    draw::Rect overlayBack;
    overlayBack
        .setBounds(navigationRect[0])
        .append(overlayBatch1);

    draw::Rect overlayNext;
    overlayNext
        .setBounds(navigationRect[1])
        .append(overlayBatch1);
}

void ArtistPage3::wrap()
{
    vector<FollowablePath2D> paths;
    for (float my = -0.25f; my < 0.6f; my += 0.028f)
    {
        paths.push_back(getPath(sketch->size * my, 0));
    }

    vector<float> widths;
    for (auto &path : paths)
    {
        widths.push_back(path.getLength() - INDENT);
    }

    wordWrapper.wrap(*font3, text, widths);
}

FollowablePath2D ArtistPage3::getPath(float y, float t)
{
    FollowablePath2D path;
    path.begin();

    for (float x = -sketch->size * 0.5f; x <= +sketch->size * 0.5f; x += 4)
    {
        float phase = t * VELOCITY / PI;
        path.add(x, y - AMPLITUDE * sinf(x * TWO_PI / PERIOD - phase));
    }

    path.end();
    return path;
}

void ArtistPage3::drawPath(FollowablePath2D &path, int lineIndex)
{
    const auto &points = path.getPoints();
    for (int i = 0; i < points.size() - 1; i++)
    {
        strokeBatch.addVertices(points[i].position, points[i + 1].position);
    }

    if (lineIndex >= wordWrapper.size)
    {
        return;
    }

    // ---

    int start = wordWrapper.offsets[lineIndex];
    int end = start + wordWrapper.lengths[lineIndex];

    float offsetX = INDENT;
    float offsetY = font3->getOffsetY(XFont::ALIGN_MIDDLE);
    Matrix matrix;

    while (start < end)
    {
        auto glyphIndex = font3->getGlyphIndex(text[start++]);
        auto width = font3->getGlyphAdvance(glyphIndex);

        if (glyphIndex >= 0)
        {
            path
                .offsetToValue(offsetX + width * 0.5f, width)
                .applyToMatrix(matrix);

            font3->addGlyph(matrix, glyphIndex, -width * 0.5f, offsetY);
        }

        offsetX += width;
    }
}
