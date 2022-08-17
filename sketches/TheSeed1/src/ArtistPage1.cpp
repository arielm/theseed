#include "ArtistPage1.h"
#include "Sketch.h"

#include "chr/gl/draw/Rect.h"
#include "chr/gl/draw/Sprite.h"

using namespace std;
using namespace chr;
using namespace gl;

ArtistPage1::ArtistPage1(Sketch *sketch)
:
Page(sketch)
{}

void ArtistPage1::setup()
{
    font = sketch->fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties2d());
    font->setShader(sketch->textureAlphaShader);

    overlayBatch1
        .setShader(sketch->colorShader)
        .setShaderColor(0, 0, 0, 1);

    setupNavigation();
}

void ArtistPage1::enter(int seed, int artistIndex)
{
    this->artistIndex = artistIndex;

    //

    fillBatch
        .setShader(sketch->textureShader)
        .setTexture(sketch->textures[artistIndex])
        .setShaderColor(1, 1, 1, 1);

    fillBatch.clear();

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(fillBatch, Matrix().scale(sketch->size / sketch->textures[artistIndex].innerHeight));

    //

    overlayBatch2
        .setShader(sketch->colorShader)
        .setShaderColor(sketch->artists[artistIndex].overlayColor);

    setupSideInfo();
}

void ArtistPage1::resize(float width, float height)
{
    this->width = width;
    this->height = height;

    scale = height / sketch->size;
}

void ArtistPage1::draw()
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

    fillBatch.flush();
    overlayBatch1.flush();

    font->setColor(1, 1, 1, 1);
    font->replaySequence(sequence1);

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

    font->setColor(sketch->artists[artistIndex].primaryColor);
    font->replaySequence(sequence2);
}

void ArtistPage1::mousePressed(float x, float y)
{
    auto transformed = transformCoordinates(x, y);

    if (navigationRect[0].contains(transformed))
    {
        sketch->gotoPage(MAP, -1);
    }
    else if (navigationRect[1].contains(transformed))
    {
        sketch->gotoPage(ARTIST2, artistIndex);
    }
}

glm::vec2 ArtistPage1::transformCoordinates(float x, float y) const
{
    return glm::vec2(x - width * 0.5f, y - height * 0.5f) / scale;
}

void ArtistPage1::setupNavigation()
{
    font->setSize(20);

    auto backText = u"back";
    float backTextX = -500 + 16 + 12;
    float backTextY = -500 + 12;

    auto nextText = u"next";
    float nextTextX = backTextX + font->getStringAdvance(backText) + 12 + 16 + 12;
    float nextTextY = -500 + 12;

    font->beginSequence(sequence1);
    Sketch::drawText(*font, backText, backTextX, backTextY, XFont::ALIGN_TOP);
    Sketch::drawText(*font, nextText, nextTextX, nextTextY, XFont::ALIGN_TOP);
    font->endSequence();

    // ---

    navigationRect[0] = math::Rectf(backTextX - 12, backTextY - 12, font->getStringAdvance(backText) + 12 * 2, 12 + 20 + 12);
    navigationRect[1] = math::Rectf(nextTextX - 12, nextTextY - 12, font->getStringAdvance(nextText) + 12 * 2, 12 + 20 + 12);

    draw::Rect overlayBack;
    overlayBack
        .setBounds(navigationRect[0])
        .append(overlayBatch1);

    draw::Rect overlayNext;
    overlayNext
        .setBounds(navigationRect[1])
        .append(overlayBatch1);
}

void ArtistPage1::setupSideInfo()
{
    font->setSize(30);

    auto workNameText = sketch->artists[artistIndex].workName;
    auto nameText = sketch->artists[artistIndex].name;

    float workNameTextX = 500 - 64 - 16 - font->getStringAdvance(workNameText);
    float nameTextX = workNameTextX - 64 - 16 * 2 - font->getStringAdvance(nameText);

    font->beginSequence(sequence2);
    Sketch::drawText(*font, workNameText, workNameTextX, -16, XFont::ALIGN_BOTTOM);
    Sketch::drawText(*font, nameText, nameTextX, -16, XFont::ALIGN_BOTTOM);
    font->endSequence();

    // ---

    overlayBatch2.clear();

    draw::Rect overlayWorkName;
    overlayWorkName
        .setBounds(workNameTextX - 16, -16 - 30 - 16, font->getStringAdvance(workNameText) + 16 * 2, 16 + 30 + 16)
        .append(overlayBatch2);

    draw::Rect overlayName;
    overlayName
        .setBounds(nameTextX - 16, -16 - 30 - 16, font->getStringAdvance(nameText) + 16 * 2, 16 + 30 + 16)
        .append(overlayBatch2);
}

