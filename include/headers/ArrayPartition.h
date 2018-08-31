#ifndef _ARRAYPARTITION_
#define _ARRAYPARTITION_
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "Error.h"

class ArrayPartition
{
public:
  ArrayPartition(int);
  ArrayPartition();

  void setNumberNodes(int number);
  void setNodeCommunity(int nodeId, int communityId);
  int getNodeCommunity(int nodeId);
  int getNumberCommunities();
  int getNumberNodes();

  int kronecker(int nodeA, int nodeB);

  void readPartition(const char *s);
  void randomPartition(int maxCommunities);

  std::vector<int> getPartitionByNode();
  std::string toStringPartitionByNode();

  int getNumberOfDifferentPartitions();

  // std::string toStringPartitionByCommunity();

  void writePartitionFile(std::string name, bool isRealCommunity = false);

private:
  int numberNodes;
  std::vector<int> partition;
  int numberCommunities;
};

#endif