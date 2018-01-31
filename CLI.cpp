#include "CLI.h"

Graph *CLI::g;
int *CLI::partition;

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
  partition = new int[n];
  for (int i = 0; i < n; i++)
  {
    partition[i] = 0;
  }

  printf("Num nodes: %d", g->numNodes());
  printf("kronecker (1, 2): %d", kronecker(1, 2));
}

int CLI::kronecker(int a, int b)
{
  return partition[a - 1] == partition[b - 1] ? 1 : 0;
}

int CLI::motifModularity()
{
  int n = g->numNodes();

  int numberMotifsInPartitions = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; i < n; j++)
    {
      for (int k = 0; k < n; k++)
      {
        numberMotifsInPartitions += CLI::weightWithKronecker(i, j) * CLI::weightWithKronecker(j, k) * CLI::weightWithKronecker(i, k);
      }
    }
  }

  int numberMotifsGraph = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; i < n; j++)
    {
      for (int k = 0; k < n; k++)
      {
        numberMotifsGraph += CLI::weight(i, j) * CLI::weight(j, k) * CLI::weight(i, k);
      }
    }
  }

  int numberMotifsRandomGraphPartitions = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; i < n; j++)
    {
      for (int k = 0; k < n; k++)
      {
        numberMotifsGraph += CLI::weight(i, j) * CLI::weight(j, k) * CLI::weight(i, k);
      }
    }
  }
}

int CLI::outWeight(int a)
{
  return (g->numOutEdges(a));
}

int CLI::inWeight(int a)
{
  return (g->numInEdges(a));
}

int CLI::weight(int a, int b)
{
  return (int)(g->hasEdge(a, b));
}

int CLI::weightWithKronecker(int a, int b)
{
  return CLI::weight(a, b) * CLI::kronecker(a, b);
}
