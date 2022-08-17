#pragma once

#include "Page.h"

#include "chr/gl/Batch.h"
#include "chr/time/Clock.h"

#include "common/xf/FontManager.h"

class CoverPage : public Page
{
public:
    CoverPage(Sketch *sketch);
    virtual ~CoverPage() {}

    void enter(int seed, int artistIndex) final;
    void resize(float width, float height) final;
    void draw() final;

    void mousePressed(float x, float y) final;

protected:
    chr::gl::VertexBatch<chr::gl::XYZ> strokeBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV.RGBA> fillBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ> overlayBatch1, overlayBatch2;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> ringBatch1, ringBatch2;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> arrowBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV.RGBA> dotBatch;
    chr::gl::VertexBatch<chr::gl::XYZ> lineBatch;

    std::shared_ptr<chr::xf::Font> font;
    chr::xf::FontSequence sequence;

    float scale;
    float width, height;
    int artistIndex;
    bool entered = false;
    chr::Clock::Ref clock = chr::Clock::create();

    int logoWidth, logoHeight;

    void generate(int seed);

    void loadLogo(const chr::InputSource &inputSource);
    void drawDot(const glm::vec2 &position, const glm::vec4 &color, float radius);

    void setupSideInfo();
    void drawText(const std::u16string &text, float x, float y, chr::xf::Font::Alignment alignment = chr::xf::Font::Alignment::ALIGN_BASELINE);
};
