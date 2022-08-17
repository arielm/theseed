#pragma once

#include "Page.h"

#include "chr/gl/Batch.h"
#include "chr/time/Clock.h"

#include "common/xf/FontManager.h"

struct Unit1
{
    std::u16string text;
    glm::vec2 position;
    float angle;
    float scale;
    float clockFactor1;
    float clockFactor2;
    float clockFactor3;

    Unit1()
    {}

    Unit1(const std::u16string &text, const glm::vec2 &position, float angle, float scale, float clockFactor1, float clockFactor2, float clockFactor3)
    :
    text(text),
    position(position),
    angle(angle),
    scale(scale),
    clockFactor1(clockFactor1),
    clockFactor2(clockFactor2),
    clockFactor3(clockFactor3)
    {}
};

struct Unit2
{
    std::u16string text;
    glm::vec2 position;
    float angle;
    float scale;

    Unit2()
    {}

    Unit2(const std::u16string &text, const glm::vec2 &position, float angle, float scale)
    :
    text(text),
    position(position),
    angle(angle),
    scale(scale)
    {}
};

class AboutPage : public Page
{
public:
    AboutPage(Sketch *sketch);
    virtual ~AboutPage() {}

    void setup() final;
    void enter(int seed, int artistIndex) final;
    void resize(float width, float height) final;
    void update() final;
    void draw() final;

    void mousePressed(float x, float y) final;

protected:
    std::shared_ptr<chr::xf::Font> font1;
    chr::xf::FontSequence sequence1;

    std::shared_ptr<chr::xf::Font> font2;
    chr::xf::FontSequence sequence2;
    chr::xf::FontSequence sequence3;

    chr::gl::IndexedVertexBatch<chr::gl::XYZ> overlayBatch1, overlayBatch2;

    float scale;
    float width, height;
    bool entered = false;
    chr::Clock::Ref clock = chr::Clock::create();

    std::vector<float> paragraphsHeights;
    std::vector<float> paragraphsY;
    std::vector<bool> morphing;
    std::vector<chr::Clock::Ref> morphingClocks;
    std::vector<std::vector<Unit1>> units1;
    std::vector<std::vector<Unit2>> units2;
    std::vector<std::vector<Unit2>> unitsSource, unitsTarget;

    chr::math::Rectf navigationRect;

    void generate(int seed);

    glm::vec2 transformCoordinates(float x, float y) const;
    bool isPointInsideEllipse(float x, float y, float h, float k, float a, float b);

    std::vector<Unit2> wrap(const std::string &text, float x1, float x2, float y1, float leading = 1);
    float layout(const std::string &text, float x1, float x2, float leading = 1);
    void drawWord(const std::u16string &text, const chr::gl::Matrix &matrix, float x, float y);

    void setupNavigation();
    void setupSideInfo();
};
