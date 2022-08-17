#include "CoverPage.h"
#include "Sketch.h"

#include "chr/gl/draw/Rect.h"
#include "chr/gl/draw/Sprite.h"
#include "chr/io/BinaryInputStream.h"
#include "chr/Random.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace io;

static constexpr float R1 = 1;
static constexpr float R2 = 500;
static constexpr float TURNS = 25;

static constexpr float DOT_RADIUS_PIXELS = 56; // SPECIFIC TO "dot_112"

CoverPage::CoverPage(Sketch *sketch)
:
Page(sketch)
{}

void CoverPage::enter(int seed, int artistIndex)
{
    clock->restart();

    if (entered)
    {
        return;
    }
    else
    {
        entered = true;
        this->artistIndex = artistIndex;
    }

    // ---

    strokeBatch
        .setPrimitive(GL_LINES)
        .setShader(sketch->colorShader)
        .setShaderColor(0, 0, 0, 0.25f);

    fillBatch
        .setShader(sketch->textureShader)
        .setTexture(sketch->textures[artistIndex])
        .setShaderColor(1, 1, 1, 1);

    overlayBatch1
        .setShader(sketch->colorShader)
        .setShaderColor(sketch->artists[artistIndex].overlayColor);

    overlayBatch2
        .setShader(sketch->colorShader)
        .setShaderColor(sketch->artists[artistIndex].overlayColor);

    ringBatch1
        .setShader(sketch->textureAlphaShader)
        .setShaderColor(sketch->artists[artistIndex].backgroundColor)
        .setTexture(sketch->buttonTexture);

    ringBatch2
        .setShader(sketch->textureAlphaShader)
        .setShaderColor(sketch->artists[artistIndex].overlayColor)
        .setTexture(sketch->buttonTexture);

    arrowBatch
        .setShader(sketch->textureAlphaShader)
        .setShaderColor(sketch->artists[artistIndex].primaryColor)
        .setTexture(sketch->arrowTexture);

    dotBatch
        .setShader(sketch->textureAlphaShader)
        .setTexture(sketch->buttonTexture);

    auto primaryColor = sketch->artists[artistIndex].primaryColor;
    lineBatch
        .setPrimitive(GL_LINES)
        .setShader(sketch->colorShader)
        .setShaderColor(primaryColor.r, primaryColor.g, primaryColor.b, 0.75f);

    // ---

    loadLogo(InputSource::resource("logo.dat"));

    // ---

    font = sketch->fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties2d());
    font->setShader(sketch->textureAlphaShader);

    // ---

    setupSideInfo();

    draw::Rect overlay1;
    overlay1
        .setBounds(-500 + 64, +500 - 168, 472, 168)
        .append(overlayBatch1);

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(ringBatch1, Matrix().scale(32 / DOT_RADIUS_PIXELS));

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(ringBatch2, Matrix().scale(24 / DOT_RADIUS_PIXELS));

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(arrowBatch);

    // ---

    generate(seed);
}

void CoverPage::resize(float width, float height)
{
    this->width = width;
    this->height = height;

    scale = height / sketch->size;
}

void CoverPage::draw()
{
    auto backgroundColor = sketch->artists[artistIndex].backgroundColor;
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    auto projectionMatrix = glm::ortho(0.0f, width, 0.0f, height);

    Matrix viewMatrix1;
    viewMatrix1
        .translate(width * 0.5f, height * 0.5f)
        .scale(+scale, -scale)
        .rotateZ(clock->getTime() * 0.125f);

    gl::State()
        .setShaderMatrix(viewMatrix1 * projectionMatrix)
        .apply();

    fillBatch.flush();
    strokeBatch.flush();

    ringBatch1.flush();
    ringBatch2.flush();
    arrowBatch.flush();

    // ---

    Matrix viewMatrix2;
    viewMatrix2
        .translate(width * 0.5f, height * 0.5f)
        .scale(+scale, -scale);

    gl::State()
        .setShaderMatrix(viewMatrix2 * projectionMatrix)
        .apply();

    overlayBatch1.flush();

    // ---

    Matrix viewMatrix3;
    viewMatrix3
        .translate(width * 0.5f, height * 0.5f)
        .scale(+scale, -scale)
        .rotateZ(-HALF_PI)
        .translate(0, 500);

    gl::State()
        .setShaderMatrix(viewMatrix3 * projectionMatrix)
        .apply();

    overlayBatch2.flush();

    font->setColor(sketch->artists[artistIndex].primaryColor);
    font->replaySequence(sequence);

    // ---

    Matrix viewMatrix4;
    viewMatrix4
        .translate(width * 0.5f, height * 0.5f)
        .scale(+scale, -scale)
        .translate(-500 + 64 + 16, +500 - 136 - 16)
        .scale((472.0f - 32.0f) / logoWidth);

    gl::State()
        .setShaderMatrix(viewMatrix4 * projectionMatrix)
        .apply();

    lineBatch.flush();
    dotBatch.flush();
}

void CoverPage::mousePressed(float x, float y)
{
    sketch->gotoPage(MAP, -1);
}

void CoverPage::generate(int seed)
{
    Random rnd1(seed);
    Random rnd2(seed);
    Random rnd3(seed);

    fillBatch.clear();
    strokeBatch.clear();

    float l = TWO_PI * TURNS;
    float L = PI * TURNS * (R1 + R2);
    float dr = (R2 - R1) / l;
    float D = 0;

    Matrix matrix;

    do
    {
        D += rnd1.nextFloat(100, 200);
        float r = sqrtf(R1 * R1 + 2 * dr * D);
        float d = (r - R1) / dr;

        matrix
            .setTranslate(-sinf(d) * r, +cosf(d) * r)
            .rotateZ(d);

        float size = rnd2.nextFloat(50, 200);
        float size2 = size / 2;

        float c = 1;
        if (rnd3.nextFloat() < 0.25f)
        {
            c = 0.75f + rnd3.nextFloat(0.25f);
        }

        draw::Rect rect;
        rect
            .setBounds(-size2, -size2, size, size)
            .setColor(c, c, c, 1)
            .setTextureOffset(sketch->artists[artistIndex].textureOffset)
            .append(fillBatch, matrix);

        strokeBatch
            .addVertex(matrix.transformPoint(-size2, -size2))
            .addVertex(matrix.transformPoint(+size2, -size2))
            .addVertex(matrix.transformPoint(+size2, -size2))
            .addVertex(matrix.transformPoint(+size2, +size2))
            .addVertex(matrix.transformPoint(+size2, +size2))
            .addVertex(matrix.transformPoint(-size2, +size2))
            .addVertex(matrix.transformPoint(-size2, +size2))
            .addVertex(matrix.transformPoint(-size2, -size2));
    }
    while (D < L);
}

void CoverPage::loadLogo(const InputSource &inputSource)
{
    BinaryInputStream stream(inputSource);

    if (stream.good())
    {
        dotBatch.clear();
        lineBatch.clear();

        logoWidth = stream.read<uint32_t>();
        logoHeight = stream.read<uint32_t>();

        auto dotCount = stream.read<uint32_t>();
        for (auto i = 0; i < dotCount; i++)
        {
            auto position = stream.read<glm::vec2>();
            auto color = stream.read<glm::vec4>();
            auto radius = stream.read<float>();

            auto primaryColor = sketch->artists[artistIndex].primaryColor;
            glm::vec4 newColor(primaryColor.r, primaryColor.g, primaryColor.b, 1 - color.r);

            drawDot(position, newColor, radius);
        }

        auto lineCount = stream.read<uint32_t>();
        for (auto i = 0; i < lineCount; i++)
        {
            auto p1 = stream.read<glm::vec2>();
            auto p2 = stream.read<glm::vec2>();

            lineBatch
                .addVertex(p1)
                .addVertex(p2);
        }
    }
}

void CoverPage::drawDot(const glm::vec2 &position, const glm::vec4 &color, float radius)
{
    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .setColor(color)
        .append(dotBatch, Matrix().translate(position).scale(radius / DOT_RADIUS_PIXELS));
}

void CoverPage::setupSideInfo()
{
    font->setSize(30);

    auto issueText = u"issue 1 :: august 2022";
    auto featuringText = u"feat. " + sketch->artists[artistIndex].name;

    float featuringTextX = 500 - 64 - 16 - font->getStringAdvance(featuringText);
    float issueTextX = featuringTextX - 64 - 16 * 2 - font->getStringAdvance(issueText);

    font->beginSequence(sequence);
    drawText(issueText, issueTextX, -16, XFont::ALIGN_BOTTOM);
    drawText(featuringText, featuringTextX, -16, XFont::ALIGN_BOTTOM);
    font->endSequence();

    // ---

    draw::Rect overlayFeaturing;
    overlayFeaturing
        .setBounds(issueTextX - 16, -16 - 30 - 16, font->getStringAdvance(issueText) + 16 * 2, 16 + 30 + 16)
        .append(overlayBatch2);

    draw::Rect overlayIssue;
    overlayIssue
        .setBounds(featuringTextX - 16, -16 - 30 - 16, font->getStringAdvance(featuringText) + 16 * 2, 16 + 30 + 16)
        .append(overlayBatch2);
}

void CoverPage::drawText(const u16string &text, float x, float y, XFont::Alignment alignment)
{
    float offset = font->getOffsetY(alignment);

    for (auto c : text)
    {
        int glyphIndex = font->getGlyphIndex(c);
        float w = font->getGlyphAdvance(glyphIndex);

        if (glyphIndex >= 0)
        {
            font->addGlyph(glyphIndex, x, y + offset);
        }

        x += w;
    }
}

