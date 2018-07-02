#include "Motif.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

Motif::Motif()
{
    // cout << "motif init" << endl;
}

int Motif::getSize()
{
    return size;
}

/**
 * Format of a motif file to be read
 * <int> - motif size
 * <string> - "directed" or "undirected"
 * #size <int> - community for each node, -1 means the node can be in any
 *               community, same number means the nodes will be in the same
 *               community, different numbers means those nodes will be in
 *               different communities
 * List of <int> <int> - each pair of int represents a edge on the motif where
 *                       each int is the node ID
 *                       node ID MUST BE from 1 to SIZE inclusive
 */
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
    else
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
    setAdjacencyListWithOrder();
}

void Motif::setAdjacencyListWithOrder()
{
    adjacencyListWithOrder.clear();
    adjacencyListSizesWithOrder.clear();
    for (int i = 0; i < size; i++)
    {
        vector< vector<int> > adjList;
        adjacencyListSizesWithOrder.push_back(adjList);
    } 

    for(int i = 0; i < adjacencyList.size(); ++i)
    {
        int firstNode = adjacencyList[i][0];
        int firstNodeOrdered = distance(nodesOrder.begin(), find(nodesOrder.begin(), nodesOrder.end(), firstNode));

        int secondNode = adjacencyList[i][1];
        int secondNodeOrdered = distance(nodesOrder.begin(), find(nodesOrder.begin(), nodesOrder.end(), secondNode));

        if(firstNode >= size || secondNode >= size)
        {
            cout << "Error finding node" << endl;
            throw;
        }

        vector<int> pair;
        pair.push_back(firstNodeOrdered);
        pair.push_back(secondNodeOrdered);
        adjacencyListWithOrder.push_back(pair);
        if(firstNodeOrdered < secondNodeOrdered)
            adjacencyListSizesWithOrder[secondNodeOrdered].push_back(pair);
        else
            adjacencyListSizesWithOrder[firstNodeOrdered].push_back(pair);
    }
}

/**
 * Returns the edges for motif nodes until indice size
 */
const std::vector< std::vector<int> > &Motif::getAdjacencyListSize(int size)
{
    return adjacencyListSizes.at(size);
}

const std::vector< std::vector<int> > &Motif::getAdjacencyListSizeWithOrder(int size)
{
    return adjacencyListSizesWithOrder.at(size);
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
    cout << "Adjacency List With Order:" << endl;
    for (int i = 0; i < adjacencyListWithOrder.size(); i++)
    {
        cout << adjacencyListWithOrder.at(i).at(0) << "\t" << adjacencyListWithOrder.at(i).at(1) << endl;
    }
    std::cout << "Orbit rules: ";
    for(int i = 0; i < orbitRules.size(); i++)
    {
        std::cout << orbitRules[i][0] << "<" << orbitRules[i][1] << "\t";
    }
    cout << std::endl;

    std::cout << "Orbit rules size: " << std::endl;
    for(int i = 0; i < orbitRulesSize.size(); i++)
    {
        cout << i << ": ";
        if(orbitRulesSize[i].empty())
            cout << "No rules.";
        else
            for(int j = 0; j < orbitRulesSize[i].size(); ++j)
                std::cout << orbitRulesSize[i][j][0] << "<" << orbitRulesSize[i][j][1] << "\t";
        cout << endl;
    }

    std::cout << "Orbit rules with order: ";
    for(int i = 0; i < orbitRulesWithOrder.size(); i++)
    {
        std::cout << orbitRulesWithOrder[i][0] << "<" << orbitRulesWithOrder[i][1] << " ";
    }
    cout << endl;

    std::cout << "Orbit rules size with order: " << std::endl;
    for(int i = 0; i < orbitRulesSizeWithOrder.size(); i++)
    {
        cout << i << ": ";
        if(orbitRulesSizeWithOrder[i].empty())
            cout << "No rules.";
        else
            for(int j = 0; j < orbitRulesSizeWithOrder[i].size(); ++j)
                std::cout << orbitRulesSizeWithOrder[i][j][0] << "<" << orbitRulesSizeWithOrder[i][j][1] << "\t";
        cout << endl;
    }
    cout << "Motif nodes order:";
    for(int i = 0; i < nodesOrder.size(); ++i)
    {
        cout << " " << nodesOrder[i];
    }
    cout << endl;
    cout << "------------------" << endl;

}

const std::vector<std::vector<int> > &Motif::getAdjacencyList()
{
    return adjacencyList;
}

const std::vector<std::vector<int> > &Motif::getAdjacencyListWithOrder()
{
    return adjacencyListWithOrder;
}

bool Motif::isDirected()
{
    return directed;
}

const vector<int> &Motif::getCommunities()
{
    return communities;
}

int Motif::getCommunity(int nodePos)
{
    return communities[nodePos];
}

int Motif::getCommunityWithOrder(int nodePos)
{
    int orderedNodePos = nodesOrder[nodePos];
    return communities[orderedNodePos];
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

    calculateOrbitsIteration(0);
    setOrbitRules();
    //relying on calculate orbits to compute the adjacency matrix
    createNodesOrder();
    setOrbitRulesWithOrder();
}

void Motif::calculateOrbitsIteration(int pos)
{

    if (pos == size)
        for (int i = 0; i < size; i++)
            orbits[i][perm[i]] = true;
    else
        for (int i = 0; i < size; i++)
            if (!used[i])
            {
                perm[pos] = i;
                bool ok = true;

                for (int j = 0; j < pos; j++)
                    if (adjacencyMatrix[perm[pos]][perm[j]] != adjacencyMatrix[pos][j])
                    {
                        ok = false;
                        break;
                    }

                if (ok)
                {
                    used[i] = true;
                    calculateOrbitsIteration(pos + 1);
                    used[i] = false;
                }
            }
}

const std::vector< std::vector<int> > &Motif::getOrbitRulesSize(int size)
{
    return orbitRulesSize.at(size);
}

const std::vector< std::vector<int> > &Motif::getOrbitRulesSizeWithOrder(int size)
{
    return orbitRulesSizeWithOrder.at(size);
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
                // The first node will need to wait to be in a rule with another node
                if(lastNode != -1)
                {
                    bool rulesAlreadyExists = false;
                    for(int k = 0; k < orbitRules.size(); k++)
                        if (orbitRules[k][0] == lastNode && orbitRules[k][1] == j)
                        {
                            rulesAlreadyExists = true;
                            break;
                        }

                    bool sizeRulesAlreadyExists = false;
                    for(int k = 0; k < orbitRulesSize[j].size(); k++)
                        if(orbitRulesSize[j][k][0] == lastNode)
                        {
                            sizeRulesAlreadyExists = true;
                            break;
                        } 

                    std::vector<int> orbitRule;
                    orbitRule.push_back(lastNode);
                    orbitRule.push_back(j);

                    if(!sizeRulesAlreadyExists)
                        orbitRulesSize[j].push_back(orbitRule);

                    if(!rulesAlreadyExists)
                        orbitRules.push_back(orbitRule);
                } 
                lastNode = j;
            }
        }
    }
}

void Motif::setOrbitRulesWithOrder()
{
    orbitRulesWithOrder.clear();
    orbitRulesSizeWithOrder.clear();
    for(int i = 0; i < size; ++i)
    {
        std::vector< std::vector<int> > pairs;
        orbitRulesSizeWithOrder.push_back(pairs);
    }

    for(int i = 0; i < orbitRules.size(); i++)
    {
        int firstNode = orbitRules[i][0];
        int firstNodeOrdered = distance(nodesOrder.begin(), find(nodesOrder.begin(), nodesOrder.end(), firstNode));

        int secondNode = orbitRules[i][1];
        int secondNodeOrdered = distance(nodesOrder.begin(), find(nodesOrder.begin(), nodesOrder.end(), secondNode));

        if(firstNode >= size || secondNode >= size)
        {
            cout << "Error finding node" << endl;
            throw;
        }
        
        vector<int> rulePair;
        rulePair.push_back(firstNodeOrdered);
        rulePair.push_back(secondNodeOrdered);
        orbitRulesWithOrder.push_back(rulePair);
        orbitRulesSizeWithOrder.at(secondNodeOrdered).push_back(rulePair);
    }
}

const std::vector< std::vector<int> > &Motif::getOrbitRules()
{
    return orbitRules;
}

const std::vector< std::vector<int> > &Motif::getOrbitRulesWithOrder()
{
    return orbitRulesWithOrder;
}

void Motif::createNodesOrder()
{
    nodesOrder.clear();
    nodesOrder.reserve(size);
    int nodeWithMostDegree, bestDegree = 0;
    for(int i = 0; i < size; ++i)
    {
        int nodeDegree = getNodeDegree(i);
        if(nodeDegree > bestDegree)
        {
            bestDegree = nodeDegree;
            nodeWithMostDegree = i;
        }
    }
    nodesOrder.push_back(nodeWithMostDegree);

    std::vector<bool> availableNodes;
    availableNodes.assign(size, true);
    availableNodes[nodeWithMostDegree] = false;

    for(int i = 0; i < size-1; i++)
    {
        int nextBestNode;
        int nextBestNodeDegree = 0;
        for(int currentNode = 0; currentNode < size; ++currentNode)
        {
            if(availableNodes.at(currentNode))
            {
                // how many edges this node has with all the nodes already selected?
                int nodeDegree = 0;
                for(int k = 0; k < nodesOrder.size(); ++k)
                {
                    int node = nodesOrder.at(k);
                    if(hasEdge(currentNode, node))
                        nodeDegree += 1;
                    if(isDirected())
                        if(hasEdge(node, currentNode))
                            nodeDegree += 1;
                }

                if(nodeDegree > nextBestNodeDegree)
                {
                    nextBestNode = currentNode;
                    nextBestNodeDegree = nodeDegree;
                }
                else if (nodeDegree == nextBestNodeDegree)
                {
                    if(getNodeDegree(currentNode) > getNodeDegree(nextBestNode))
                    {
                        nextBestNode = currentNode;
                        nextBestNodeDegree = nodeDegree;
                    }
                }
            }
        }
        availableNodes.at(nextBestNode) = false;
        nodesOrder.push_back(nextBestNode);
    }

}

int Motif::getNodeDegree(int node)
{
    int degree = 0;
    for(int i = 0; i < size; ++i)
    {
        if(node != i)
        {
            if(hasEdge(node, i))
                degree += 1;

            if(isDirected())
                if(hasEdge(i, node))
                    degree += 1;
        }
    }
    return degree;
}

bool Motif::hasEdge(int nodeA, int nodeB)
{
    return adjacencyMatrix[nodeA][nodeB] == 1 ? true : false;
}

bool Motif::hasEdgeWithOrder(int nodeA, int nodeB)
{
    int orderedNodeA = nodesOrder[nodeA];
    int orderedNodeB = nodesOrder[nodeB];
    return adjacencyMatrix[orderedNodeA][orderedNodeB] == 1 ? true : false;
}