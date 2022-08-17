#pragma once

#include "Tree.h"
#include "UI.h"

class TreeManager
{
public:
  std::shared_ptr<Stream> streamLoaded;
  std::shared_ptr<Tree> treeLoaded;

  std::shared_ptr<Tree> createLoadedTree(std::shared_ptr<UI> ui, const chr::InputSource &inputSource);

protected:
  void setTreeParams(std::shared_ptr<Tree> tree);
};
