#include "AboutPage.h"
#include "Sketch.h"

#include "chr/gl/draw/Rect.h"
#include "chr/Random.h"

#include "PoissonGenerator.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float FONT_SIZE = 20;
static constexpr float LEADING = 1.2f;
static constexpr float X1 = -350;
static constexpr float X2 = +350;
static constexpr float MARGIN = 160;
static constexpr float MORPH_DURATION = 0.5f;

static const string TEXT[3] =
{
    "In the context of coding, a seed points to the place where we start. Choosing *the seed* as the name of our algorithm driven generative art publication marks both our desire to start exactly where we find ourselves in this moment, understanding that our microcosm is a reflection of the world at large, and our commitment to supporting artists in the fx(hash) space.",
    "More specifically, we wish to create community by showcasing a diverse group of artists in a format that is itself creative, interactive and self-aware. While the writing will be kept to a minimum, the intention is that the words contribute to capturing the essence of our complex marketplace, populated by a growing number of incredible humans, and reflect it back to readers/viewers/players not only with lightheartedness, poetic whimsy, but also meaning.",
    "Symbolically, a seed represents latent potential and also holds a much needed quality of earthiness. Many seeds in nature are tiny, regardless of the tree they hold within their DNA. In this sense, a seed is not unlike a concentrated point of awareness, or a dot in a vast expanse of possibilities: it exists unencumbered by any taxonomical baggage. As iterations are minted, *the seed* will sprout and take on a life of its own. We are foremost the stewards of an intention to create beauty and to hold space for other artists."
};

AboutPage::AboutPage(Sketch *sketch)
:
Page(sketch)
{}

void AboutPage::setup()
{
    font1 = sketch->fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties2d());
    font1->setShader(sketch->textureAlphaShader);
    font1->setSize(FONT_SIZE);

    font2 = sketch->fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties2d());
    font2->setShader(sketch->textureAlphaShader);

    //

    for (int i = 0; i < 3; i++)
    {
        paragraphsHeights.push_back(layout(TEXT[0], X1, X2, LEADING));
    }

    float totalHeight = paragraphsHeights[0] + MARGIN + paragraphsHeights[1] + MARGIN + paragraphsHeights[2];
    float y0 = -totalHeight / 2;
    float y1 = y0 + paragraphsHeights[0] + MARGIN;
    float y2 = y1 + paragraphsHeights[1] + MARGIN;
    paragraphsY.push_back(y0);
    paragraphsY.push_back(y1);
    paragraphsY.push_back(y2);

    //

    for (int i = 0; i < 3; i++)
    {
        units1.emplace_back();
        units2.emplace_back();

        unitsSource.emplace_back();
        unitsTarget.emplace_back();

        morphing.push_back(false);
        morphingClocks.push_back(Clock::create());
    }

    //

    overlayBatch1
        .setShader(sketch->colorShader)
        .setShaderColor(0, 0, 0, 1);

    overlayBatch2
        .setShader(sketch->colorShader)
        .setShaderColor(0, 0, 0, 1);

    setupNavigation();
    setupSideInfo();

    //

    clock->start();
}

void AboutPage::enter(int seed, int artistIndex)
{
    font1->setSize(FONT_SIZE);
    font1->setColor(0, 0, 0, 0.75f);

    if (entered)
    {
        return;
    }
    else
    {
        entered = true;
    }

    generate(seed);
}

void AboutPage::resize(float width, float height)
{
    this->width = width;
    this->height = height;

    scale = height / sketch->size;
}

void AboutPage::update()
{
    for (int i = 0; i < 3; i++)
    {
        units2[i].clear();

        if (morphing[i])
        {
            float elapsed = morphingClocks[i]->getTime();
            if (elapsed <= MORPH_DURATION)
            {
                float u = elapsed / MORPH_DURATION;

                for (int j = 0; j < unitsSource[i].size(); j++)
                {
                    auto position = u * unitsTarget[i][j].position + (1 - u) * unitsSource[i][j].position;
                    float angle = u * unitsTarget[i][j].angle + (1 - u) * unitsSource[i][j].angle;
                    float scale = u * unitsTarget[i][j].scale + (1 - u) * unitsSource[i][j].scale;

                    units2[i].emplace_back(unitsSource[i][j].text, position, angle, scale);
                }
            }
            else
            {
                for (const auto &unit: unitsTarget[i])
                {
                    units2[i].emplace_back(unit.text, unit.position, unit.angle, unit.scale);
                }
            }
        }
        else
        {
            for (const auto &unit: units1[i])
            {
                auto now = clock->getTime();
                auto position = unit.position + cosf(now * unit.clockFactor1) * 2.0f;
                float angle = (unit.angle + cosf(now * unit.clockFactor2) * 7.0f) * D2R;
                float scale = unit.scale + cosf(now * unit.clockFactor3) * 0.15f;

                units2[i].emplace_back(unit.text, position, angle, scale);
            }
        }
    }
}

void AboutPage::draw()
{
    glClearColor(1, 1, 1, 1);
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

    font1->beginSequence(sequence1);
    Matrix matrix;

    for (int i = 0; i < 3; i++)
    {
        for (const auto &unit: units2[i])
        {
            matrix
                .setTranslate(unit.position)
                .rotateZ(unit.angle)
                .scale(unit.scale);

            drawWord(unit.text, matrix, 0, 0);
        }
    }

    font1->endSequence();
    font1->replaySequence(sequence1);

    overlayBatch1.flush();

    font2->setColor(1, 1, 1, 1);
    font2->replaySequence(sequence2);

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

    font2->setColor(1, 1, 1, 1);
    font2->replaySequence(sequence3);
}

void AboutPage::mousePressed(float x, float y)
{
    auto transformed = transformCoordinates(x, y);

    if (navigationRect.contains(transformed))
    {
        sketch->gotoPage(MAP, -1);
        return;
    }

    for (int i = 0; i < 3; i++)
    {
        if (!morphing[i])
        {
            glm::vec2 center(0, paragraphsY[i] + paragraphsHeights[i] / 2);

            if (isPointInsideEllipse(transformed.x, transformed.y, center.x, center.y, 260, 120))
            {
                morphing[i] = true;
                morphingClocks[i]->start();

                unitsSource[i] = units2[i];
                unitsTarget[i] = wrap(TEXT[i], X1, X2, paragraphsY[i], LEADING);
            }
        }
    }
}

void AboutPage::generate(int seed)
{
    Random rnd1(seed);
    Random rnd2(seed);
    Random rnd3(seed);
    Random rnd4(seed);
    PoissonGenerator::DefaultPRNG PRNG(seed);

    for (int i = 0; i < 3; i++)
    {
        auto words = utils::split(TEXT[i], ' ');
        glm::vec2 center(0, paragraphsY[i] + paragraphsHeights[i] / 2);
        const auto points = PoissonGenerator::generatePoissonPoints(words.size(), PRNG, true);

        units1[i].clear();
        int index = 0;

        for (const auto &word : words)
        {
            float x = points[index].x * 520 - 260;
            float y = points[index].y * 240 - 120;
            float angle = rnd2.nextFloat(-7, +7);
            float scale = rnd3.nextFloat(0.85f, 1.15f);
            float clockFactor1 = rnd4.nextFloat(5.0f, 10.0f);
            float clockFactor2 = rnd4.nextFloat(0.5f, 1.0f);
            float clockFactor3 = rnd4.nextFloat(2.5f, 5.0f);

            units1[i].emplace_back(utils::to<u16string>(word), glm::vec2(center.x + x, center.y + y), angle, scale, clockFactor1, clockFactor2, clockFactor3);
            index++;
        }
    }
}

glm::vec2 AboutPage::transformCoordinates(float x, float y) const
{
    return glm::vec2(x - width * 0.5f, y - height * 0.5f) / scale;
}

bool AboutPage::isPointInsideEllipse(float x, float y, float h, float k, float a, float b)
{
    auto p = (powf((x - h), 2) / powf(a, 2)) + (powf((y - k), 2) / powf(b, 2));
    return p <= 1.1f;
}

float AboutPage::layout(const std::string &text, float x1, float x2, float leading)
{
    auto words = utils::split(text, ' ');
    float spaceWidth = font1->getCharAdvance(' ');
    float lineHeight = font1->getHeight();
    float x = x1;
    float height = lineHeight;

    for (const auto &word : words)
    {
        auto word16 = utils::to<u16string>(word);
        float wordAdvance = font1->getStringAdvance(word16);

        if (x + wordAdvance >= x2)
        {
            x = x1;
            height += lineHeight * leading;
        }

        x += wordAdvance + spaceWidth;
    }

    return height;
}

vector<Unit2> AboutPage::wrap(const string &text, float x1, float x2, float y1, float leading)
{
    vector<Unit2> units;

    auto words = utils::split(text, ' ');
    float spaceWidth = font1->getCharAdvance(' ');
    float lineHeight = font1->getHeight();
    float x = x1;
    float y = y1;

    for (const auto &word : words)
    {
        auto word16 = utils::to<u16string>(word);
        float wordAdvance = font1->getStringAdvance(word16);

        if (x + wordAdvance >= x2)
        {
            x = x1;
            y += lineHeight * leading;
        }

        units.emplace_back(word16, glm::vec2(x + wordAdvance * 0.5f, y), 0, 1);
        x += wordAdvance + spaceWidth;
    }

    return units;
}

void AboutPage::drawWord(const u16string &text, const Matrix &matrix, float x, float y)
{
    float offsetX = font1->getOffsetX(text, XFont::ALIGN_MIDDLE);
    float offsetY = font1->getOffsetY(XFont::ALIGN_MIDDLE);

    for (auto c : text)
    {
        int glyphIndex = font1->getGlyphIndex(c);
        float w = font1->getGlyphAdvance(glyphIndex);

        if (glyphIndex >= 0)
        {
            font1->addGlyph(matrix, glyphIndex, x + offsetX, y + offsetY);
        }

        x += w;
    }
}

void AboutPage::setupNavigation()
{
    font2->setSize(20);

    auto backText = u"back";
    float backTextX = -500 + 16 + 12;
    float backTextY = -500 + 12;

    font2->beginSequence(sequence2);
    Sketch::drawText(*font2, backText, backTextX, backTextY, XFont::ALIGN_TOP);
    font2->endSequence();

    // ---

    navigationRect = math::Rectf(backTextX - 12, backTextY - 12, font2->getStringAdvance(backText) + 12 * 2,12 + 20 + 12);

    draw::Rect overlay;
    overlay
        .setBounds(navigationRect)
        .append(overlayBatch1);
}

void AboutPage::setupSideInfo()
{
    font2->setSize(30);

    auto text = u"from the editor";
    auto textX = 500 - 64 - 16 - font2->getStringAdvance(text);

    font2->beginSequence(sequence3);
    Sketch::drawText(*font2, text, textX, -16, XFont::ALIGN_BOTTOM);
    font2->endSequence();

    // ---

    draw::Rect overlay;
    overlay
        .setBounds(textX - 16, -16 - 30 - 16, font2->getStringAdvance(text) + 16 * 2, 16 + 30 + 16)
        .append(overlayBatch2);
}
