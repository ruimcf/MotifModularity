#include "GraphMatrix.h"
#include "GraphUtils.h"

class CLI
{
private:
  static Graph *g;
  static int *partition;
  static int kronecker(int a, int b);
  static int weightWithKronecker(int a, int b);
  static int motifModularity();
  static int weight(int a, int b);

public:
  static int a;
  static void start(int argc, char **argv);
};