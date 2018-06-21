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

struct MotifConstantValues {
  long numberMotifsInGraph;
  long degreeMotifsRandomGraph;
};

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
  static double motifModularity();
  static void nodeCombination(int offset);
  static void countCombinationMotifs();
  static bool combinationHasMotifEdges();
  static bool combinationHasMotifCommunities();
  static int combinationNullcaseWeights();

  static double optimizedMotifModularity();
  static void optimizedNodeCombination(int offset, bool edgesCheck, bool communitiesCheck);
  static bool optimizedCombinationHasMotifEdges();
  static bool optimizedCombinationHasMotifCommunities();
  static bool optimizedCombinationOrbitRules();

  static MotifConstantValues getMotifConstantValues();
  static void motifConstantValuesIteration(int offset, MotifConstantValues *ptr);
  static double optimizedMotifModularityWithConstantValues(MotifConstantValues motifConstantValues);
  static void optimizedNodeCombinationWithConstantValues(int offset, bool edgesCheck, bool communitiesCheck);

  static void setNodes();

  static ofstream resultsFile;

  static Graph *g;
  // static int *partition;
  static ArrayPartition networkPartition;
  static vector<int> nodes;
  static vector<int> combination;
  static vector<bool> used;
  static long n1;
  static long n2;
  static long n3;
  static long n4;
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
  static double singleNodeTestAllGreedyAlgorithm();

  static void openResultsFile();
  static void writeLineToFile(string line);
  static void closeResultsFile();

  static void parseArgs(int argc, char **argv);
public:
  static void start(int argc, char **argv);
};

void pretty_print(const vector<int> &v);
string int_array_to_string(int int_array[], int size_of_array);