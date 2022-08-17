#include "Sketch.h"
#include "CoverPage.h"
#include "MapPage.h"
#include "AboutPage.h"
#include "ArtistPage1.h"
#include "ArtistPage2.h"
#include "ArtistPage3.h"
#include "BackPage.h"

using namespace std;
using namespace chr;
using namespace gl;

Sketch::Sketch()
{}

void Sketch::setup()
{
    artists.emplace_back();
    artists[0].name = u"Lammetje";
    artists[0].workName = u"NBR";
    artists[0].imageSource = InputSource::resource("lammetje.jpg");
    artists[0].recordSource = InputSource::resource("record_lammetje.chr");
    artists[0].codeSource = InputSource::resource("code_lammetje.txt");
    artists[0].textureOffset = glm::vec2(-7, -136);
    artists[0].backgroundColor = glm::vec4(0.25f, 0.25f, 0.25f, 1);
    artists[0].overlayColor = glm::vec4(216 / 255.0f, 113 / 255.0f, 44 / 255.0f, 1);
    artists[0].primaryColor = glm::vec4(1, 1, 1, 1);

    artists.emplace_back();
    artists[1].name = u"Liam Egan";
    artists[1].workName = u"Wonder";
    artists[1].imageSource = InputSource::resource("egan.jpg");
    artists[1].recordSource = InputSource::resource("record_egan.chr");
    artists[1].codeSource = InputSource::resource("code_egan.txt");
    artists[1].textureOffset = glm::vec2(-101, -129);
    artists[1].backgroundColor = glm::vec4(0.25f, 0.25f, 0.25f, 1);
    artists[1].overlayColor = glm::vec4(108 / 255.0f, 186 / 255.0f, 189 / 255.0f, 1);
    artists[1].primaryColor = glm::vec4(1, 1, 1, 1);

    artists.emplace_back();
    artists[2].name = u"elsif";
    artists[2].workName = u"Memory Module";
    artists[2].imageSource = InputSource::resource("elsif.jpg");
    artists[2].recordSource = InputSource::resource("record_elsif.chr");
    artists[2].codeSource = InputSource::resource("code_elsif.txt");
    artists[2].textureOffset = glm::vec2(0, 0);
    artists[2].backgroundColor = glm::vec4(1, 1, 1, 1);
    artists[2].overlayColor = glm::vec4(252 / 255.0f, 234 / 255.0f, 37 / 255.0f, 1);
    artists[2].primaryColor = glm::vec4(0, 0, 0, 0.75f);

    artists.emplace_back();
    artists[3].name = u"clint";
    artists[3].workName = u"f(x)ields";
    artists[3].coderName = u"Orr Kislev";
    artists[3].imageSource = InputSource::resource("clint.jpg");
    artists[3].recordSource = InputSource::resource("record_clint.chr");
    artists[3].codeSource = InputSource::resource("code_clint.txt");
    artists[3].textureOffset = glm::vec2(0, -2);
    artists[3].backgroundColor = glm::vec4(1, 1, 1, 1);
    artists[3].overlayColor = glm::vec4(1, 0, 0, 1);
    artists[3].primaryColor = glm::vec4(1, 1, 1, 1);

    artists.emplace_back();
    artists[4].name = u"Ciput_GENART";
    artists[4].workName = u"Freehand";
    artists[4].coderName = u"0xphiiil";
    artists[4].imageSource = InputSource::resource("ciput.jpg");
    artists[4].recordSource = InputSource::resource("record_ciput.chr");
    artists[4].codeSource = InputSource::resource("code_ciput.txt");
    artists[4].textureOffset = glm::vec2(-5, -222);
    artists[4].backgroundColor = glm::vec4(0.25f, 0.25f, 0.25f, 1);
    artists[4].overlayColor = glm::vec4(169 / 255.0f, 126 / 255.0f, 255 / 255.0f, 1);
    artists[4].primaryColor = glm::vec4(1, 1, 1, 1);

    for (const auto &artist : artists)
    {
        textures.emplace_back(Texture::ImageRequest(artist.imageSource).setMipmap(true));
    }

    // ---

    buttonTexture = Texture(Texture::ImageRequest("dot_112.png")
                                .setFlags(image::FLAGS_TRANSLUCENT)
                                .setMipmap(true));

    arrowTexture = Texture(Texture::ImageRequest("arrow.png")
                               .setFlags(image::FLAGS_TRANSLUCENT)
                               .setMipmap(true));

    // ---

    seed = 123456;

    #if defined(CHR_PLATFORM_EMSCRIPTEN)
        double random = EM_ASM_DOUBLE(
            return fxrand();
        );

        seed = random * 10000;
        LOGI << seed << endl;
    #endif

    Random rnd(seed);
    int artistIndex = rnd.nextInt(0, artists.size());

    gotoPage(COVER, artistIndex);

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
    currentPage->resize(windowInfo.width, windowInfo.height);
}

void Sketch::update()
{
    currentPage->update();
}

void Sketch::draw()
{
    currentPage->draw();
}

void Sketch::mouseMoved(float x, float y)
{
    currentPage->mouseMoved(x, y);
}

void Sketch::mouseDragged(int button, float x, float y)
{
    currentPage->mouseDragged(x, y);
}

void Sketch::mousePressed(int button, float x, float y)
{
    currentPage->mousePressed(x, y);
}

void Sketch::mouseReleased(int button, float x, float y)
{
    currentPage->mouseReleased(x, y);
}

void Sketch::gotoPage(PageId id, int artistIndex)
{
    currentPage = getPage(id);
    currentPage->enter(seed, artistIndex);
}

Page* Sketch::getPage(PageId id)
{
    auto it = pages.find(id);
    if (it != pages.end())
    {
        return it->second;
    }

    Page *page = nullptr;

    switch (id)
    {
        case COVER:
            page = new CoverPage(this);
            break;

        case MAP:
            page = new MapPage(this);
            break;

        case ABOUT:
            page = new AboutPage(this);
            break;

        case ARTIST1:
            page = new ArtistPage1(this);
            break;

        case ARTIST2:
            page = new ArtistPage2(this);
            break;

        case ARTIST3:
            page = new ArtistPage3(this);
            break;

        case BACK:
            page = new BackPage(this);
            break;
    }

    pages[id] = page;
    page->setup();
    page->resize(windowInfo.width, windowInfo.height);

    return page;
}

void Sketch::drawText(chr::XFont &font, const u16string &text, float x, float y, XFont::Alignment alignment)
{
    float offset = font.getOffsetY(alignment);

    for (auto c : text)
    {
        int glyphIndex = font.getGlyphIndex(c);
        float w = font.getGlyphAdvance(glyphIndex);

        if (glyphIndex >= 0)
        {
            font.addGlyph(glyphIndex, x, y + offset);
        }

        x += w;
    }
}
