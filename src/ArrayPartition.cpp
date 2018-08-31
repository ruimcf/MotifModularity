#include "ArrayPartition.h"
#include "Random.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

ArrayPartition::ArrayPartition(int a)
{
    numberNodes = a;
    partition.reserve(a);
}

ArrayPartition::ArrayPartition()
{
}

void ArrayPartition::setNumberNodes(int number)
{
    partition.resize(number);
    numberNodes = number;
}

void ArrayPartition::setNodeCommunity(int nodeId, int communityId)
{
    partition.at(nodeId) = communityId;
}

int ArrayPartition::getNodeCommunity(int nodeId)
{
    return partition.at(nodeId);
}

int ArrayPartition::kronecker(int nodeA, int nodeB)
{
    return partition.at(nodeA) == partition.at(nodeB) ? 1 : 0;
}

void ArrayPartition::randomPartition(int maxCommunities)
{
    std::vector<int> communities(maxCommunities, 0);
    for (int nodeId = 0; nodeId < numberNodes; nodeId++)
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
    numberCommunities = numberCommunities;
}

int ArrayPartition::getNumberCommunities()
{
    return numberCommunities;
}

int ArrayPartition::getNumberNodes()
{
    return numberNodes;
}

void ArrayPartition::readPartition(const char *s)
{
    FILE *f = fopen(s, "r");
    if (!f)
        Error::msg(NULL);

    int a, size = 0;

    while (fscanf(f, "%d", &a) == 1)
    {
        partition.at(size) = a;
        size++;
    }
    fclose(f);
}

std::vector<int> ArrayPartition::getPartitionByNode()
{
    return partition;
}

std::string ArrayPartition::toStringPartitionByNode()
{
    std::stringstream ss;
    for(int i = 0; i < numberNodes; i++)
    {
        ss << std::to_string(partition[i]) << " ";
    }

    return ss.str();
}

void ArrayPartition::writePartitionFile(std::string name, bool isRealCommunity){
    ofstream file;
    std::string filePath = "results/partitions/"+name+".tsv";
    file.open(filePath, ios::trunc);
    if(!file.is_open()){
        cout << "Error opening partition file for writing" << endl;
        return;
    }
    if(isRealCommunity)
        file << "Id\tRealCommunity" << endl;
    else
        file << "Id\tCommunity" << endl;

    for(int i = 0; i < numberNodes; i++){
        file << i+1 << "\t" << partition[i] << endl;
    }

    cout << "Wrote partition to file " << filePath << endl;
}

int ArrayPartition::getNumberOfDifferentPartitions()
{
    vector<int> differentPartitions;
    differentPartitions.reserve(numberNodes);
    for(int i = 0; i < numberNodes; ++i)
    {
        if(find(differentPartitions.begin(), differentPartitions.end(), partition[i]) == differentPartitions.end())
        {
            differentPartitions.push_back(partition[i]);
        }
    }
    return differentPartitions.size();
}