#include "GraphMatrix.h"
#include "GraphUtils.h"

class CLI
{
private:
  static Graph *g;
  static int *partition;
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

  static float motifModularity();
  static int weight(int a, int b);

public:
  static int a;
  static void start(int argc, char **argv);
};