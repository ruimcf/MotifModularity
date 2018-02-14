#include "GraphMatrix.h"
#include "GraphUtils.h"
#include <string>

class CLI
{
private:
  static Graph *g;
  static int *partition;
  static std::vector<float> bestModularity;
  static std::vector<int *> bestPartitions;
  static vector<int> nodes;
  static vector<int> combination;
  static float n1;
  static float n2;
  static float n3;
  static float n4;
  static int kronecker(int a, int b);
  static int maskedWeight(int a, int b);
  static int inWeight(int a);
  static int outWeight(int a);
  static int nullcaseWeight(int a, int b);
  static int maskedNullcaseWeight(int a, int b);
  static void readPartition(const char *s);
  static void randomPartition(int maxCummunities);
  static void createAllPartitions();
  static void createAllPartitionsStep(int level, int numNodes);
  static float cicleModularity(int size);
  static void combinationRecursive(int offset, int k);
  static void computeCicleModularity();
  static void computeCombinationCicleModularity();

  static float motifModularity();
  static int weight(int a, int b);

public:
  static int a;
  static void start(int argc, char **argv);
};

void pretty_print(const vector<int> &v);