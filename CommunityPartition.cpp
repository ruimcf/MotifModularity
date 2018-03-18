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
    partition.clear();
    partition.reserve(numberCommunities);
    for (int i = 0; i < numberCommunities; ++i)
    {
        list<int> a;
        partition.push_back(a);
    }
}

void CommunityPartition::setNodeCommunity(int nodeId, int communityId)
{
    int nodeCommunity = CommunityPartition::getNodeCommunity(nodeId);

    //the node is already in the desired position
    if (nodeCommunity == communityId)
        return;

    //if the node is in a community, remove it from there
    if (nodeCommunity >= 0)
    {
        partition[nodeCommunity].remove(nodeId);
    }

    //Insert node in desired community
    // if empty, insert immediatly
    if (partition[nodeCommunity].empty())
    {
        partition[nodeCommunity].push_back(nodeId);
        return;
    }

    // Insert in order
    for (list<int>::iterator it = partition[nodeCommunity].begin(); it != partition[nodeCommunity].end(); ++it)
    {
        if ((*it) > nodeId)
        {
            partition[nodeCommunity].insert(it, nodeId);
            return;
        }
    }

    // Insert in last
    partition[nodeCommunity].push_back(nodeId);

    //NEED TO ORDER!!!!!!!!
}

int CommunityPartition::getNodeCommunity(int nodeId)
{
    for (vector<list<int>>::iterator v_it = partition.begin(); v_it != partition.end(); ++v_it)
    {
        for (list<int>::iterator l_it = (*v_it).begin(); l_it != (*v_it).end(); ++l_it)
        {
            if ((*l_it) > nodeId)
            {
                break;
            }
            if ((*l_it) == nodeId)
            {
                return std::distance(partition.begin(), v_it);
            }
        }
    }
    return -1;
}

int CommunityPartition::kronecker(int nodeA, int nodeB)
{
    return CommunityPartition::getNodeCommunity(nodeA) == CommunityPartition::getNodeCommunity(nodeB) ? 1 : 0;
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