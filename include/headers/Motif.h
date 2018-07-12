#ifndef _MOTIF_
#define _MOTIF_

#include <stdlib.h>
#include <string>
#include <vector>

class Motif
{
    public:
        Motif();
        void readFromFile(std::string path);
        int getCommunity(int nodeId);
        int getCommunityWithOrder(int nodeId);
        void print();
        int getSize();
        bool isDirected();
        const std::vector< std::vector<int> > &getAdjacencyList();
        const std::vector< std::vector<int> > &getAdjacencyListWithOrder();
        const std::vector< std::vector<int> > &getAdjacencyListSize(int size);
        const std::vector< std::vector<int> > &getAdjacencyListSizeWithOrder(int size);
        const std::vector< std::vector<int> > &getAdjacencyMatrix();
        const std::vector<int> &getCommunities();
        const std::vector< std::vector<int> > &getOrbitRules();
        const std::vector< std::vector<int> > &getOrbitRulesWithOrder();
        const std::vector< std::vector<int> > &getOrbitRulesSize(int size);
        const std::vector< std::vector<int> > &getOrbitRulesSizeWithOrder(int size);
        bool hasEdge(int nodeA, int nodeB);
        bool hasEdgeWithOrder(int nodeA, int nodeB);
        vector<int> getNeighbours(int node);
        vector<int> getNeighboursWithOrder(int node);
        vector<int> getInNeighbours(int node);
        vector<int> getInNeighboursWithOrder(int node);
        vector<int> getOutNeighbours(int node);
        vector<int> getOutNeighboursWithOrder(int node);

    private:
        std::vector<int> nodesOrder;
        bool directed;
        int size;
        std::vector<int> communities;
        std::vector< std::vector<int> > adjacencyMatrix;
        int getNodeDegree(int node);
        void createNodesOrder();
        std::vector< std::vector<int> > adjacencyList;
        std::vector< std::vector<int> > adjacencyListWithOrder;
        //at each pos, the edges that connect to node pos and pos is > than the other node
        std::vector< std::vector< std::vector<int> > > adjacencyListSizes;
        std::vector< std::vector< std::vector<int> > > adjacencyListSizesWithOrder;
        void setAdjacencyMatrix();
        void calculateOrbits();
        void calculateOrbitsIteration(int pos);
        bool fixOneNode();
        std::vector< std::vector<bool> > orbits;
        std::vector<bool> used;
        std::vector<int> perm;
        std::vector< std::vector<int> > orbitRules;
        std::vector< std::vector<int> > orbitRulesWithOrder;
        std::vector< std::vector< std::vector<int> > > orbitRulesSize;
        std::vector< std::vector< std::vector<int> > > orbitRulesSizeWithOrder;
        void setOrbitRulesWithOrder();
        void setAdjacencyListWithOrder();
};

#endif