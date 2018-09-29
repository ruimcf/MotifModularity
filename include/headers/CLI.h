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
  long long numberMotifsInGraph;
  long long degreeMotifsRandomGraph;
};

struct MotifVariableValues {
  long long numberMotifsInCommunities;
  long long degreeMotifsInCommunities;
};

struct MotifValues {
  long long numberMotifsInCommunities;
  long long numberMotifsInGraph;
  long long degreeMotifsInCommunities; 
  long long degreeMotifsRandomGraph;  
};

class CLI
{
private:
  static ofstream resultsFile;
  static Graph *g;
  static ArrayPartition networkPartition;
  static vector<int> nodes;
  static vector<int> combination;
  static vector<long> combinationWeightsArray;
  static vector<bool> used;
  static bool directed;
  static bool weighted;
  static string partitionFile;
  static string networkFile;
  static string networkFileName;
  static string motifFile;
  static bool readMotif;
  static bool readPartition;
  static bool notOptimized;
  static int seed;
  static Motif motif;
  static void setNodes();
  static int parseArgs(int argc, char **argv);
  static int numberOfCommunities;
  static bool hasNumberOfCommunities;

  /**
   * Classic triangleModularity
   */
  static double triangleModularity();

  /**
   * Cicle Modularity for some size
   */
  static double cicleModularity(int size);
  static void cicleModularityIteration(int offset, int k, MotifValues *values);
  static void combinationCicleModularity(MotifValues *values);

  /**
   * Motif Modularity
   * only computes when some combination is complete
   */
  static double motifModularity();
  static void nodeCombination(int offset, MotifValues *values);
  static void countCombinationMotifs(MotifValues *values);
  static bool combinationObeysOrbitRules();
  static bool combinationHasMotifEdges();
  static bool combinationHasMotifCommunities();
  static int combinationNullcaseWeights();

  /**
   * Optimized Motif Modularity
   * checks the combination while it is being constructed
   * to prune cases where it would never evaluate earlier
   */
  static double optimizedMotifModularity();
  static MotifValues optimizedMotifModularityValues();
  static void optimizedMotifModularityValuesIteration(int offset, bool edgesCheck, bool communitiesCheck, MotifValues *values);
  static bool optimizedCombinationHasMotifEdges();
  static bool optimizedCombinationHasMotifCommunities();
  static bool optimizedCombinationOrbitRules();
  static int getOrbitRulesNextValidNode();
  static void nextCombinationNullcaseWeights();

  /**
   * For the same graph, different partitions have some constant values
   * that can be re-used, so that we only need to compute the variable values
   */
  static MotifConstantValues getMotifConstantValues();
  static void getMotifConstantValuesIteration(int offset, MotifConstantValues *ptr);
  static MotifVariableValues getMotifVariableValues();
  static void getMotifVariableValuesIteration(int offset, bool edgesCheck, MotifVariableValues *values);

  /**
   *  The same as above, but we know that only one of the nodes changed
   */
  static MotifVariableValues nodeVariableValues(int changingNode);
  static void nodeVariableValuesIteration(int changingNode, int position, int offset, MotifVariableValues *changingNodeValues);
  static MotifValues changingNodeMotifValues(MotifValues allPreviousValues, MotifVariableValues beforeChangeValues, MotifVariableValues afterChangeValues);

  /**
   * Greedy algorithms to maximize modularity
   */
  static double singleNodeGreedyAlgorithm();
  static double singleNodeGreedyAlgorithmNotOptimized();
  static double singleNodeTestAllGreedyAlgorithm();

  /**
   * Helper functions to compute motif modularity from values
   */
  static double motifModularityFromValues(MotifValues values);
  static double motifModularityFromValues(MotifConstantValues cValues, MotifVariableValues vValues);
  static double motifModularityFromValues(long long v1, long long v2, long long v3, long long v4);

  /**
   * Basic helper operation functions
   */
  static int kronecker(int a, int b);
  static int maskedWeight(int a, int b);
  static int nullcaseWeight(int a, int b);
  static int maskedNullcaseWeight(int a, int b);

  /**
   * Results Helpers
   */
  static void openResultsFile();
  static void writeLineToFile(string line);
  static void closeResultsFile();
  static void writeNetworkToGephiData();
  static void registerConfigs();

  /**
   * Create all partitions for a graph
   * and compute the modularity for each *impratical*
   */
  static void createAllPartitions();
  static void createAllPartitionsStep(int level, int numNodes);

public:
  static void start(int argc, char **argv);
};
