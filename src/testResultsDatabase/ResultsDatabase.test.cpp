#include "ResultsDatabase.h"



void startTest(){
    ResultsDatabase resultsDatabase;
    resultsDatabase.readFromCSVFile();
    resultsDatabase.readFromCSVFile();
    resultsDatabase.printTable();
    // resultsDatabase.addColumn("Portugal");
    // resultsDatabase.addEntryToColumn("Portugal", 3.0);
    // resultsDatabase.addEntryToColumn("Espanha", 3.0);
    // resultsDatabase.addEntryToColumn("Portugal", 2.0);
    // resultsDatabase.printTable();
    // resultsDatabase.addColumn("Espanha");
    // resultsDatabase.addEntryToColumn("Espanha", 14.2);
    // resultsDatabase.printToCSVFile();

    // resultsDatabase.addColumn("France");
    // resultsDatabase.addEntryToColumn("France", .22231);
    // resultsDatabase.addEntryToColumn("Portugal", 2.0);
    // resultsDatabase.addEntryToColumn("Portugal", 2.0);
    // resultsDatabase.addEntryToColumn("Espanha", 14.2);
    // resultsDatabase.printToCSVFile();
}

int main(int argc, char ** argv)
{
    startTest();

    return 0;
};

