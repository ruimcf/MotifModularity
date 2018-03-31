#include "CLI.h"

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
float CLI::n1 = 0, CLI::n2 = 0, CLI::n3 = 0, CLI::n4 = 0;
float bestModularity;
int *bestPartition;

void CLI::parseArgs(int argc, char **argv){
    for(int i = 1; i < argc; ++i){
        string arg = argv[i];
        if(arg == "--directed" || arg == "-d"){
            directed = true;
        }
        else if(arg == "--undirected" || arg == "-ud"){
            directed = false;
        }
        else if(arg == "--weighted" || arg == "-w"){
            weighted = true;
        }
        else if(arg == "--unweighted" || arg == "-uw"){
            weighted = false;
        }
        else if(arg == "--partition" || arg == "-p"){
            if(++i > argc){
                cout << "partition few args" << endl;
                return;
            }
            readPartition = true;
            partitionFile = argv[i];
        }
        else if(arg == "--network" || arg == "-n"){
            if(++i > argc){
                cout << "network few args" << endl;
                return;
            }
            networkFile = argv[i];
        }
        else if(arg == "--seed" || arg == "-s"){
            if(++i >= argc){
                cout << "seed few args" << endl;
                return;
            }
            cout << i << " " << argc << endl;
            seed = atoi(argv[i]);
        }
    }
}

void CLI::openResultsFile(){
    //clear file if it exists
    string resultPath = "results/" + networkFile;
    resultsFile.open(resultPath, ios::trunc);
    if(resultsFile.is_open()){
        cout << "Opened file for results " << resultPath << endl;
    } 
    else {
        cout << "Failed to open File " << resultPath << endl;
    }
}

void CLI::writeLineToFile(string line){
    resultsFile << line;
}

void CLI::closeResultsFile(){
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
    cout << "SEED: " << seed << endl;
    g = new GraphMatrix();
    if(networkFile.empty()){
        cout << "no network given" << endl;
        return;
    }
    GraphUtils::readFileTxt(g, networkFile.c_str(), directed, weighted);
    int n = g->numNodes();
    networkPartition.setNumberNodes(n);

    if(readPartition){
        networkPartition.readPartition(partitionFile.c_str());
        cout << "Partition read: " << int_array_to_string(networkPartition.getPartition(), n) << endl;
        printf("Num nodes: %d\n", g->numNodes());
        float modularity = CLI::triangleModularity();
        printf("Triangle modularity: %f\nTotal: %d\n", modularity, total);
    }
    else {
        networkPartition.randomPartition(2);
    }

    CLI::openResultsFile();

    CLI::singleNodeGreedyAlgorithm();

    CLI::closeResultsFile();

    // total = 0;
    // CLI::createAllPartitions();
    // for (int i = 0; i < g->numNodes(); i++)
    // {
    //     nodes.push_back(i);
    // }
    //float _motifModularity = CLI::cicleModularity(3);
    //printf("Circle modularity: %f\nTotal: %d\n", _motifModularity, total);
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
    numPartitions = numberForEvenPartitions(g->numNodes());
    networkPartition.randomPartition(numPartitions);
    int chosenNode, chosenIndex;
    float currentModularity = CLI::triangleModularity();
    int chosenNodePartition;
    float bestModularity;
    int betterPartition;
    bool running = true;
    FailObject failObject;
    srand(seed);
    vector<int> allNodes;
    allNodes.reserve(g->numNodes());
    for (int i = 0; i < g->numNodes(); ++i)
    {
        allNodes.push_back(i);
    }
    vector<int> availableNodes(allNodes);
    while (!failObject.finished() && !availableNodes.empty())
    {
        chosenIndex = rand() % availableNodes.size();
        chosenNode = availableNodes[chosenIndex];

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
            availableNodes.erase(availableNodes.begin() + chosenIndex);
        }
        else
        {
            stringstream ss1, ss2;
            ss1 << "Current modularity: " << currentModularity << "\tTimes failed: " << failObject.getConsecutiveTimesFailed() << endl;
            ss2 << "Partition: " << int_array_to_string(networkPartition.getPartition(), g->numNodes()) << endl;
            cout << ss1.str();
            cout << ss2.str();
            writeLineToFile(ss1.str());
            writeLineToFile(ss2.str());

            failObject.recordSuccess();
            networkPartition.setNodeCommunity(chosenNode, betterPartition);
            availableNodes = allNodes;
        }
    }

    stringstream ss1, ss2;
    ss1 << "Best modularity: " << currentModularity << endl;
    ss2 << "Partition: " << int_array_to_string(networkPartition.getPartition(), g->numNodes()) << endl;
    cout << ss1.str();
    cout << ss2.str();
    writeLineToFile(ss1.str());
    writeLineToFile(ss2.str());

    return currentModularity;
}
