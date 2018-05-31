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
        void print();
        int getSize();
        bool isDirected();
        const std::vector< std::vector<int> > &getAdjacencyList();
        const std::vector< std::vector<int> > &getAdjacencyListSize(int size);
        const std::vector<int> &getCommunities();
        const std::vector< std::vector<int> > &getOrbitRules();
        const std::vector< std::vector<int> > &getOrbitRulesSize(int size);

    private:
        bool directed;
        int size;
        std::vector<int> communities;
        std::vector< std::vector<int> > adjacencyMatrix;
        std::vector< std::vector<int> > adjacencyList;
        //at each pos, the edges that connect to node pos and pos is > than the other node
        std::vector< std::vector< std::vector<int> > > adjacencyListSizes;
        void setAdjacencyMatrix();
        void calculateOrbits();
        void go(int pos);
        std::vector< std::vector<bool> > orbits;
        std::vector<bool> used;
        std::vector<int> perm;
        std::vector< std::vector<int> > orbitRules;
        std::vector< std::vector< std::vector<int> > > orbitRulesSize;
        void setOrbitRules();
};

#endif