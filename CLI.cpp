#include "CLI.h"

using namespace std;

Graph *CLI::g;
vector<int> CLI::nodes;
vector<int> CLI::combination;
ArrayPartition CLI::networkPartition;
int total = 0;
float CLI::n1 = 0, CLI::n2 = 0, CLI::n3 = 0, CLI::n4 = 0;
float bestModularity;
int *bestPartition;

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
    networkPartition.setNumberNodes(n);

    char partitionFile[MAX_BUF];
    strcpy(partitionFile, argv[2]);
    networkPartition.readPartition(partitionFile);
    cout << "Parition read: " << int_array_to_string(networkPartition.getPartition(), n) << endl;
    //CLI::randomPartition(2);

    printf("Num nodes: %d\n", g->numNodes());
    float _triangleModularity = CLI::triangleModularity();
    printf("Triangle modularity: %f\nTotal: %d\n", _triangleModularity, total);

    // total = 0;
    // CLI::createAllPartitions();
    // for (int i = 0; i < g->numNodes(); i++)
    // {
    //     nodes.push_back(i);
    // }
    //float _motifModularity = CLI::cicleModularity(3);
    //printf("Circle modularity: %f\nTotal: %d\n", _motifModularity, total);

    CLI::singleNodeGreedyAlgorithm();
}

int CLI::kronecker(int a, int b)
{
    return networkPartition.kronecker(a, b);
}

float CLI::triangleModularity()
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
                numberMotifsGraph += g->hasEdge(i, j) * g->hasEdge(j, k) * g->hasEdge(i, k);
                numberMotifsRandomGraphPartitions += CLI::maskedNullcaseWeight(i, j) * CLI::maskedNullcaseWeight(j, k) * CLI::maskedNullcaseWeight(i, k);
                numberMotifsRandomGraph += CLI::nullcaseWeight(i, j) * CLI::nullcaseWeight(j, k) * CLI::nullcaseWeight(i, k);
                //cout << "n1: " << numberMotifsInPartitions << " n2: " << numberMotifsGraph << " n3: " << numberMotifsRandomGraphPartitions << " n4: " << numberMotifsRandomGraph << endl;
            }
        }
    }

    float _motifModularity = numberMotifsInPartitions / numberMotifsGraph - numberMotifsRandomGraphPartitions / numberMotifsRandomGraph;
    //printf("Motif modularity: %f\n", _motifModularity);

    return _motifModularity;
}

float CLI::cicleModularity(int size)
{
    CLI::iterateCombinations(0, size);
    return n1 / n2 - n3 / n4;
}

void CLI::iterateCombinations(int offset, int k)
{
    if (k == 0)
    {
        CLI::combinationCicleModularity();
        return;
    }
    for (int i = offset; i < g->numNodes(); i++)
    {
        combination.push_back(nodes[i]);
        CLI::iterateCombinations(i + 1, k - 1);
        combination.pop_back();
    }
}

void CLI::combinationCicleModularity()
{
    int g1 = CLI::maskedWeight(combination.back(), combination.front());
    int g2 = g->hasEdge(combination.back(), combination.front());
    int g3 = CLI::maskedNullcaseWeight(combination.back(), combination.front());
    int g4 = CLI::nullcaseWeight(combination.back(), combination.front());

    for (int i = 0; i < combination.size() - 1; i++)
    {
        g1 *= CLI::maskedWeight(combination[i], combination[i + 1]);
        g2 *= g->hasEdge(combination[i], combination[i + 1]);
        g3 *= CLI::maskedNullcaseWeight(combination[i], combination[i + 1]);
        g4 *= CLI::nullcaseWeight(combination[i], combination[i + 1]);
    }

    total++;
    n1 += g1;
    n2 += g2;
    n3 += g3;
    n4 += g4;

    //cout << "n1: " << n1 << " n2: " << n2 << " n3: " << n3 << " n4: " << n4 << endl;
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

int CLI::nullcaseWeight(int a, int b)
{
    return g->nodeOutEdges(a) * g->nodeInEdges(b);
}

int CLI::maskedNullcaseWeight(int a, int b)
{
    return CLI::nullcaseWeight(a, b) * CLI::kronecker(a, b);
}

int CLI::maskedWeight(int a, int b)
{
    return g->hasEdge(a, b) * CLI::kronecker(a, b);
}

void CLI::createAllPartitions()
{
    int numNodes = g->numNodes();
    bestModularity = 0;
    bestPartition = new int[numNodes];
    CLI::createAllPartitionsStep(0, numNodes);
}

string int_array_to_string(int int_array[], int size_of_array)
{
    string returnstring = "";
    for (int temp = 0; temp < size_of_array; temp++)
        returnstring += to_string(int_array[temp]);
    return returnstring;
}

void CLI::createAllPartitionsStep(int level, int numberNodes)
{
    if (level == numberNodes - 1)
    {
        float modularity = CLI::triangleModularity();
        if (modularity >= bestModularity)
        {
            bestModularity = modularity;
            memcpy(bestPartition, networkPartition.getPartition(), sizeof(int) * numberNodes);
            cout << "New best modularity: " << modularity << endl
                 << "Paritition: " << int_array_to_string(networkPartition.getPartition(), numberNodes) << endl;
        }
        return;
    }
    int maxCommunities = numberNodes;
    for (int i = 0; i < maxCommunities; i++)
    {
        networkPartition.setNodeCommunity(level, i);
        CLI::createAllPartitionsStep(level + 1, numberNodes);
    }
}

/**
 * Possíveis approaches para o greedy algorithm
 * 
 * Como fazer a partição inicial?
 *  1. Tudo separado
 *  2. Fazer uma partição com grupos de três no máximo, mas maximizando
 *     o numero de grupos de tamanho 3
 *  3. Fazer a random partition com max communities numNodes / 3 seria
 *     uniforme se as partições tivessem o mesmo nº de nós
 */

int numberForEvenPartitions(int numNodes)
{
    return (int)ceil(numNodes / 3.0);
}

float CLI::singleNodeGreedyAlgorithm()
{
    int numPartitions = 2;
    networkPartition.randomPartition(numPartitions);
    int chosenNode;
    float currentModularity = CLI::triangleModularity();
    int chosenNodePartition;
    float bestModularity;
    int betterPartition;
    bool running = true;
    FailObject failObject;
    srand(time(NULL));
    while (!failObject.finished())
    {
        chosenNode = rand() % g->numNodes();
        chosenNodePartition = networkPartition.getNodeCommunity(chosenNode);
        betterPartition = -1;
        for (int i = 0; i < numPartitions; i++)
        {
            if (i != chosenNodePartition)
            {
                networkPartition.setNodeCommunity(chosenNode, i);
                float currentPartitionModularity = triangleModularity();
                if (currentPartitionModularity > currentModularity)
                {
                    currentModularity = currentPartitionModularity;
                    betterPartition = i;
                }
            }
        }
        if (betterPartition == -1)
        {
            failObject.recordFail();
            networkPartition.setNodeCommunity(chosenNode, chosenNodePartition);
        }
        else
        {
            cout << currentModularity << " " << failObject.getConsecutiveTimesFailed() << endl;
            failObject.recordSuccess();
            networkPartition.setNodeCommunity(chosenNode, betterPartition);
        }
    }

    cout << "Best modularity: " << currentModularity << endl
         << "Partition: " << int_array_to_string(networkPartition.getPartition(), g->numNodes()) << endl;

    return currentModularity;
}
