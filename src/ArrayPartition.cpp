#include "ArrayPartition.h"
#include "Random.h"
#include <iostream>
#include <fstream>

ArrayPartition::ArrayPartition(int a)
{
    numberOfNodes = a;
    partition = new int[a];
}

ArrayPartition::ArrayPartition()
{
    partition = NULL;
}

void ArrayPartition::setNumberNodes(int number)
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

void ArrayPartition::setNodeCommunity(int nodeId, int communityId)
{
    partition[nodeId] = communityId;
}

int ArrayPartition::getNodeCommunity(int nodeId)
{
    return partition[nodeId];
}

int ArrayPartition::kronecker(int nodeA, int nodeB)
{
    return partition[nodeA] == partition[nodeB] ? 1 : 0;
}

void ArrayPartition::randomPartition(int maxCommunities)
{
    std::vector<int> communities(maxCommunities, 0);
    for (int nodeId = 0; nodeId < numberOfNodes; nodeId++)
    {
        int randomCommunity = Random::getInteger(0, maxCommunities - 1);
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

int ArrayPartition::numberCommunities()
{
    return numberOfCommunities;
}

int ArrayPartition::numberNodes()
{
    return numberOfNodes;
}

void ArrayPartition::readPartition(const char *s)
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

int *ArrayPartition::getPartition()
{
    return partition;
}

void ArrayPartition::writePartitionFile(std::string name){
    ofstream file;
    file.open("results/partitions/"+name+".tsv", ios::trunc);
    if(!file.is_open()){
        cout << "Error opening partition file for writing" << endl;
        return;
    }
    file << "Id Community" << endl;
    for(int i = 0; i < numberOfNodes; i++){
        file << i+1 << " " << partition[i] << endl;
    }

    cout << "Wrote partition to file " << "results/partitions/"+name+".tsv" << endl;
}