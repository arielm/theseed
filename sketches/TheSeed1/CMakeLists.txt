cmake_minimum_required (VERSION 3.2.2)

project(TheSeed1)

set(INCLUDE_DIRS
  $ENV{THESEED_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "src/Page.cpp"
  "src/CoverPage.cpp"
  "src/MapPage.cpp"
  "src/AboutPage.cpp"
  "src/ArtistPage1.cpp"
  "src/ArtistPage2.cpp"
  "src/ArtistPage3.cpp"
  "src/BackPage.cpp"
  "src/Reader.cpp"
  "src/Branch.cpp"
  "src/Tree.cpp"
  "src/Stream.cpp"
  "src/UI.cpp"
  "src/PlaybackGroup.cpp"
  "src/PlaybackScreen.cpp"
  "src/TreeManager.cpp"
  "src/ClockItem.cpp"
  "src/Button.cpp"
  "src/ImageButton.cpp"
  "src/TextButton.cpp"
  "src/ImageItem.cpp"
  "src/WordWrapper.cpp"
  "$ENV{THESEED_PATH}/common/xf/Font.cpp"
  "$ENV{THESEED_PATH}/common/xf/FontManager.cpp"
  "$ENV{THESEED_PATH}/common/xf/FontSequence.cpp"
  "$ENV{THESEED_PATH}/common/quad/QuadBatch.cpp"
  "$ENV{THESEED_PATH}/common/Tracer.cpp"
  "$ENV{THESEED_PATH}/common/stroke/Hairline.cpp"
  "$ENV{THESEED_PATH}/common/tunnel/FingerTracker.cpp"
  "$ENV{THESEED_PATH}/common/tunnel/ItemList.cpp"
  "$ENV{THESEED_PATH}/common/tunnel/ObstacleList.cpp"
  "$ENV{THESEED_PATH}/common/tunnel/String.cpp"
  "$ENV{THESEED_PATH}/common/tunnel/StringSet.cpp"
)

#
# For Mojave and up
#
add_compile_definitions(
  GL_SILENCE_DEPRECATION
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
