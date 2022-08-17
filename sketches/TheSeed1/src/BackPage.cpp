#include "BackPage.h"
#include "Sketch.h"

#include "chr/gl/draw/Sprite.h"
#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float R1 = 1;
static constexpr float R2 = 750;
static constexpr float TURNS = 25;
static constexpr float DD1 = 1.0f;
static constexpr float DD2 = 30.0f;

static constexpr float FONT_SIZE = 25;
static constexpr float GAP = 25;

static const float START[4] {
    39480,
    32240,
    25610,
    22530
};

static const float END[4] {
    39925,
    32600,
    25875,
    23050
};

BackPage::BackPage(Sketch *sketch)
:
Page(sketch)
{}

void BackPage::setup()
{
    twitterTexture = Texture(Texture::ImageRequest("twitter.png")
                                 .setFlags(image::FLAGS_TRANSLUCENT)
                                 .setMipmap(true));

    strokeBatch
        .setPrimitive(GL_LINE_STRIP)
        .setShader(sketch->colorShader)
        .setShaderColor(29 / 255.0f, 155 / 255.0f, 240 / 255.0f, 1);

    spriteBatch
        .setShader(sketch->textureAlphaShader)
        .setShaderColor(29 / 255.0f, 155 / 255.0f, 240 / 255.0f, 1)
        .setTexture(twitterTexture);

    font = sketch->fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties2d());
    font->setShader(sketch->textureAlphaShader);

    // ---

    drawSpiral(-500, +500);

    // ---

    overlayBatch
        .setShader(sketch->colorShader)
        .setShaderColor(1, 1, 1, 1);

    setupNavigation();
}

void BackPage::enter(int seed, int artistIndex)
{
    this->artistIndex = artistIndex;

    clock->restart();
}

void BackPage::resize(float width, float height)
{
    this->width = width;
    this->height = height;

    scale = height / sketch->size;
}

void BackPage::draw()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    auto projectionMatrix = glm::ortho(0.0f, width, 0.0f, height);

    Matrix modelViewMatrix;
    modelViewMatrix
        .translate(width * 0.5f, height * 0.5f)
        .scale(+scale, -scale);

    gl::State()
        .setShaderMatrix(modelViewMatrix * projectionMatrix)
        .apply();

    // ---

    float u = clock->getTime() * 1.25f;
    if (u >= 1) {
        u = 1;
    }

    strokeBatch.flush();

    font->setColor(1, 1, 1, 1);
    font->setSize(FONT_SIZE);

    spriteBatch.clear();
    font->beginSequence(sequence1);
    drawTextOnPath(u"the seed :: issue 1", START[0] + u * (END[0] - START[0]));
    drawTextOnPath(u"made by", START[1] + u * (END[1] - START[1]));
    drawLabelOnPath(u"arielmalka", START[2] + u * (END[2] - START[2]));
    drawLabelOnPath(u"aethersovereign", START[3] + u * (END[3] - START[3]));
    font->endSequence();

    spriteBatch.flush();
    font->replaySequence(sequence1);

    // ---

    overlayBatch.flush();

    font->setColor(0, 0, 0, 1);
    font->replaySequence(sequence2);
}

void BackPage::mousePressed(float x, float y)
{
    if (navigationRect.contains(transformCoordinates(x, y)))
    {
        sketch->gotoPage(ARTIST3, artistIndex);
    }
}

glm::vec2 BackPage::transformCoordinates(float x, float y) const
{
    return glm::vec2(x - width * 0.5f, y - height * 0.5f) / scale;
}

void BackPage::drawSpiral(float x, float y)
{
    float l = TWO_PI * TURNS;
    float L = PI * TURNS * (R1 + R2);
    float dr = (R2 - R1) / l;
    float DD = (DD2 - DD1) / l;
    float D = 0;

    path.begin();

    do
    {
        float r = sqrtf(R1 * R1 + 2 * dr * D);
        float d = (r - R1) / dr;
        D += DD1 + d * DD;

        float xx = x - sinf(d) * r;
        float yy = y + cosf(d) * r;

        strokeBatch.addVertex(xx, yy);
        path.add(xx, yy);
    }
    while (D < L);

    path.end();
}

void BackPage::drawTextOnPath(const u16string &text, float offset)
{
    Matrix matrix;
    float length = path.getLength();

    for (auto c : text)
    {
        auto glyphIndex = font->getGlyphIndex(c);
        float width = font->getGlyphAdvance(glyphIndex);

        if (offset > 0 && glyphIndex > -1)
        {
            path
                .offsetToValue(offset + width * 0.5f, width)
                .applyToMatrix(matrix);

            font->addGlyph(matrix, glyphIndex, -width * 0.5f, font->getOffsetY(XFont::ALIGN_MIDDLE));
        }

        offset += width;

        if (offset > length)
        {
            break;
        }
    }
}

void BackPage::drawLabelOnPath(const u16string &text, float offset)
{
    drawSpriteOnPath(offset - GAP);
    drawTextOnPath(text, offset);
}

void BackPage::drawSpriteOnPath(float offset)
{
    Matrix matrix;
    path
        .offsetToValue(offset, 1)
        .applyToMatrix(matrix);

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(spriteBatch, matrix.scale(0.67f));
}

void BackPage::setupNavigation()
{
    font->setSize(20);

    auto backText = u"back";
    float backTextX = -500 + 16 + 12;
    float backTextY = -500 + 12;

    font->beginSequence(sequence2);
    Sketch::drawText(*font, backText, backTextX, backTextY, XFont::ALIGN_TOP);
    font->endSequence();

    // ---

    navigationRect = math::Rectf(backTextX - 12, backTextY - 12, font->getStringAdvance(backText) + 12 * 2, 12 + 20 + 12);

    draw::Rect overlay;
    overlay
        .setBounds(navigationRect)
        .append(overlayBatch);
}

