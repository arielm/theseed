#include "PlaybackGroup.h"
#include "UI.h"

using namespace std;
using namespace chr;
using namespace math;

PlaybackGroup::PlaybackGroup(std::shared_ptr<UI> ui)
:
Group(ui)
{
  buttonRewind = ImageButton(ui, "rewind");
  buttonPlay = ImageButton(ui, "play");
  buttonPause = ImageButton(ui, "pause");

  itemClock = ClockItem(ui);

  itemCross = ImageItem(ui, "cross");
  buttonSpeed1 = TextButton(ui, u"1");
  buttonSpeed2 = TextButton(ui, u"2");
  buttonSpeed4 = TextButton(ui, u"4");
}

void PlaybackGroup::resize()
{
  ui->subFont->setSize(ui->subFontSize);

  float gap = ui->gap;
  float height = 18 * 2.5f;
  float y = ui->size * 0.5f - gap - height;

  float w1 = itemClock.getWidth();
  float x7 = -w1 * 0.5f;

  float w2 = 4 * 2.5f;
  float x9 = x7 + w1 + gap * 2;
  float y1 = y + (height - w2) * 0.5f;

  float x10 = x9 + w2 + gap;
  float x11 = x10 + height + gap;
  float x12 = x11 + height + gap;

  float x13 = x7 - gap - height;
  float x14 = x13 - gap - height;
  float x15 = x14 - gap - height;

  buttonRewind.setBounds(Rectf(x15, y, height, height));
  buttonPlay.setBounds(Rectf(x14, y, height, height));
  buttonPause.setBounds(Rectf(x13, y, height, height));

  itemClock.setBounds(Rectf(x7, y, w1, height));

  itemCross.setBounds(Rectf(x9, y1, w2, w2));
  buttonSpeed1.setBounds(Rectf(x10, y, height, height));
  buttonSpeed2.setBounds(Rectf(x11, y, height, height));
  buttonSpeed4.setBounds(Rectf(x12, y, height, height));
}

void PlaybackGroup::draw()
{
  buttonRewind.draw();
  buttonPlay.draw();
  buttonPause.draw();

  itemClock.draw();

  itemCross.draw();
  buttonSpeed1.draw();
  buttonSpeed2.draw();
  buttonSpeed4.draw();
}

void PlaybackGroup::enter(Screen *screen, Tree *tree)
{
  tree->setListener(screen);

  buttonRewind.setListener(screen);
  buttonRewind.reset();

  buttonPlay.setListener(screen);
  buttonPlay.reset();

  buttonPause.setListener(screen);
  buttonPause.reset();

  buttonSpeed1.setListener(screen);
  buttonSpeed1.reset();

  buttonSpeed2.setListener(screen);
  buttonSpeed2.reset();

  buttonSpeed4.setListener(screen);
  buttonSpeed4.reset();
  buttonSpeed4.setLocked(true);
  setClockRate(*tree->stream->clock, 4); // default speed
}

void PlaybackGroup::run(Tree *tree)
{
  itemClock.setMediaTime(tree->stream->getCurrentMediaTime());

  buttonRewind.run();
  buttonPlay.run();
  buttonPause.run();
  buttonSpeed1.run();
  buttonSpeed2.run();
  buttonSpeed4.run();
}

void PlaybackGroup::event(Tree *tree, EventCaster *source, int message)
{
  if (source == tree && (message == Tree::EVENT_ENDOFMEDIA))
  {
    buttonPause.setEnabled(false);
    buttonPlay.setLocked(false);
    buttonPlay.setEnabled(false);
  }

  // ---

  if (source == &buttonRewind)
  {
    tree->stop();
    tree->start(Stream::MODE_PLAYBACK, 0);

    tree->stream->clock->stop();
    buttonPause.setEnabled(false);
    buttonPause.setLocked(false);
    buttonPlay.setEnabled(true);
    buttonPlay.setLocked(false);
    buttonRewind.setEnabled(false);
  }
  else if (source == &buttonPlay)
  {
    if (!tree->started)
    {
      tree->start(Stream::MODE_PLAYBACK, 0);
    }
    else
    {
      tree->stream->clock->start();
    }

    buttonPlay.setLocked(true);
    buttonPause.setEnabled(true);
    buttonPause.setLocked(false);
    buttonRewind.setEnabled(true);
  }
  else if (source == &buttonPause)
  {
    if (tree->stream->clock->getState() == Clock::STARTED)
    {
      tree->stream->clock->stop();
      buttonPause.setLocked(true);
      buttonPlay.setLocked(false);
    }
  }

  // ---

  if (source == &buttonSpeed1)
  {
    setClockRate(*tree->stream->clock, 1);
    buttonSpeed1.setLocked(true);
    buttonSpeed2.setLocked(false);
    buttonSpeed4.setLocked(false);
  }
  else if (source == &buttonSpeed2)
  {
    setClockRate(*tree->stream->clock, 2);
    buttonSpeed1.setLocked(false);
    buttonSpeed2.setLocked(true);
    buttonSpeed4.setLocked(false);
  }
  else if (source == &buttonSpeed4)
  {
    setClockRate(*tree->stream->clock, 4);
    buttonSpeed1.setLocked(false);
    buttonSpeed2.setLocked(false);
    buttonSpeed4.setLocked(true);
  }
}

void PlaybackGroup::setClockRate(Clock &clock, float rate)
{
  if (clock.getState() == Clock::STOPPED)
  {
    clock.setRate(rate);
  }
  else
  {
    clock.stop();
    clock.setRate(rate);
    clock.start();
  }
}
