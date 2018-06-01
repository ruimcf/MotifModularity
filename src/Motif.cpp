#include "Motif.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

Motif::Motif()
{
    cout << "motif init" << endl;
}

int Motif::getSize()
{
    return size;
}
void Motif::readFromFile(std::string path)
{
    ifstream ifs;
    ifs.open(path, ifstream::in);
    if (!ifs.is_open())
    {
        cout << "Error opening motif file" << endl;
        return;
    }
    ifs >> size;

    string directedOrUndirected;
    ifs >> directedOrUndirected;
    if (directedOrUndirected == "directed")
        directed = true;
    else if (directedOrUndirected == "undirected")
        directed = false;

    communities.clear();
    communities.reserve(size);
    for (int i = 0; i < size; i++)
    {
        int nodeCommunity;
        ifs >> nodeCommunity;
        communities.push_back(nodeCommunity);
    }

    adjacencyListSizes.clear();
    for (int i = 0; i < size; i++)
    {
        vector< vector<int> > adjList;
        adjacencyListSizes.push_back(adjList);
    }

    adjacencyList.clear();
    int node1, node2;
    while (ifs >> node1 && ifs >> node2)
    {
        //Subtract 1 to the node id to match the graph adjacency matrix
        std::vector<int> pair;
        pair.push_back(node1 - 1);
        pair.push_back(node2 - 1);
        int max = pair[0] >= pair[1] ? pair[0] : pair[1];
        adjacencyListSizes.at(max).push_back(pair);
        adjacencyList.push_back(pair);
    }

    calculateOrbits();
}

/**
 * Returns the edges for motif nodes until indice size
 */
const std::vector< std::vector<int> > &Motif::getAdjacencyListSize(int size)

{
    return adjacencyListSizes.at(size);
}

void Motif::print()
{
    cout << "Size: " << size << endl;
    cout << "Communities:" << endl;
    for (int i = 0; i < size; i++)
    {
        cout << communities[i] << "\t";
    }
    cout << endl;
    cout << "Adjacency List:" << endl;
    for (int i = 0; i < adjacencyList.size(); i++)
    {
        cout << adjacencyList.at(i).at(0) << "\t" << adjacencyList.at(i).at(1) << endl;
    }
    cout << "Adjacency List Sizes:" << endl;
    for (int i = 0; i < adjacencyListSizes.size(); i++)
    {
        cout << "Size " << i << endl;
        for (int j = 0; j < adjacencyListSizes.at(i).size(); j++)
        cout << adjacencyListSizes.at(i).at(j).at(0) << "\t" << adjacencyListSizes.at(i).at(j).at(1) << endl;
    }
    std::cout << "Orbit rules:" << std::endl;
    for(int i = 0; i < orbitRules.size(); i++)
    {
        std::cout << orbitRules[i][0] << "\t" << orbitRules[i][1] << std::endl;
    }
}

const std::vector<std::vector<int> > &Motif::getAdjacencyList()
{
    return adjacencyList;
}

bool Motif::isDirected()
{
    return directed;
}

const vector<int> &Motif::getCommunities()
{
    return communities;
}

void Motif::setAdjacencyMatrix()
{
    adjacencyMatrix.clear();
    adjacencyMatrix.reserve(size);
    for (int i = 0; i < size; i++)
    {
        std::vector<int> line(size, 0);
        adjacencyMatrix.push_back(line);
    }
    for (int i = 0; i < adjacencyList.size(); i++)
    {
        int node1 = adjacencyList[i][0];
        int node2 = adjacencyList[i][1];
        adjacencyMatrix[node1][node2] = 1;
        if (!directed)
        {
            adjacencyMatrix[node2][node1] = 1;
        }
    }
}

const std::vector< std::vector<int> > &Motif::getAdjacencyMatrix()
{
    return adjacencyMatrix;
}

void Motif::calculateOrbits()
{
    setAdjacencyMatrix();
    orbits.clear();
    orbits.reserve(size);
    used.clear();
    used.reserve(size);
    perm.clear();
    perm.reserve(size);
    for (int i = 0; i < size; i++)
    {
        std::vector<bool> line(size, false);
        orbits.push_back(line);
        used.push_back(false);
    }

    go(0);
    setOrbitRules();
}

void Motif::go(int pos)
{
    bool ok;

    if (pos == size)
    {
        // for (int i = 0; i < size; i++)
        //     printf("%d", perm[i]);
        // putchar('\n');

        for (int i = 0; i < size; i++)
            orbits[i][perm[i]] = true;
    }
    else
    {
        for (int i = 0; i < size; i++)
        {
            if (!used[i])
            {
                perm[pos] = i;

                ok = true;
                for (int j = 0; j < pos; j++)
                    if (adjacencyMatrix[perm[pos]][perm[j]] != adjacencyMatrix[pos][j])
                    {
                        ok = false;
                        break;
                    }

                if (ok)
                {
                    used[i] = true;
                    go(pos + 1);
                    used[i] = false;
                }
            }
        }
    }
}

const std::vector< std::vector<int> > &Motif::getOrbitRulesSize(int size)
{
    return orbitRulesSize.at(size);
}


void Motif::setOrbitRules()
{
    orbitRules.clear();
    orbitRulesSize.clear();
    for(int i = 0; i < size; ++i)
    {
        std::vector< std::vector<int> > pairsForSize;
        orbitRulesSize.push_back(pairsForSize);
    }

    for(int i = 0; i < size; i++)
    {
        int lastNode = -1;
        for(int j = 0; j < size; j++)
        {
            if(orbits[i][j])
            {
                if(lastNode != -1)
                {
                    bool rulesAlreadyExists = false;
                    for(int k = 0; k < orbitRules.size(); k++)
                        if (orbitRules[k][0] == lastNode && orbitRules[k][1] == j)
                            rulesAlreadyExists = true;

                    bool sizeRulesAlreadyExists = false;
                    for(int k = 0; k < orbitRulesSize[j].size(); k++){
                        if(orbitRulesSize[j][k][0] == lastNode)
                            sizeRulesAlreadyExists = true;
                    } 

                    if(!sizeRulesAlreadyExists) {
                        std::vector<int> pair;
                        pair.push_back(lastNode);
                        pair.push_back(j);
                        orbitRulesSize[j].push_back(pair);
                        // cout << "Orbit Rule added size " << j << ": " << lastNode << " - " << j << endl;
                    }

                    if(!rulesAlreadyExists) {
                        std::vector<int> pair;
                        pair.push_back(lastNode);
                        pair.push_back(j);
                        orbitRules.push_back(pair);
                    }
                } 
                lastNode = j;
            }
        }
    }
}

const std::vector< std::vector<int> > &Motif::getOrbitRules()
{
    return orbitRules;
}