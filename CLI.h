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

  static float motifModularity();
  static int weight(int a, int b);

public:
  static int a;
  static void start(int argc, char **argv);
};