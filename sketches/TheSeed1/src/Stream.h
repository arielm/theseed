#pragma once

#include "chr/time/FrameClock.h"
#include "chr/FileSystem.h"

class Tree;

class Stream
{
public:
  enum
  {
    MODE_PLAYBACK
  };

  enum
  {
    TYPE_READONLY
  };

  enum
  {
    EVENT_TEXT
  };

  int mode;
  int type;
  chr::FrameClock::Ref clock = chr::FrameClock::create();

  bool recorded = false;
  bool loaded = false;

  Stream(int type);

  void setTree(Tree *tree);
  void start(int mode, int64_t creationTime);
  void stop();

  int getDuration();
  int64_t getCreationDate();
  void run();
  int getCurrentMediaTime();
  void setPlaybackMediaTime(int targetTime);

  bool load(const chr::InputSource &inputSource);

protected:
  // an interleaved stream used as a data-source both for LIVE (writing to it) & PLAYBACK (reading from it) modes
  std::vector<int> ext_track_time;
  std::vector<int> ext_track_event;
  std::vector<char16_t> ext_track_text;
  std::vector<int> ext_track_nav;
  int ext_size = 0;

  // used for playback
  int ext_pos = 0;
  int currentMediaTime = 0;
  int previousPlaybackTime = 0;

  bool started = false;
  int duration = 0;
  int64_t creationTime = 0;

  Tree *tree = nullptr;

  bool textEvent(char16_t c, int t); //	returns false if the event was not processed
  bool navEvent(int id, int t); // returns false if the event was not processed
};
