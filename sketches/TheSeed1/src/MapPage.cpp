#include "MapPage.h"
#include "Sketch.h"

#include "chr/gl/draw/Sprite.h"
#include "chr/io/BinaryInputStream.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;
using namespace io;

static constexpr float TRACER_CHUNK_LENGTH = 48;
static constexpr float TRACER_TOL = 3.0;

static constexpr float FINGER_RADIUS_DP = 43;
static constexpr float PATH_OFFSET = 0;
static constexpr float FONT_SIZE = 24;
static constexpr float BUTTON_RADIUS = 24;

static constexpr float DT = 1.0f;
static constexpr float FRICTION = 0.0175f;
static constexpr float GRAVITY = 1.0f;

static constexpr float DOT_RADIUS_PIXELS = 56; // SPECIFIC TO "dot_112"

MapPage::MapPage(Sketch *sketch)
:
Page(sketch),
touchEnabled(true),
numTouches(0),
idProvider(2)
{}

void MapPage::setup()
{
    dotTexture = Texture(
        Texture::ImageRequest("dot2x.png")
            .setFlags(image::FLAGS_TRANSLUCENT)
            .setMipmap(true));

    // ---

    font = sketch->fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties2d());
    font->setShader(sketch->textureAlphaShader);
    font->setSize(FONT_SIZE);
    defaultSpaceWidth = font->getCharAdvance(u' ');

    // ---

    simulationParams.dt = DT;
    simulationParams.friction = FRICTION;
    simulationParams.gravity = GRAVITY;

    fingerRadius = FINGER_RADIUS_DP * getDisplayInfo().density / DisplayInfo::REFERENCE_DENSITY;

    tracer = Tracer(TRACER_CHUNK_LENGTH, TRACER_TOL);

    //

    add(InputSource::resource("trace1.dat"), sketch->artists[0].name);
    add(InputSource::resource("trace2.dat"), sketch->artists[1].name);
    add(InputSource::resource("trace3.dat"), sketch->artists[2].name);
    add(InputSource::resource("trace4.dat"), sketch->artists[3].name);
    add(InputSource::resource("trace5.dat"), sketch->artists[4].name);
    add(InputSource::resource("trace6.dat"), u"from the editor");
    add(InputSource::resource("trace7.dat"), u"home");

    // ---

    for (int i = 0; i < sets.size(); i++)
    {
        hairlines.emplace_back(textureManager);
    }

    for (auto &hairline : hairlines)
    {
        hairline.strip
            .setShader(sketch->textureAlphaShader)
            .setShaderColor(1, 0, 0, 0.67f);
    }

    dotBatch
        .setTexture(dotTexture)
        .setShader(sketch->textureAlphaShader)
        .setShaderColor(1, 0, 0, 0.67f);

    buttonBatch
        .setTexture(sketch->buttonTexture)
        .setShader(sketch->textureAlphaShader)
        .setShaderColor(1, 0, 0, 0.67f);

    arrowBatch
        .setTexture(sketch->arrowTexture)
        .setShader(sketch->textureAlphaShader)
        .setShaderColor(1, 1, 1, 1);
}

void MapPage::enter(int seed, int artistIndex)
{
    clock->restart();

    font->setSize(FONT_SIZE);
    font->setColor(0, 0, 0, 1);
}

void MapPage::resize(float width, float height)
{
    this->width = width;
    this->height = height;

    scale = height / sketch->size;

    for (int i = 0; i < sets.size(); i++)
    {
        hairlines[i].stroke(sets[i]->path, scale);
    }
}

void MapPage::update()
{
    for (auto &set : sets)
    {
        set->update(ax, ay, simulationParams);
    }
}

void MapPage::draw()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    auto projectionMatrix = glm::ortho(0.0f, width, height, 0.0f);

    Matrix modelViewMatrix;
    modelViewMatrix
        .translate(width * 0.5f, height * 0.5f)
        .scale(scale);

    State()
        .setShaderMatrix(modelViewMatrix * projectionMatrix)
        .apply();

    for (auto &hairline : hairlines)
    {
        hairline.strip.flush();
    }

    dotBatch.clear();
    buttonBatch.clear();
    arrowBatch.clear();

    font->beginSequence(sequence);

    for (auto &set : sets)
    {
        drawSet(*set);
    }

    font->endSequence();
    font->replaySequence(sequence);

    dotBatch.flush();
    buttonBatch.flush();
    arrowBatch.flush();
}

void MapPage::mousePressed(float x, float y)
{
    int index = 0;
    auto transformed = transformCoordinates(x, y);

    for (const auto &set : sets)
    {
        auto center = set->path.offsetToPosition(set->path.getLength());
        if (glm::length(transformed - center) < BUTTON_RADIUS)
        {
            buttonPressed(index);
            return;
        }

        index++;
    }

    mouseDragged(x, y);
    numTouches++;
}

void MapPage::mouseDragged(float x, float y)
{
    if (touchEnabled)
    {
        double timestamp = clock->getTime();

        for (auto &set : sets)
        {
            set->updateTouch(0, transformCoordinates(x, y), timestamp);
        }
    }
}

void MapPage::mouseReleased(float x, float y)
{
    if (touchEnabled)
    {
        for (auto &set : sets)
        {
            set->removeTouch(0, transformCoordinates(x, y));
        }
    }

    if (--numTouches == 0)
    {
        idProvider.reset(0);
    }
}

glm::vec2 MapPage::transformCoordinates(float x, float y) const
{
    return glm::vec2(x - width * 0.5f, y - height * 0.5f) / scale;
}

vector<glm::vec2> MapPage::loadPoints(const InputSource &inputSource)
{
    vector<glm::vec2> points;
    BinaryInputStream stream(inputSource);

    if (stream.good())
    {
        auto count = stream.read<uint32_t>();
        points.reserve(count);

        for (auto i = 0; i < count; i++)
        {
            points.emplace_back(stream.read<glm::vec2>());
        }
    }

    return points;
};

/*
 * SPLIT STRING TO SANITIZED LINES
 */
vector<u16string> MapPage::filterText(const u16string &input)
{
    vector<u16string> output;
    u16string buffer;

    for (auto c : input)
    {
        if (c == u'\n')
        {
            auto sanitized = sanitizeText(buffer);

            if (!sanitized.empty())
            {
                output.push_back(sanitized);
            }

            buffer.clear();
        }
        else
        {
            buffer.append(1, c);
        }
    }

    auto sanitized = sanitizeText(buffer);

    if (!sanitized.empty())
    {
        output.push_back(sanitized);
    }

    return output;
}

/*
 * - TRIMS SPACES AT START AND END
 * - LIMITS TO ONE CONSECUTIVE SPACES
 * - TURNS INVALID CHARACTERS TO QUESTION MARKS
 */
u16string MapPage::sanitizeText(const u16string &input)
{
    u16string output;
    int spaceCount = 0;
    bool beginnings = true;

    for (auto c : input)
    {
        if (font->isValid(c))
        {
            if (font->isSpace(c))
            {
                spaceCount++;
            }
            else
            {
                spaceCount = 0;
                beginnings = false;
            }

            if (!beginnings && (spaceCount < 2))
            {
                output.append(1, c);
            }
        }
        else
        {
            output.append(1, u'?');
            spaceCount = 0;
            beginnings = false;
        }
    }

    if (spaceCount > 0)
    {
        return output.substr(0, output.size() - 1);
    }
    else
    {
        return output;
    }
}

void MapPage::addStrings(StringSet &set, const vector<u16string> &lines, float offset)
{
    for (auto &line : lines)
    {
        set.addString(createString(line, offset));
        offset += set.spaceWidth + font->getStringAdvance(line);
    }
}

String* MapPage::createString(const u16string &text, float offset)
{
    int size = text.size();
    String *string = new String(size);

    float length = 0;
    offset -= PATH_OFFSET;

    for (int i = 0; i < size; i++)
    {
        auto c = text[i];
        auto w = font->getCharAdvance(c);

        string->setPosition(i, offset + length, w, true);
        length += w;
    }

    string->length = length;
    string->size = size;

    string->data = new CustomStringData(text);

    return string;
}

void MapPage::add(const InputSource &inputSource, const u16string &text)
{
    const auto &points = loadPoints(inputSource);

    FollowablePath2D path;
    path
        .begin()
        .add(points)
        .end();

    tracer.trace(path);

    auto set = make_shared<StringSet>(tracer.path, PATH_OFFSET, defaultSpaceWidth, fingerRadius, idProvider);
    sets.push_back(set);
    addStrings(*set, filterText(text), 0);
}

void MapPage::drawPath(const FollowablePath2D &path)
{
    Matrix matrix;
    matrix
        .translate(path.offsetToPosition(0))
        .scale(0.5f);

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(dotBatch, matrix);

    matrix
        .setTranslate(path.offsetToPosition(path.getLength()))
        .scale(BUTTON_RADIUS / DOT_RADIUS_PIXELS);

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(buttonBatch, matrix);

    path.offsetToValue(path.getLength()).applyToMatrix(matrix);

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(arrowBatch, matrix);
}

void MapPage::drawSet(const StringSet &set)
{
    drawPath(set.path);

    for (vector<String*>::const_iterator it = set.strings.begin(); it != set.strings.end(); ++it)
    {
        drawString(set.path, *it);
    }
}

void MapPage::drawString(const FollowablePath2D &path, String *string)
{
    CustomStringData *stringData = (CustomStringData*)string->data;
    auto &text = stringData->text;

    float offsetX = string->position[0];
    float offsetY = font->getOffsetY(XFont::ALIGN_MIDDLE); // XXX
    Matrix matrix;

    for (auto c : text)
    {
        auto glyphIndex = font->getGlyphIndex(c);
        auto half = 0.5f * font->getGlyphAdvance(glyphIndex);
        offsetX += half;

        if (glyphIndex >= 0)
        {
            path
                .offsetToValue(offsetX, FONT_SIZE) // XXX
                .applyToMatrix(matrix);
            font->addGlyph(matrix, glyphIndex, -half, offsetY);
        }

        offsetX += half;
    }
}

void MapPage::buttonPressed(int index)
{
    switch (index)
    {
        case 0:
            sketch->gotoPage(ARTIST1, 0);
            break;

        case 1:
            sketch->gotoPage(ARTIST1, 1);
            break;

        case 2:
            sketch->gotoPage(ARTIST1, 2);
            break;

        case 3:
            sketch->gotoPage(ARTIST1, 3);
            break;

        case 4:
            sketch->gotoPage(ARTIST1, 4);
            break;

        case 5:
            sketch->gotoPage(ABOUT, -1);
            break;

        case 6:
            sketch->gotoPage(COVER, -1);
            break;
    }
}
