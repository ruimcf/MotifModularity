#include "Partition.h"

Partition::Partition(int a)
{
    numberOfNodes = a;
    partition = new int[a];
}

Partition::Partition()
{
    partition = NULL;
}

void Partition::setNumberNodes(int number)
{
    if (partition == NULL)
    {
        partition = new int[number];
    }
    else
    {
        printf("DESTROYED PREVIOUS PARTITION\n");
        delete[] partition;
        partition = new int[number];
    }
    numberOfNodes = number;
}

void Partition::setNodeCommunity(int nodeId, int communityId)
{
    partition[nodeId] = communityId;
}

int Partition::getNodeCommunity(int nodeId)
{
    return partition[nodeId];
}

int Partition::kronecker(int nodeA, int nodeB)
{
    return partition[nodeA] == partition[nodeB] ? 1 : 0;
}

void Partition::randomPartition(int maxCommunities)
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

int Partition::numberCommunities()
{
    return numberOfCommunities;
}

int Partition::numberNodes()
{
    return numberOfNodes;
}

void Partition::readPartition(const char *s)
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

int *Partition::getPartition()
{
    return partition;
}