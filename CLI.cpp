#include "CLI.h"

Graph *CLI::g;
int *CLI::module;

void CLI::start(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("Use with more arg");
    return;
  }
  char fileName[MAX_BUF];
  strcpy(fileName, argv[1]);
  printf("Filename %s\n", fileName);
  g = new GraphMatrix();
  bool dir = false;
  bool weight = true;
  GraphUtils::readFileTxt(g, fileName, dir, weight);

  int n = g->numNodes();
  module = new int[n];
  for (int i = 0; i < n; i++)
  {
    module[i] = 0;
  }

  printf("Num nodes: %d", g->numNodes());
  printf("kronecker (1, 2): %d", kronecker(1, 2));
}

int CLI::kronecker(int a, int b)
{
  return module[a - 1] == module[b - 1] ? 1 : 0;
}