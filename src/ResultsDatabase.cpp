#include "ResultsDatabase.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

ResultsDatabase::ResultsDatabase()
{

}

void ResultsDatabase::addColumn (string columnName)
{
    vector<double> newTableColumn;
    table.insert(make_pair(columnName, newTableColumn));
    cout << "Added a new column: " << columnName << endl;
}

template <typename C, class T> void findit(const C& c, T val) {  
    cout << "Trying find() on value " << val << endl;  
    auto result = c.find(val);  
    if (result != c.end()) {  
        cout << "Element found: "; print_elem(*result); cout << endl;  
    } else {  
        cout << "Element not found." << endl;  
    }  
} 

void ResultsDatabase::addEntryToColumn(string columnName, double result)
{
    map<string, vector<double> >::iterator findResult = table.find(columnName);  
    if (findResult != table.end()) {  
        vector<double> &column = (findResult->second);
        column.push_back(result);
        cout << "Added result " << result << " to column " << columnName << endl;
    } else {  
        cout << "Column "<< columnName << " not found." << endl;  
    }   
}

void ResultsDatabase::printTable()
{
    printHeaders();
    bool working = true;
    int index = 0;
    while (working)
    {
        bool stillHaveResults = false;
        map<string, vector<double> >::iterator tableIterator = table.begin();
        while (tableIterator != table.end())
        {
            vector<double> &column = tableIterator->second;
            if(column.size() > index)
            {
                stillHaveResults = true;
            }
            ++tableIterator;
        }

        if(stillHaveResults)
        {
            cout << index+1 << "\t";
            tableIterator = table.begin();
            while (tableIterator != table.end())
            {
                vector<double> &column = tableIterator->second;
                if(column.size() > index)
                {
                    cout << "| " << column[index] << "\t";
                }
                else
                {
                    cout << "| -\t";
                }
                ++tableIterator;
            }
            cout << endl;
            index += 1;
        }
        else {
            working = false;
        }
    }
}

void ResultsDatabase::printHeaders()
{
   map<string, vector<double> >::iterator tableIterator = table.begin();
   cout << "step\t";
    while (tableIterator != table.end())
    {
        string columnName = tableIterator->first;
        cout << "| " << columnName << " ";
        ++tableIterator;
    } 
    cout << endl;
}

void ResultsDatabase::printToCSVFile()
{
    string resultPath = "results/results-table.csv";
    ofstream resultsFile(resultPath);
     if (resultsFile.is_open())
    {
        cout << "Opened file for results " << resultPath << endl;
        // write headers
        resultsFile << "step";
        for(auto entry : table)
        {
            resultsFile << "," << entry.first;
        }
        resultsFile << endl;

        // write values
        bool working = true;
        int index = 0;
        int maxValue = 0;
        for(auto entry : table)
        {
            if(entry.second.size() > maxValue)
                maxValue = entry.second.size();
        }
        for(int index = 0; index < maxValue; ++index)
        {
            resultsFile << index+1;
            for(auto entry : table)
            {
                vector<double> &column = entry.second;
                resultsFile << ",";
                if(column.size() > index)
                    resultsFile << column[index];
            }
            resultsFile << endl;
        }
        resultsFile.close();
    }
    else
    {
        cout << "Failed to open File " << resultPath << endl;
    }

}