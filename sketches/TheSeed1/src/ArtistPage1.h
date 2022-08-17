#pragma once

#include "Page.h"

#include "chr/gl/Batch.h"

#include "common/xf/FontManager.h"

class ArtistPage1 : public Page
{
public:
    ArtistPage1(Sketch *sketch);
    virtual ~ArtistPage1() {}

    void setup() final;
    void enter(int seed, int artistIndex) final;
    void resize(float width, float height) final;
    void draw() final;

    void mousePressed(float x, float y) final;

protected:
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> fillBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ> overlayBatch1;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ> overlayBatch2;

    std::shared_ptr<chr::xf::Font> font;
    chr::xf::FontSequence sequence1;
    chr::xf::FontSequence sequence2;

    chr::math::Rectf navigationRect[2];

    float scale;
    float width, height;
    int artistIndex;

    glm::vec2 transformCoordinates(float x, float y) const;

    void setupNavigation();
    void setupSideInfo();
};
