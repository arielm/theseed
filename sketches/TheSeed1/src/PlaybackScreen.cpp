#include "PlaybackScreen.h"
#include "UI.h"

using namespace std;
using namespace chr;

PlaybackScreen::PlaybackScreen(shared_ptr<UI> ui)
:
Screen(ui)
{}

void PlaybackScreen::setCurrentTree(Tree *tree)
{
  currentTree = tree;
}

void PlaybackScreen::resize()
{
  ui->playbackGroup->resize();
}

void PlaybackScreen::run()
{
  currentTree->run();
  ui->playbackGroup->run(currentTree);
}

void PlaybackScreen::draw()
{
  currentTree->draw();

  ui->beginGroupDraw();
  ui->playbackGroup->draw();
  ui->endGroupDraw();
}

void PlaybackScreen::enter()
{
  ui->playbackGroup->enter(this, currentTree);

  currentTree->cam_fov = 60;
  currentTree->cam_twist = 0 * D2R;
  currentTree->cam_elevation = 0 * D2R;
  currentTree->cam_azimuth = 0 * D2R;
  currentTree->cam_x = 0;
  currentTree->cam_y = 0;
  currentTree->cam_distance = 500;

  ui->playbackGroup->buttonPlay.setLocked(true);
  ui->playbackGroup->buttonRewind.setEnabled(true);
  ui->playbackGroup->buttonPause.setEnabled(true);
  currentTree->start(Stream::MODE_PLAYBACK, 0);
}

void PlaybackScreen::event(EventCaster *source, int message)
{
  ui->playbackGroup->event(currentTree, source, message);
}
