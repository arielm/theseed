#pragma once

class Sketch;

class Page
{
public:
    Page(Sketch *sketch);

    virtual void setup() {}
    virtual void enter(int seed, int artistIndex) {}
    virtual void resize(float width, float height) {}
    virtual void update() {}
    virtual void draw() {}

    virtual void mouseMoved(float x, float y) {}
    virtual void mouseDragged(float x, float y) {}
    virtual void mousePressed(float x, float y) {}
    virtual void mouseReleased(float x, float y) {}

protected:
    Sketch *sketch;
};
