#pragma once

#include "Page.h"

#include "chr/gl/Batch.h"
#include "chr/path/FollowablePath2D.h"
#include "chr/time/Clock.h"

#include "common/xf/FontManager.h"

class BackPage : public Page
{
public:
    BackPage(Sketch *sketch);
    virtual ~BackPage() {}

    void setup() final;
    void enter(int seed, int artistIndex) final;
    void resize(float width, float height) final;
    void draw() final;

    void mousePressed(float x, float y) final;

protected:
    chr::gl::VertexBatch<chr::gl::XYZ> strokeBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> spriteBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ> overlayBatch;

    std::shared_ptr<chr::xf::Font> font;
    chr::xf::FontSequence sequence1;
    chr::xf::FontSequence sequence2;

    chr::gl::Texture twitterTexture;
    chr::path::FollowablePath2D path;

    float scale;
    float width, height;
    int artistIndex;
    chr::Clock::Ref clock = chr::Clock::create();

    chr::math::Rectf navigationRect;

    glm::vec2 transformCoordinates(float x, float y) const;

    void drawSpiral(float x, float y);
    void drawTextOnPath(const std::u16string &text, float offset);
    void drawLabelOnPath(const std::u16string &text, float offset);
    void drawSpriteOnPath(float offset);

    void setupNavigation();
};
