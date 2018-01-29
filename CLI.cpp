#include "CLI.h"

Graph *CLI::g;

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
  printf("Num nodes: %d", g->numNodes());
}