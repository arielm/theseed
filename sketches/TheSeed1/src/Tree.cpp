#include "Tree.h"
#include "UI.h"
#include "Branch.h"

#include "chr/gl/Camera.h"
#include "chr/gl/draw/Rect.h"
#include "chr/Log.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

Tree::Tree(shared_ptr<Stream> stream, std::shared_ptr<UI> ui)
:
stream(stream),
ui(ui)
{
  stream->setTree(this);
}

void Tree::start(int mode, int64_t creationTime)
{
  if (started)
  {
    return;
  }

  stream->start(mode, creationTime); // can fail (silently for now...)
  started = true;

  if (!rootBranch)
  {
    rootBranch = make_shared<Branch>(this, nullptr, 0, ui->mainFont, ui->mainFontSize);
    rootBranch->id = 0;
    branches.push_back(rootBranch);
  }

  currentBranch = rootBranch;
  rootBranch->isRoot = true;
  rootBranch->start(0); // branch's beginning is relative to the tree
}

void Tree::stop()
{
  if (started)
  {
    stream->stop();
    started = false;
    rootBranch->stop();
  }
}
void Tree::endOfMedia()
{
  stop();
  castEvent(EVENT_ENDOFMEDIA);
}

int Tree::branchAdded(shared_ptr<Branch> branch)
{
  if (std::find(branches.begin(), branches.end(), branch) != branches.end())
  {
    return branch->id;
  }

  branches.push_back(branch);
  return branches.size() - 1;
}

void Tree::branchStopped(shared_ptr<Branch> branch)
{}

void Tree::branchEdited(shared_ptr<Branch> branch)
{}

bool Tree::switchBranch(shared_ptr<Branch> to)
{
  if (!to->started || (to == currentBranch))
  {
    return false;
  }

  if (!currentBranch->isEmpty())
  {
    to->setPrevious(currentBranch);
  }
  else
  {
    currentBranch->stop(); // cancelling branches that are empty when they're switched-from...
  }

  currentBranch = to;

  LOGD << "JUMP TO BRANCH: " << to->id << endl;
  return true;
}

void Tree::run()
{
  stream->run();
}

void Tree::draw()
{
  draw(LAYER_TREE);
}

void Tree::draw(int layerId)
{
  if (currentBranch)
  {
    switch (layerId)
    {
      case LAYER_TREE:
        drawTree();
        break;
    }
  }
}

void Tree::drawTree()
{
  ui->mainFont->beginSequence(ui->mainFontSequence, true);

  Matrix matrix;
  currentBranch->drawFrom(matrix);

  ui->mainFont->endSequence();

  //

  Camera camera;

  camera
    .setFov(cam_fov)
    .setClip(0.1f, 1000.0f)
    .setWindowSize(ui->windowSize);

  camera.getViewMatrix()
    .setIdentity()
    .scale(1, -1, 1)
    .translate(cam_x, cam_y, -cam_distance)
    .rotateZ(cam_twist)
    .rotateX(-cam_elevation)
    .rotateZ(-cam_azimuth);

  State()
    .setShader(ui->textureAlphaShader)
    .setShaderMatrix<MVP>(camera.getViewProjectionMatrix())
    .glLineWidth(2)
    .apply();

  ui->mainFont->replaySequence(ui->mainFontSequence);

  //

  ui->lineBatch.clear();

  if (started)
  {
    if (stream->getCurrentMediaTime() % (2 * cursor_period) >= cursor_period)
    {
      ui->lineBatch
        .addVertex(0, -ui->mainFontSize * 0.5f, 0, glm::vec4(1, 1, 1, 1))
        .addVertex(0, +ui->mainFontSize * 0.5f, 0, glm::vec4(1, 1, 1, 1));
    }
  }

  ui->lineBatch.flush();
}
