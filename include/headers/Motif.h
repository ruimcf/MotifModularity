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
        std::vector< std::vector<int> > getAdjacencyList();
        std::vector< std::vector<int> > getAdjacencyListSize(int size);
        std::vector<int> getCommunities();
        std::vector< std::vector<int> > getOrbitRules();

    private:
        bool directed;
        int size;
        std::vector<int> communities;
        std::vector< std::vector<int> > adjacencyMatrix;
        std::vector< std::vector<int> > adjacencyList;
        std::vector< std::vector< std::vector<int> > > adjacencyListSizes;
        void setAdjacencyMatrix();
        void calculateOrbits();
        void go(int pos);
        std::vector< std::vector<bool> > orbits;
        std::vector<bool> used;
        std::vector<int> perm;
        std::vector< std::vector<int> > orbitRules;
        void setOrbitRules();
};

#endif