#pragma once

#include "Page.h"
#include "UI.h"

#include "chr/gl/Batch.h"

#include "common/xf/FontManager.h"

class ArtistPage2 : public Page
{
public:
    ArtistPage2(Sketch *sketch);
    virtual ~ArtistPage2() {}

    void setup() final;
    void enter(int seed, int artistIndex) final;
    void resize(float width, float height) final;
    void update() final;
    void draw() final;

    void mouseMoved(float x, float y) final;
    void mousePressed(float x, float y) final;

protected:
    std::shared_ptr<UI> ui;

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
