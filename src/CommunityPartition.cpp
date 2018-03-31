#include "CommunityPartition.h"
#include "Random.h"

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
    CommunityPartition::insertNodeCommunity(nodeId, communityId);

    //Might be necessary to reorder the communities
    CommunityPartition::sortPartition();
}

void CommunityPartition::insertNodeCommunity(int nodeId, int communityId)
{
    // if empty, insert immediatly
    if (partition[communityId].empty())
    {
        partition[communityId].push_back(nodeId);
        return;
    }

    // Insert in order
    for (list<int>::iterator it = partition[communityId].begin(); it != partition[communityId].end(); ++it)
    {
        if ((*it) > nodeId)
        {
            partition[communityId].insert(it, nodeId);
            return;
        }
    }

    // Insert in last
    partition[communityId].push_back(nodeId);
}

bool compareFunction(const list<int> &i, const list<int> &j)
{
    if (i.empty() && j.empty())
        return true;

    if (!i.empty() && j.empty())
        return true;

    if (i.empty() && !j.empty())
        return false;

    if (i.front() < j.front())
        return true;

    return false;
}

void CommunityPartition::sortPartition()
{
    std::sort(partition.begin(), partition.end(), compareFunction);
}

int CommunityPartition::getNodeCommunity(int nodeId)
{
    for (vector< list<int> >::iterator v_it = partition.begin(); v_it != partition.end(); ++v_it)
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
    for (int nodeId = 0; nodeId < numberNodes; nodeId++)
    {
        int randomCommunity = Random::getInteger(0, maxCommunities - 1);
        communities[randomCommunity] += 1;
        CommunityPartition::insertNodeCommunity(nodeId, randomCommunity);
    }
    numberCommunities = 0;
    for (std::vector<int>::iterator it = communities.begin(); it != communities.end(); ++it)
        if (*it > 0)
            numberCommunities++;

    printf("Number of communities: %d\n", numberCommunities);
    CommunityPartition::sortPartition();
}

int CommunityPartition::getNumberCommunities()
{
    return numberCommunities;
}

int CommunityPartition::getNumberNodes()
{
    return numberNodes;
}

void CommunityPartition::readPartition(const char *s)
{
    FILE *f = fopen(s, "r");
    if (!f)
        Error::msg(NULL);

    int communityId, nodeId = 0;

    while (fscanf(f, "%d", &communityId) == 1)
    {
        CommunityPartition::insertNodeCommunity(nodeId, communityId);
        nodeId++;
    }
    fclose(f);
    CommunityPartition::sortPartition();
}

std::vector< std::list<int> > CommunityPartition::getPartition()
{
    return partition;
}

void CommunityPartition::printPartition()
{
    for (int i = 0; i < partition.size(); ++i)
    {
        cout << "Community " << i << ": " << endl;
        for (list<int>::iterator it = partition[i].begin(); it != partition[i].end(); ++it)
        {
            cout << *it << " ";
        }
        cout << endl;
    }
}