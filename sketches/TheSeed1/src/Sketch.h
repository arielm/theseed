#pragma once

#include "Page.h"

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/Random.h"

#include "common/xf/FontManager.h"

enum PageId
{
    COVER,
    MAP,
    ABOUT,
    ARTIST1,
    ARTIST2,
    ARTIST3,
    BACK
};

struct Artist
{
    std::u16string name;
    std::u16string workName;
    std::u16string coderName;
    chr::InputSource imageSource;
    chr::InputSource recordSource;
    chr::InputSource codeSource;
    glm::vec2 textureOffset;
    glm::vec4 backgroundColor;
    glm::vec4 overlayColor;
    glm::vec4 primaryColor;
};

class Sketch : public chr::CrossSketch
{
public:
    chr::gl::shaders::ColorShader colorShader;
    chr::gl::shaders::TextureShader textureShader;
    chr::gl::shaders::TextureAlphaShader textureAlphaShader;

    chr::xf::FontManager fontManager;

    std::vector<chr::gl::Texture> textures;
    chr::gl::Texture buttonTexture;
    chr::gl::Texture arrowTexture;

    std::vector<Artist> artists;

    float size = 1000;
    Page *currentPage;

    Sketch();
    virtual ~Sketch() {}

    void setup() final;
    void resize() final;
    void update() final;
    void draw() final;

    void mouseMoved(float x, float y) final;
    void mouseDragged(int button, float x, float y) final;
    void mousePressed(int button, float x, float y) final;
    void mouseReleased(int button, float x, float y) final;

    void gotoPage(PageId id, int artistIndex);

    static void drawText(chr::XFont &font, const std::u16string &text, float x, float y, chr::xf::Font::Alignment alignment = chr::xf::Font::Alignment::ALIGN_BASELINE);

protected:
    int seed;
    std::map<PageId, Page*> pages;

    Page* getPage(PageId id);
};
