#ifndef _COMMUNITYPARTITION_
#define _COMMUNITYPARTITION_
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "Common.h"
#include "Error.h"

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

  int kronecker(int nodeA, int nodeB);

  void readPartition(const char *s);
  void randomPartition(int maxCommunities);

  std::vector<int> getPartitionByNode();
  std::string toStringPartitionByNode();

  std::vector< std::list<int> > getPartitionByCommunity();
  std::string toStringPartitionByCommunity();

private:
  int numberNodes;
  int numberCommunities;
  std::vector< std::list<int> > partition;
  void sortPartition();
  void insertNodeCommunity(int nodeId, int communityId);
};

#endif