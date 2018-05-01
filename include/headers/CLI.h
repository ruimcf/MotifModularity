#include "GraphMatrix.h"
#include "GraphUtils.h"
#include "Error.h"
#include <string>
#include <math.h>
#include "FailObject.h"
#include "ArrayPartition.h"
#include "Motif.h"
#include <iostream>
#include <fstream>
#include <sstream>

class CLI
{
private:
  static bool directed;
  static bool weighted;
  static string partitionFile;
  static string networkFile;
  static string motifFile;
  static bool readMotif;
  static bool readPartition;
  static int seed;

  static Motif motif;
  static void motifModularity();
  static void nodeCombination(int offset, int left);
  static void countCombinationMotifs();

  static void setNodes();

  static ofstream resultsFile;

  static Graph *g;
  // static int *partition;
  static ArrayPartition networkPartition;
  static vector<int> nodes;
  static vector<int> combination;
  static double n1;
  static double n2;
  static double n3;
  static double n4;
  static int kronecker(int a, int b);
  static int maskedWeight(int a, int b);
  static int nullcaseWeight(int a, int b);
  static int maskedNullcaseWeight(int a, int b);

  static void createAllPartitions();
  static void createAllPartitionsStep(int level, int numNodes);

  static double cicleModularity(int size);
  static void iterateCombinations(int offset, int k);
  static void computeCicleModularity();
  static void combinationCicleModularity();
  static double triangleModularity();

  static std::vector<double> constantMotifValues();
  static double triangleModularityPreCalculated(std::vector<double> constantMotifValues);

  static std::vector<double> firstIterationTriangleModularity();
  static std::vector<double> changingNodeTriangleModularity(std::vector<double> previousValues, int changedNode, int previousCommunity);

  static double singleNodeGreedyAlgorithm();

  static void openResultsFile();
  static void writeLineToFile(string line);
  static void closeResultsFile();

  static void parseArgs(int argc, char **argv);
public:
  static void start(int argc, char **argv);
};

void pretty_print(const vector<int> &v);
string int_array_to_string(int int_array[], int size_of_array);