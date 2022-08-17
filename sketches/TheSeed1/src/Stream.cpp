#include "Stream.h"
#include "Tree.h"
#include "Branch.h"
#include "Reader.h"

#include "chr/Log.h"
#include "chr/cross/Keyboard.h"

using namespace std;
using namespace chr;

static const u16string FORMAT_STRING = u"CHRONOTEXT_TTC";

Stream::Stream(int type)
:
type(type)
{}

void Stream::setTree(Tree *tree)
{
  this->tree = tree;
}

void Stream::start(int mode, int64_t creationTime)
{
  if (started)
  {
    return;
  }

  if ((type == TYPE_READONLY) && !loaded)
  {
    LOGE << "STREAM IS EMPTY!" << endl;
    return;
  }

  this->mode = mode;
  this->creationTime = creationTime;

  clock->setTime(0);
  clock->start();
  started = true;

  if (mode == MODE_PLAYBACK)
  {
    ext_pos = 0;
    previousPlaybackTime = 0;
  }
}

void Stream::stop()
{
  if (started)
  {
    started = false;
    clock->stop();
  }
}

int Stream::getDuration()
{
  return duration;
}

int64_t Stream::getCreationDate()
{
  return creationTime;
}

void Stream::run()
{
  if (!started)
  {
    return;
  }

  clock->update();
  currentMediaTime = int(clock->getTime() * 1000);

  setPlaybackMediaTime(currentMediaTime);
}

int Stream::getCurrentMediaTime()
{
  return currentMediaTime;
}

void Stream::setPlaybackMediaTime(int targetTime)
{
  if (!started)
  {
    return;
  }

  if (targetTime >= getDuration())
  {
    tree->endOfMedia();
    return;
  }

  if (targetTime < previousPlaybackTime)
  {
    LOGE << "CAN'T PLAY BACKWARDS!" << endl;
    LOGE << "ext_pos: " << ext_pos << endl;
    LOGE << "targetTime: " << targetTime << endl;
    LOGE << "previousPlaybackTime: " << previousPlaybackTime << endl;

    tree->endOfMedia();
    return;
  }

  int t;
  while (ext_pos < ext_size && (t = ext_track_time[ext_pos]) <= targetTime)
  {
    if (ext_track_event[ext_pos] == EVENT_TEXT)
    {
      textEvent(ext_track_text[ext_pos], t);
    }
    else
    {
      navEvent(ext_track_nav[ext_pos], t);
    }
    ext_pos++;
  }

  previousPlaybackTime = targetTime;
}

bool Stream::textEvent(char16_t c, int t)
{
  auto current = tree->currentBranch;

  if (c == 8)
  {
    return current->remove();
  }
  else if ((c == 10) || (c == 13))
  {
    auto branch = current->addBranch();

    if (branch)
    {
      branch->start(t);

      LOGD << "NEW BRANCH: " << branch->id << endl;
      return tree->switchBranch(branch);
    }
    else
    {
      LOGD << "CAN'T CREATE BRANCH!" << endl;
      return false;
    }
  }
  else
  {
    return current->add(c, t - current->beginning);
  }
}

bool Stream::navEvent(int id, int t)
{
  return tree->switchBranch(tree->branches[id]);
}

/*
 * FORMAT (JAVA TYPES):
 *
 * HEADER:
 * chars:	format-string
 * long:	record creation date (in millis)
 * int:	duration (in millis)
 * int:	number of events (track length)

 * TRACK:
 * for each event:
 *   int:	time
 *   byte:	event-type
 *   char:	text-data
 *   short:	nav-data
 */

bool Stream::load(const InputSource &inputSource)
{
  if ((type != TYPE_READONLY) || loaded)
  {
    LOGE << "STREAM EITHER RECORDABLE OR ALREADY LOADED!" << endl;
    return false;
  }

  Reader reader(inputSource);

  if (reader.readJavaString(14) != FORMAT_STRING)
  {
    LOGE << "BAD STREAM FORMAT!" << endl;
    return false;
  }

  creationTime = reader.readJavaLong();
  duration = reader.readJavaInt();
  ext_size = reader.readJavaInt();

  for (int i = 0; i < ext_size; i++)
  {
    ext_track_time.push_back(reader.readJavaInt());
    ext_track_event.push_back(reader.readJavaByte());
    ext_track_text.push_back(reader.readJavaChar());
    ext_track_nav.push_back(reader.readJavaShort());
  }

  loaded = true;

  return true;
}
