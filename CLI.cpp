#include "CLI.h"

using namespace std;

Graph *CLI::g;
int *CLI::partition;
vector<int> CLI::nodes;
vector<int> CLI::combination;
int total = 0;
float CLI::n1 = 0, CLI::n2 = 0, CLI::n3 = 0, CLI::n4 = 0;

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
  CLI::readPartition(partitionFile);
  //CLI::randomPartition(2);

  printf("Num nodes: %d\n", g->numNodes());
  CLI::motifModularity();
  total = 0;
  //CLI::createAllPartitions();
  for (int i = 0; i < g->numNodes(); i++)
  {
    nodes.push_back(i);
  }
  float _motifModularity = CLI::cicleModularity(3);
  printf("Motif modularity: %f\n", _motifModularity);
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
    for (int j = i + 1; j < n; j++)
    {
      for (int k = j + 1; k < n; k++)
      {
        total++;
        numberMotifsInPartitions += CLI::maskedWeight(i, j) * CLI::maskedWeight(j, k) * CLI::maskedWeight(i, k);
        numberMotifsGraph += CLI::weight(i, j) * CLI::weight(j, k) * CLI::weight(i, k);
        numberMotifsRandomGraphPartitions += CLI::maskedNullcaseWeight(i, j) * CLI::maskedNullcaseWeight(j, k) * CLI::maskedNullcaseWeight(i, k);
        numberMotifsRandomGraph += CLI::nullcaseWeight(i, j) * CLI::nullcaseWeight(j, k) * CLI::nullcaseWeight(i, k);
      }
    }
  }

  float _motifModularity = numberMotifsInPartitions / numberMotifsGraph - numberMotifsRandomGraphPartitions / numberMotifsRandomGraph;
  printf("Motif modularity: %f\n", _motifModularity);

  return _motifModularity;
}

float CLI::cicleModularity(int size)
{
  CLI::combinationRecursive(0, size);
  // cout << "Final values" << endl
  //      << "n1: " << n1 << " n2: " << n2 << " n3: " << n3 << " n4: " << n4 << endl;
  return n1 / n2 - n3 / n4;
}

void CLI::combinationRecursive(int offset, int k)
{
  if (k == 0)
  {
    //I have the combination
    //pretty_print(combination);
    CLI::computeCombinationCicleModularity();
    return;
  }
  for (int i = offset; i < g->numNodes(); i++)
  {
    combination.push_back(nodes[i]);
    CLI::combinationRecursive(i + 1, k - 1);
    combination.pop_back();
  }
}

void CLI::computeCombinationCicleModularity()
{
  int g1 = CLI::maskedWeight(combination.front(), combination.back());
  int g2 = CLI::weight(combination.front(), combination.back());
  int g3 = CLI::maskedNullcaseWeight(combination.front(), combination.back());
  int g4 = CLI::nullcaseWeight(combination.front(), combination.back());

  for (int i = 0; i < combination.size() - 1; i++)
  {
    g1 *= CLI::maskedWeight(combination[i], combination[i + 1]);
    g2 *= CLI::weight(combination[i], combination[i + 1]);
    g3 *= CLI::maskedNullcaseWeight(combination[i], combination[i + 1]);
    g4 *= CLI::nullcaseWeight(combination[i], combination[i + 1]);
  }

  n1 += g1;
  n2 += g2;
  n3 += g3;
  n4 += g4;
}

void pretty_print(const vector<int> &v)
{
  static int count = 0;
  cout << "combination no " << (++count) << ": [ ";
  for (int i = 0; i < v.size(); ++i)
  {
    cout << v[i] << " ";
  }
  cout << "] " << endl;
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

void CLI::createAllPartitions()
{
  int numNodes = g->numNodes();
  CLI::createAllPartitionsStep(0, numNodes);
}

void CLI::createAllPartitionsStep(int level, int numberNodes)
{
  if (level == numberNodes - 1)
  {
    CLI::motifModularity();
    return;
  }
  partition[level] = 0;
  CLI::createAllPartitionsStep(level + 1, numberNodes);
  partition[level] = 1;
  CLI::createAllPartitionsStep(level + 1, numberNodes);
}