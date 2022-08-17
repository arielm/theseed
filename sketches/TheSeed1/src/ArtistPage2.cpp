#include "ArtistPage2.h"
#include "Sketch.h"

#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;

ArtistPage2::ArtistPage2(Sketch *sketch)
:
Page(sketch)
{}

void ArtistPage2::setup()
{
    font = sketch->fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties2d());
    font->setShader(sketch->textureAlphaShader);

    overlayBatch1
        .setShader(sketch->colorShader)
        .setShaderColor(1, 1, 1, 1);

    setupNavigation();

    // ---

    ui = make_shared<UI>(sketch);
    ui->setup();
}

void ArtistPage2::enter(int seed, int artistIndex)
{
    this->artistIndex = artistIndex;

    //

    overlayBatch2
        .setShader(sketch->colorShader)
        .setShaderColor(sketch->artists[artistIndex].overlayColor);

    setupSideInfo();

    //

    ui->enter(artistIndex);
}

void ArtistPage2::resize(float width, float height)
{
    this->width = width;
    this->height = height;

    scale = height / sketch->size;

    ui->resize(width, height);
}

void ArtistPage2::update()
{
    ui->run();
}

void ArtistPage2::draw()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    ui->draw();

    // ---

    auto projectionMatrix = glm::ortho(0.0f, width, 0.0f, height);

    Matrix modelViewMatrix1;
    modelViewMatrix1
        .translate(width * 0.5f, height * 0.5f)
        .scale(+scale, -scale);

    gl::State()
        .setShaderMatrix(modelViewMatrix1 * projectionMatrix)
        .apply();

    overlayBatch1.flush();

    font->setColor(0, 0, 0, 1);
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

void ArtistPage2::mouseMoved(float x, float y)
{
    ui->mouseMoved(x, y);
}

void ArtistPage2::mousePressed(float x, float y)
{
    auto transformed = transformCoordinates(x, y);

    if (navigationRect[0].contains(transformed))
    {
        sketch->gotoPage(ARTIST1, artistIndex);
        return;
    }
    else if (navigationRect[1].contains(transformed))
    {
        sketch->gotoPage(ARTIST3, artistIndex);
        return;
    }

    ui->mousePressed(x, y);
}

glm::vec2 ArtistPage2::transformCoordinates(float x, float y) const
{
    return glm::vec2(x - width * 0.5f, y - height * 0.5f) / scale;
}

void ArtistPage2::setupNavigation()
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

void ArtistPage2::setupSideInfo()
{
    font->setSize(30);

    auto nameText = sketch->artists[artistIndex].name;
    float nameTextX = 500 - 64 - 16 - font->getStringAdvance(nameText);

    font->beginSequence(sequence2);
    Sketch::drawText(*font, nameText, nameTextX, -16, XFont::ALIGN_BOTTOM);
    font->endSequence();

    // ---

    overlayBatch2.clear();

    draw::Rect overlayName;
    overlayName
        .setBounds(nameTextX - 16, -16 - 30 - 16, font->getStringAdvance(nameText) + 16 * 2, 16 + 30 + 16)
        .append(overlayBatch2);
}

