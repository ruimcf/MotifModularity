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
        std::vector<int> getCommunities();

    private:
        bool directed;
        int size;
        std::vector<int> communities;
        // std::vector< std::vector<int> > adjacencyMatrix;
        std::vector< std::vector<int> > adjacencyList;
};

#endif