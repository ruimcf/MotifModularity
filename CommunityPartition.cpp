#include "CommunityPartition.h"
using namespace std;

CommunityPartition::CommunityPartition()
{
}

void CommunityPartition::setNumberNodes(int number)
{
    numberNodes = number;
}

void CommunityPartition::setNumberCommunities(int number)
{
    numberCommunities = number;
}

void CommunityPartition::init(int nNodes, int nCommunities)
{
    numberNodes = nNodes;
    numberCommunities = nCommunities;
    nodePartition.clear();
    communityPartition.clear();
    nodePartition.reserve(numberNodes);
    communityPartition.reserve(numberCommunities);
    for (int i = 0; i < numberCommunities; i++)
    {
        vector<int> a;
        communityPartition.push_back(a);
    }
}

void CommunityPartition::setNodeCommunity(int nodeId, int communityId)
{
    partition[nodeId] = communityId;
}

int CommunityPartition::getNodeCommunity(int nodeId)
{
    return partition[nodeId];
}

int CommunityPartition::kronecker(int nodeA, int nodeB)
{
    return partition[nodeA] == partition[nodeB] ? 1 : 0;
}

void CommunityPartition::randomPartition(int maxCommunities)
{
    std::vector<int> communities(maxCommunities, 0);
    srand(time(NULL));
    for (int nodeId = 0; nodeId < numberOfNodes; nodeId++)
    {
        int randomCommunity = rand() % maxCommunities;
        communities[randomCommunity] += 1;
        partition[nodeId] = randomCommunity;
    }
    int numberCommunities = 0;
    for (std::vector<int>::iterator it = communities.begin(); it != communities.end(); ++it)
        if (*it > 0)
            numberCommunities++;

    printf("Number of communities: %d\n", numberCommunities);
    numberOfCommunities = numberCommunities;
}

int CommunityPartition::numberCommunities()
{
    return numberOfCommunities;
}

int CommunityPartition::numberNodes()
{
    return numberOfNodes;
}

void CommunityPartition::readPartition(const char *s)
{
    FILE *f = fopen(s, "r");
    if (!f)
        Error::msg(NULL);

    int a, size = 0;

    while (fscanf(f, "%d", &a) == 1)
    {
        partition[size] = a;
        size++;
    }
    fclose(f);
}

int *CommunityPartition::getPartition()
{
    return partition;
}