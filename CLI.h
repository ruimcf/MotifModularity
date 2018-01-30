#include "GraphMatrix.h"
#include "GraphUtils.h"

class CLI
{
private:
  static Graph *g;
  static int *module;
  static int kronecker(int a, int b);

public:
  static int a;
  static void start(int argc, char **argv);
};