#include "TreeManager.h"

using namespace std;
using namespace chr;

shared_ptr<Tree> TreeManager::createLoadedTree(shared_ptr<UI> ui, const InputSource &inputSource)
{
  streamLoaded = make_shared<Stream>(Stream::TYPE_READONLY);

  if (streamLoaded->load(inputSource))
  {
    treeLoaded = make_shared<Tree>(streamLoaded, ui);
    setTreeParams(treeLoaded);
  }
  else
  {
    treeLoaded.reset();
  }

  return treeLoaded;
}

void TreeManager::setTreeParams(shared_ptr<Tree> tree)
{
  tree->curvature_maxAngle = 9 * D2R; // (radians) per font-size (distance)
  tree->curvature_timeThreshold = 1000; // millis, after this period: curvature is null
  tree->curvature_interpFactor = 0.1f; // between 0 & 1, the lower it is the more time it takes for curvature to change
  tree->vanishDistance = 1000; // beyond this distance, no text is shown (strongly related to font-size)
  tree->slope = -5 * D2R;
}
