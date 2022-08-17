#pragma once

#include "Page.h"
#include "WordWrapper.h"

#include "chr/gl/Batch.h"
#include "chr/path/FollowablePath2D.h"
#include "chr/time/Clock.h"

#include "common/xf/FontManager.h"

class ArtistPage3 : public Page
{
public:
    ArtistPage3(Sketch *sketch);
    virtual ~ArtistPage3() {}

    void setup() final;
    void enter(int seed, int artistIndex) final;
    void resize(float width, float height) final;
    void draw() final;

    void mousePressed(float x, float y) final;

protected:
    chr::gl::IndexedVertexBatch<chr::gl::XYZ> overlayBatch1;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ> overlayBatch2;
    chr::gl::VertexBatch<chr::gl::XYZ> strokeBatch;

    std::shared_ptr<chr::xf::Font> font1;
    chr::xf::FontSequence sequence1;
    chr::xf::FontSequence sequence2;

    std::shared_ptr<chr::xf::Font> font3;
    chr::xf::FontSequence sequence3;

    chr::math::Rectf navigationRect[2];

    float scale;
    float width, height;
    int artistIndex;
    chr::Clock::Ref clock = chr::Clock::create();

    std::u16string text;
    WordWrapper wordWrapper;

    glm::vec2 transformCoordinates(float x, float y) const;

    void setupNavigation();
    void setupSideInfo();

    void wrap();
    chr::path::FollowablePath2D getPath(float y, float t);
    void drawPath(chr::path::FollowablePath2D &path, int lineIndex);
};
