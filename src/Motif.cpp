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

void Motif::readFromFile(std::string path)
{
    ifstream ifs;
    ifs.open(path, ifstream::in);
    if(!ifs.is_open()){
        cout << "Error opening motif file" << endl;
        return;
    }
    int _size;
    ifs >> _size;
    size = _size;
    vector<int> _communities;
    for(int i = 0; i < size; i++)
    {
        int nodeCommunity;
        ifs >> nodeCommunity;
        _communities.push_back(nodeCommunity);
    }
    vector< vector<int> > adjMatrix;
    for(int i = 0; i < size; i++){
        vector<int> line;
        for(int j = 0; j < size; j++){
            int hasEdge;
            ifs >> hasEdge;
            line.push_back(hasEdge);
        }
        adjMatrix.push_back(line);
    }

    communities = _communities;
    adjacencyMatrix = adjMatrix;
}

void Motif::print()
{
    cout << "Size: " << size << endl;
    cout << "Communities:" << endl;
    for(int i = 0; i < size; i++)
    {
        cout << communities[i] << "\t";
    }
    cout << endl;
    cout << "Adjacency Matrix:" << endl;
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            cout << adjacencyMatrix.at(i).at(j) << "\t";
        }
        cout << endl;
    } 
}