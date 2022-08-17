#pragma once

#include "Page.h"

#include "chr/gl/Batch.h"
#include "chr/path/FollowablePath2D.h"
#include "chr/time/Clock.h"

#include "common/Tracer.h"
#include "common/tunnel/StringSet.h"
#include "common/stroke/Hairline.h"
#include "common/xf/FontManager.h"

struct CustomStringData : public StringData
{
    std::u16string text;

    explicit CustomStringData(std::u16string text)
    :
    text(std::move(text))
    {}
};

class MapPage : public Page
{
public:
    MapPage(Sketch *sketch);
    virtual ~MapPage() {}

    void setup() final;
    void enter(int seed, int artistIndex) final;
    void resize(float width, float height) final;
    void update() final;
    void draw() final;

    void mouseDragged(float x, float y) final;
    void mousePressed(float x, float y) final;
    void mouseReleased(float x, float y) final;

protected:
    std::shared_ptr<chr::xf::Font> font;
    chr::xf::FontSequence sequence;
    float defaultSpaceWidth;

    chr::gl::TextureManager textureManager;

    chr::gl::Texture dotTexture;

    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> dotBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> buttonBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> arrowBatch;

    Tracer tracer;
    std::vector<chr::Hairline> hairlines;

    std::vector<StringSetRef> sets;

    float scale;
    float width, height;
    chr::Clock::Ref clock = chr::Clock::create();

    float fingerRadius;
    float ax = 0, ay = 1;

    bool touchEnabled;
    int numTouches;
    IdProvider idProvider;

    SimulationParams simulationParams;

    std::vector<glm::vec2> loadPoints(const chr::InputSource &inputSource);
    std::vector<std::u16string> filterText(const std::u16string &input);
    std::u16string sanitizeText(const std::u16string &input);

    glm::vec2 transformCoordinates(float x, float y) const;

    void addStrings(StringSet &set, const std::vector<std::u16string> &lines, float offset);
    String* createString(const std::u16string &text, float offset);
    void add(const chr::InputSource &inputSource, const std::u16string &text);

    void drawPath(const chr::path::FollowablePath2D &path);
    void drawSet(const StringSet &set);
    void drawString(const chr::path::FollowablePath2D &path, String *string);

    void buttonPressed(int index);
};
