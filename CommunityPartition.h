#ifndef _COMMUNITYPARTITION_
#define _COMMUNITYPARTITION_
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "gtries/Error.h"

class CommunityPartition
{
public:
  CommunityPartition(int);
  CommunityPartition();
  void init(int nNodes, int nCommunities);
  void setNumberNodes(int number);
  void setNumberCommunities(int number);
  void setNodeCommunity(int nodeId, int communityId);
  int getNodeCommunity(int nodeId);
  int getNumberCommunities();
  int getNumberNodes();
  int *getPartition();

  int kronecker(int nodeA, int nodeB);

  void readPartition(const char *s);
  void randomPartition(int maxCommunities);

private:
  int numberNodes;
  int numberCommunities;
  std::vector<std::list<int>> partition;
  void sortPartition();
};

#endif