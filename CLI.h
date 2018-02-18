#include "gtries/GraphMatrix.h"
#include "gtries/GraphUtils.h"
#include "gtries/Error.h"
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
  static int nullcaseWeight(int a, int b);
  static int maskedNullcaseWeight(int a, int b);

  static void readPartition(const char *s);
  static void randomPartition(int maxCummunities);
  static void createAllPartitions();
  static void createAllPartitionsStep(int level, int numNodes);

  static float cicleModularity(int size);
  static void iterateCombinations(int offset, int k);
  static void computeCicleModularity();
  static void combinationCicleModularity();
  static float triangleModularity();

public:
  static void start(int argc, char **argv);
};

void pretty_print(const vector<int> &v);