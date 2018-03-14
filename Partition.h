#ifndef _PARTITION_
#define _PARTITION_
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "gtries/Error.h"

class Partition
{
  public:
    Partition(int);
    Partition();
    void setNumberNodes(int number);
    void setNodeCommunity(int nodeId, int communityId);
    int getNodeCommunity(int nodeId);
    int numberCommunities();
    int numberNodes();
    int *getPartition();

    int kronecker(int nodeA, int nodeB);

    void readPartition(const char *s);
    void randomPartition(int maxCommunities);

  private:
    int numberOfNodes;
    int *partition;
    int numberOfCommunities;
};

#endif