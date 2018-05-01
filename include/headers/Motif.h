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

    private:
        int size;
        std::vector<int> communities;
        std::vector< std::vector<int> > adjacencyMatrix;
};

#endif