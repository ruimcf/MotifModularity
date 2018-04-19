#include "CLI.h"
#include "Random.h"
#include <ctime>

using namespace std;

Graph *CLI::g;
vector<int> CLI::nodes;
vector<int> CLI::combination;
bool CLI::directed, CLI::weighted, CLI::readPartition;
string CLI::networkFile;
ofstream CLI::resultsFile;
string CLI::partitionFile;
int CLI::seed;
ArrayPartition CLI::networkPartition;
int total = 0;
double CLI::n1 = 0, CLI::n2 = 0, CLI::n3 = 0, CLI::n4 = 0;
double bestModularity;
int *bestPartition;

void CLI::parseArgs(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "--directed" || arg == "-d")
        {
            directed = true;
        }
        else if (arg == "--undirected" || arg == "-ud")
        {
            directed = false;
        }
        else if (arg == "--weighted" || arg == "-w")
        {
            weighted = true;
        }
        else if (arg == "--unweighted" || arg == "-uw")
        {
            weighted = false;
        }
        else if (arg == "--partition" || arg == "-p")
        {
            if (++i > argc)
            {
                cout << "partition few args" << endl;
                return;
            }
            readPartition = true;
            partitionFile = argv[i];
        }
        else if (arg == "--network" || arg == "-n")
        {
            if (++i > argc)
            {
                cout << "network few args" << endl;
                return;
            }
            networkFile = argv[i];
        }
        else if (arg == "--seed" || arg == "-s")
        {
            if (++i >= argc)
            {
                cout << "seed few args" << endl;
                return;
            }
            cout << i << " " << argc << endl;
            seed = atoi(argv[i]);
        }
    }
}

void CLI::openResultsFile()
{
    //clear file if it exists
    string resultPath = "results/" + networkFile;
    resultsFile.open(resultPath, ios::trunc);
    if (resultsFile.is_open())
    {
        cout << "Opened file for results " << resultPath << endl;
    }
    else
    {
        cout << "Failed to open File " << resultPath << endl;
    }
}

void CLI::writeLineToFile(string line)
{
    resultsFile << line;
}

void CLI::closeResultsFile()
{
    resultsFile.close();
}

void CLI::start(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Use with more arg");
        return;
    }
    readPartition = false;
    directed = false;
    weighted = false;
    seed = time(NULL);
    CLI::parseArgs(argc, argv);
    CLI::openResultsFile();
    cout << "SEED: " << seed << endl;
    CLI::writeLineToFile("SEED: " + to_string(seed) + "\n");
    Random::seed(seed);
    g = new GraphMatrix();
    if (networkFile.empty())
    {
        cout << "no network given" << endl;
        return;
    }
    GraphUtils::readFileTxt(g, networkFile.c_str(), directed, weighted);
    int n = g->numNodes();
    networkPartition.setNumberNodes(n);

    if (readPartition)
    {
        networkPartition.readPartition(partitionFile.c_str());
        cout << "Partition read: " << networkPartition.toStringPartitionByNode() << endl;
        printf("Num nodes: %d\n", g->numNodes());
        networkPartition.writePartitionFile(networkFile);
        double modularity = CLI::triangleModularity();
        printf("Triangle modularity: %f\nTotal: %d\n", modularity, total);
    }
    else
    {
        networkPartition.randomPartition(2);
    }

    for (int i = 0; i < g->numNodes(); i++)
    {
        nodes.push_back(i);
    }

    clock_t begin = clock();
    CLI::singleNodeGreedyAlgorithm();
    clock_t end = clock();
    double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Elapsed seconds: " << elapsedSecs << endl;
    CLI::closeResultsFile();

    // total = 0;
    // CLI::createAllPartitions();
    //double _motifModularity = CLI::cicleModularity(3);
    //printf("Circle modularity: %f\nTotal: %d\n", _motifModularity, total);
}

double CLI::triangleModularity()
{
    int n = g->numNodes();

    double numberMotifsInPartitions = 0;
    double numberMotifsGraph = 0;
    double numberMotifsRandomGraphPartitions = 0;
    double numberMotifsRandomGraph = 0;
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
            }
        }
    }

    double motifModularity = numberMotifsInPartitions / numberMotifsGraph - numberMotifsRandomGraphPartitions / numberMotifsRandomGraph;

    // cout << motifModularity << "\t" << numberMotifsInPartitions << "\t" << numberMotifsGraph << "\t" << numberMotifsRandomGraphPartitions << "\t" << numberMotifsRandomGraph << endl; 

    return motifModularity;
}

double CLI::triangleModularityPreCalculated(std::vector<double> motifValues)
{
    int n = g->numNodes();

    double numberMotifsInPartitions = 0;
    double numberMotifsRandomGraphPartitions = 0;
    
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if(CLI::kronecker(i,j)) 
            {
                for (int k = j + 1; k < n; k++)
                {
                    if(CLI::kronecker(j,k) && CLI::kronecker(i, k))
                    {
                        numberMotifsInPartitions += g->hasEdge(j, k) * g->hasEdge(i, k) * g->hasEdge(i, j);
                        numberMotifsRandomGraphPartitions += CLI::maskedNullcaseWeight(i, j) * CLI::maskedNullcaseWeight(j, k) * CLI::maskedNullcaseWeight(i, k);
                    }
                }
            }
        }
    }

    
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = i + 1; j < n; j++)
    //     {
    //         if(CLI::kronecker(i,j)) 
    //         {
    //             for (int k = j + 1; k < n; k++)
    //             {
    //                 if(CLI::kronecker(j,k))
    //                 {
    //                     numberMotifsRandomGraphPartitions += CLI::maskedNullcaseWeight(i, j) * CLI::maskedNullcaseWeight(j, k) * CLI::maskedNullcaseWeight(i, k);
    //                 }
    //             }
    //         }
    //     }
    // }

    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = i + 1; j < n; j++)
    //     {
    //         if (g->hasEdge(i, j) && CLI::kronecker(i,j))
    //         {
    //             for (int k = j + 1; k < n; k++)
    //             {
    //                 if (g->hasEdge(j, k) && g->hasEdge(i, k) && CLI::kronecker(i,k) && CLI::kronecker(j,k))
    //                 {
    //                     numberMotifsInPartitions += 1;
    //                 }
    //             }
    //         }
    //     }
    // }
    double motifModularity = numberMotifsInPartitions / motifValues.at(0) - numberMotifsRandomGraphPartitions / motifValues.at(1);

    // cout << motifModularity << "\t" << numberMotifsInPartitions << "\t" << motifValues.at(0) << "\t" << numberMotifsRandomGraphPartitions << "\t" <<  motifValues.at(1) << endl; 

    return motifModularity;
}

vector<double> CLI::constantMotifValues()
{
    vector<double> values;
    int n = g->numNodes();
    double numberMotifsGraph = 0;
    double numberMotifsRandomGraph = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            for (int k = j + 1; k < n; k++)
            {
                numberMotifsGraph += g->hasEdge(i, j) * g->hasEdge(j, k) * g->hasEdge(i, k);
                numberMotifsRandomGraph += CLI::nullcaseWeight(i, j) * CLI::nullcaseWeight(j, k) * CLI::nullcaseWeight(i, k);
            }
        }
    }
    values.push_back(numberMotifsGraph);
    values.push_back(numberMotifsRandomGraph);
    return values;
}

double CLI::cicleModularity(int size)
{
    n1 = n2 = n3 = n4 = 0;
    combination.clear();
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

int CLI::kronecker(int a, int b)
{
    return networkPartition.kronecker(a, b);
}

void CLI::createAllPartitions()
{
    int numNodes = g->numNodes();
    bestModularity = 0;
    bestPartition = new int[numNodes];
    CLI::createAllPartitionsStep(0, numNodes);
}

void CLI::createAllPartitionsStep(int level, int numberNodes)
{
    if (level == numberNodes - 1)
    {
        double modularity = CLI::triangleModularity();
        if (modularity >= bestModularity)
        {
            bestModularity = modularity;
            memcpy(bestPartition, networkPartition.getPartitionByNode().data(), sizeof(int) * numberNodes);
            cout << "New best modularity: " << modularity << endl
                 << "Paritition: " << networkPartition.toStringPartitionByNode() << endl;
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

double CLI::singleNodeGreedyAlgorithm()
{
    int chosenNode, chosenIndex, chosenNodePartition, betterPartition, numPartitions;
    double bestModularity, currentModularity;
    vector<int> allNodes;

    numPartitions = 4;
    // numPartitions = numberForEvenPartitions(g->numNodes());
    networkPartition.randomPartition(numPartitions);

    // currentModularity = CLI::cicleModularity(3);
    // cout << "Current Modularity 1 " << currentModularity << endl;
    // currentModularity = CLI::cicleModularity(3);
    // cout << "Current Modularity 2 " << currentModularity << endl;

    std::vector<double> motifValues = CLI::constantMotifValues();
    currentModularity = CLI::triangleModularityPreCalculated(motifValues);
    currentModularity = CLI::triangleModularity();
    
    cout << "Current Modularity " << currentModularity << endl;

    FailObject failObject;
    allNodes.reserve(g->numNodes());
    for (int i = 0; i < g->numNodes(); ++i)
    {
        allNodes.push_back(i);
    }
    vector<int> availableNodes(allNodes);

    while (!failObject.finished() && !availableNodes.empty())
    {
        chosenIndex = Random::getInteger(0, availableNodes.size() - 1);
        chosenNode = availableNodes[chosenIndex];

        chosenNodePartition = networkPartition.getNodeCommunity(chosenNode);
        betterPartition = -1;
        for (int i = 0; i < numPartitions; i++)
        {
            if (i != chosenNodePartition)
            {
                networkPartition.setNodeCommunity(chosenNode, i);
                // double currentPartitionModularity = CLI::triangleModularity();
                double currentPartitionModularity = CLI::triangleModularityPreCalculated(motifValues);
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
            availableNodes.erase(availableNodes.begin() + chosenIndex);
        }
        else
        {
            networkPartition.setNodeCommunity(chosenNode, betterPartition);

            stringstream ss;
            ss << "Current modularity: " << currentModularity << endl;
            ss << "Times failed: " << failObject.getConsecutiveTimesFailed() << endl;
            ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
            cout << ss.str();
            writeLineToFile(ss.str());

            failObject.recordSuccess();
            availableNodes = allNodes;
        }
    }

    stringstream ss;
    ss << "Best modularity: " << currentModularity << endl;
    ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
    cout << ss.str();
    writeLineToFile(ss.str());

    return currentModularity;
}
