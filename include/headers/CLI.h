#include "GraphMatrix.h"
#include "GraphUtils.h"
#include "Error.h"
#include <string>
#include <math.h>
#include "FailObject.h"
#include "ArrayPartition.h"

class CLI
{
private:
  static Graph *g;
  // static int *partition;
  static ArrayPartition networkPartition;
  static vector<int> nodes;
  static vector<int> combination;
  static float n1;
  static float n2;
  static float n3;
  static float n4;
  static int kronecker(int a, int b);
  static int maskedWeight(int a, int b);
  static int nullcaseWeight(int a, int b);
  static int maskedNullcaseWeight(int a, int b);

  static void createAllPartitions();
  static void createAllPartitionsStep(int level, int numNodes);

  static float cicleModularity(int size);
  static void iterateCombinations(int offset, int k);
  static void computeCicleModularity();
  static void combinationCicleModularity();
  static float triangleModularity();

  static float singleNodeGreedyAlgorithm();

public:
  static void start(int argc, char **argv);
};

void pretty_print(const vector<int> &v);
string int_array_to_string(int int_array[], int size_of_array);