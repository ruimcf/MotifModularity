#include "CLI.h"
#include "Error.h"

Graph *CLI::g;
int *CLI::partition;

void CLI::start(int argc, char **argv)
{
  if (argc < 3)
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
  char partitionFile[MAX_BUF];
  strcpy(partitionFile, argv[2]);
  //CLI::readPartition(partitionFile);
  CLI::randomPartition(2);

  printf("Num nodes: %d\n", g->numNodes());
  printf("kronecker (1, 2): %d\n", kronecker(1, 2));
  CLI::motifModularity();
}

int CLI::kronecker(int a, int b)
{
  return partition[a - 1] == partition[b - 1] ? 1 : 0;
}

float CLI::motifModularity()
{
  int n = g->numNodes();

  float numberMotifsInPartitions = 0;
  float numberMotifsGraph = 0;
  float numberMotifsRandomGraphPartitions = 0;
  float numberMotifsRandomGraph = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      for (int k = 0; k < n; k++)
      {
        numberMotifsInPartitions += CLI::maskedWeight(i, j) * CLI::maskedWeight(j, k) * CLI::maskedWeight(i, k);
        numberMotifsGraph += CLI::weight(i, j) * CLI::weight(j, k) * CLI::weight(i, k);
        numberMotifsRandomGraphPartitions += CLI::maskedNullcaseWeight(i, j) * CLI::maskedNullcaseWeight(j, k) * CLI::maskedNullcaseWeight(i, k);
        numberMotifsRandomGraph += CLI::nullcaseWeight(i, j) * CLI::nullcaseWeight(j, k) * CLI::nullcaseWeight(i, k);
      }
    }
  }

  float _motifModularity = numberMotifsInPartitions / numberMotifsGraph - numberMotifsRandomGraphPartitions / numberMotifsRandomGraph;
  printf("Motif modularity: %f", _motifModularity);

  return _motifModularity;
}

int CLI::outWeight(int a)
{
  return (g->nodeOutEdges(a));
}

int CLI::inWeight(int a)
{
  return (g->nodeInEdges(a));
}

int CLI::nullcaseWeight(int a, int b)
{
  return CLI::outWeight(a) * CLI::inWeight(b);
}

int CLI::maskedNullcaseWeight(int a, int b)
{
  return CLI::nullcaseWeight(a, b) * CLI::kronecker(a, b);
}

int CLI::weight(int a, int b)
{
  return (int)(g->hasEdge(a, b));
}

int CLI::maskedWeight(int a, int b)
{
  return CLI::weight(a, b) * CLI::kronecker(a, b);
}

void CLI::readPartition(const char *s)
{
  FILE *f = fopen(s, "r");
  if (!f)
    Error::msg(NULL);

  int a, size = 0;

  while (fscanf(f, "%d", &a) == 1)
  {
    partition[size] = a;
    size++;
  }
  fclose(f);
}

void CLI::randomPartition(int maxCommunities)
{
  std::vector<int> v;
  srand(time(NULL));
  int n = g->numNodes();
  int numberCommunities = 0;
  for (int i = 0; i < n; i++)
  {
    int x = rand() % maxCommunities;
    if (std::find(v.begin(), v.end(), x) != v.end())
    {
      /* v contains x */
    }
    else
    {
      numberCommunities++;
      v.push_back(x);
    }
    partition[i] = x;
  }
  printf("Number of communities: %d\n", numberCommunities);
}