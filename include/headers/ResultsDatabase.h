#ifndef _RESULTS_DATABASE_
#define _RESULTS_DATABASE_

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

class ResultsDatabase 
{
    public:
        ResultsDatabase();
        void addColumn(std::string columnName);
        void addEntryToColumn(std::string columnName, double result);
        void printTable();
        void printToCSVFile();
        void readFromCSVFile();
    private:
        std::map< std::string, std::vector< double > > table;
        void printHeaders();
};

#endif