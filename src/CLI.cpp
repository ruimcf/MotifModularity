#include "CLI.h"
#include "Random.h"
#include <ctime>

using namespace std;

int __number = 0;
Graph *CLI::g;
vector<int> CLI::nodes;
vector<int> CLI::combination;
bool CLI::directed, CLI::weighted, CLI::readPartition, CLI::readMotif;
string CLI::networkFile;
ofstream CLI::resultsFile;
string CLI::partitionFile;
string CLI::motifFile;
int CLI::seed;
ArrayPartition CLI::networkPartition;
Motif CLI::motif;
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

        else if (arg == "--motif" || arg == "-m")
        {
            if (++i > argc)
            {
                cout << "motif few args" << endl;
                return;
            }
            readMotif = true;
            motifFile = argv[i];
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
    readMotif = false;
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

    if (readMotif){
        motif.readFromFile(motifFile);
        motif.print();
    }

        getchar();

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
    cout << "#" << __number++ << "Normal Triangularity " << " Modularity:" << motifModularity << "\t" << numberMotifsInPartitions << "\t" << numberMotifsGraph << "\t" << numberMotifsRandomGraphPartitions << "\t" << numberMotifsRandomGraph << endl; 

    return motifModularity;
}

void CLI::setNodes()
{
    nodes.clear();
    nodes.reserve(g->numNodes());
    for (int i = 0; i < g->numNodes(); i++)
    {
        nodes.push_back(i);
    }
}

vector<double> CLI::firstIterationTriangleModularity()
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
    vector<double> values;
    values.push_back(numberMotifsInPartitions);
    values.push_back(numberMotifsGraph); 
    values.push_back(numberMotifsRandomGraphPartitions);
    values.push_back(numberMotifsRandomGraph);
    values.push_back(motifModularity);

    cout << "First Iteration Modularity" << motifModularity << "\t" << numberMotifsInPartitions << "\t" << numberMotifsGraph << "\t" << numberMotifsRandomGraphPartitions << "\t" << numberMotifsRandomGraph << endl; 
    return values;
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

/**
 * Calculate the modularity when one node has changed from a previous computation
 * previousValues content:
 * 0 - previousNumberMotifsInPartitions
 * 1 - numberMotifsGraph (constant)
 * 2 - previousNumberMotifsRandomGraphPartitions
 * 3 - numberMotifsRandomGraph (constant)
 * 
 * return content:
 * 0 - numberMotifsInPartitions
 * 1 - numberMotifsGraph
 * 2 - numberMotifsRandomGraphPartitions
 * 3 - numberMotifsRandomGraph
 * 4 - triangleModularity
 */
  
vector<double> CLI::changingNodeTriangleModularity(vector<double> previousValues, int changedNode, int previousCommunity)
{
    double previousNumberMotifsInPartitions = previousValues[0];
    double numberMotifsGraph = previousValues[1];
    double previousNumberMotifsRandomGraphPartitions = previousValues[2];
    double numberMotifsRandomGraph = previousValues[3];
    double newMotifs = 0;
    double previousMotifs = 0;
    double newRandomMotifs = 0;
    double previousRandomMotifs = 0;
    double numberMotifsInPartitions, numberMotifsRandomGraphPartitions, modularity;
    int newCommunity = networkPartition.getNodeCommunity(changedNode);
    cout << "pnmp2 " << previousNumberMotifsInPartitions << endl;

    vector<int> neighbours;
    for(int i=0; i < g->numNodes(); i++)
    {
        if(g->hasEdge(changedNode, i))
        {
            if(changedNode == i){
                cout << "ERROR: THIS SHOULDNT HAVE A CONNECTION TO THE SAME NODE" << endl;
            }
            neighbours.push_back(i);
        }
    }

    vector<int> neighboursInPreviousCommunity;
    vector<int> neighboursInNewCommunity;
    for(int i = 0; i < neighbours.size(); i++)
    {
        if(networkPartition.getNodeCommunity(neighbours[i]) == previousCommunity)
        {
            neighboursInPreviousCommunity.push_back(neighbours[i]);
        }
        else if(networkPartition.getNodeCommunity(neighbours[i]) == newCommunity)
        {
            neighboursInPreviousCommunity.push_back(neighbours[i]);
        }
    }

    for(int i = 0; i < neighboursInPreviousCommunity.size(); i++)
    {
        for(int j = i + 1; j < neighboursInPreviousCommunity.size(); j++)
        {
            int nodeA = neighboursInPreviousCommunity[i];
            int nodeB = neighboursInPreviousCommunity[j];
            if(g->hasEdge(nodeA, nodeB))
            {
                previousMotifs += 1;
            }
            previousRandomMotifs += CLI::nullcaseWeight(nodeA, nodeB) * CLI::nullcaseWeight(nodeB, changedNode) * CLI::nullcaseWeight(nodeA, changedNode);
        }
    }

    for(int i = 0; i < neighboursInNewCommunity.size(); i++)
    {
        for(int j = i + 1; j < neighboursInNewCommunity.size(); j++)
        {
            int nodeA = neighboursInNewCommunity[i];
            int nodeB = neighboursInNewCommunity[j];
            if(g->hasEdge(nodeA, nodeB))
            {
                newMotifs += 1;
            }
            newRandomMotifs += CLI::nullcaseWeight(nodeA, nodeB) * CLI::nullcaseWeight(nodeB, changedNode) * CLI::nullcaseWeight(nodeA, changedNode);
        }
    }

    numberMotifsInPartitions = previousNumberMotifsInPartitions + newMotifs - previousMotifs;
    numberMotifsRandomGraphPartitions = previousNumberMotifsRandomGraphPartitions + newRandomMotifs - previousRandomMotifs;
    modularity = numberMotifsInPartitions / numberMotifsGraph - numberMotifsRandomGraphPartitions / numberMotifsRandomGraph;
    cout << "numberMotifs in partitions: " << previousNumberMotifsInPartitions << " + " << newMotifs << " - " << previousMotifs << " = " << numberMotifsInPartitions << endl;
    cout << "numberMotifs RandomGraph Partitions: " << previousNumberMotifsRandomGraphPartitions << " + " << newRandomMotifs << " - " << previousRandomMotifs << " = " << numberMotifsRandomGraphPartitions << endl;

    cout << "#" << __number++ << "ChangedNode Modularity:" << modularity << "\t" << numberMotifsInPartitions << "\t" << numberMotifsGraph << "\t" << numberMotifsRandomGraphPartitions << "\t" << numberMotifsRandomGraph << endl; 
    std::vector<double> values;
    values.push_back(numberMotifsInPartitions);
    values.push_back(numberMotifsGraph); 
    values.push_back(numberMotifsRandomGraphPartitions);
    values.push_back(numberMotifsRandomGraph);
    values.push_back(modularity);
    return values;
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

 double CLI::motifModularity()
{
    n1 = n2 = n3 = n4 = 0;
    combination.clear();
    CLI::setNodes();
    CLI::nodeCombination(0, motif.getSize());
    return n1 / n2 - n3 / n4;
}

void CLI::nodeCombination(int offset, int left)
{
    if(left == 0)
    {
        CLI::countCombinationMotifs();
        return;
    }
    for (int i = offset; i < g->numNodes(); i++)
    {
        combination.push_back(nodes[i]);
        CLI::iterateCombinations(i + 1, left - 1);
        combination.pop_back();
    } 
}

void CLI::countCombinationMotifs()
{
    //I need to check different permutations of this set of nodes
    //But for now I will use only the given one

    bool motifEdgesCheck = CLI::combinationHasMotifEdges();

    if (motifEdgesCheck)
    {
        int combinationWeights = CLI::combinationNullcaseWeights();
        bool motifCommunitiesCheck = CLI::combinationHasMotifCommunities();
        if(motifCommunitiesCheck) 
        {
            //This partition contains a motif
            n1 += 1;
            n3 += combinationWeights;
        }
        //Total graph contains a motif
        n2 += 1;
        n4 += combinationWeights;
    }

}

// Check if the combination edges are according the motif
// i.e. check if the combination is an occurence of the motif
bool CLI::combinationHasMotifEdges()
{
    vector< vector<int> > adjacencyList = motif.getAdjacencyList();
    for(int i = 0; i < adjacencyList.size(); i++)
    {
        if(!g->hasEdge(combination[adjacencyList[i][0]], combination[adjacencyList[i][1]])){
            return false;
        }
    } 
    return true;
}

// Check if the combination communities are in accordance with the motif communities
bool CLI::combinationHasMotifCommunities()
{
    vector<int> communities = motif.getCommunities();
    for(int i = 0; i < communities.size(); i++)
    {
        for(int j = i + 1; j < communities.size(); j++)
        {
            // If one of the nodes can be in any community we continue
            if(communities[i] == -1 || communities[j] == -1){
                continue;
            }
            // If the communities are different in the motif, they have
            // to be different in the partition
            else if (communities[i] != communities[j]){
                if(CLI::kronecker(combination[i], combination[j]))
                {
                    return false;
                }
            }
            // If the communities are the same on the motif for the pair,
            // they have to be the same on the partition
            else {
                if(!CLI::kronecker(combination[i], combination[j]))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

// For each motif edges, calculate the nullcaseWeight and return the product of them
int CLI::combinationNullcaseWeights()
{
    vector< vector<int> > adjacencyList = motif.getAdjacencyList();
    int product = 1;
    for(int i = 0; i < adjacencyList.size(); i++)
    {
        product *= CLI::nullcaseWeight(combination[adjacencyList[i][0]], combination[adjacencyList[i][1]]);
    }
    return product;
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

    // std::vector<double> motifValues = CLI::constantMotifValues();
    // currentModularity = CLI::triangleModularityPreCalculated(motifValues);

    currentModularity = CLI::motifModularity();
    // currentModularity = CLI::triangleModularity();
    // cout << "Current Modularity " << currentModularity << endl;
    // std::vector<double> bestPartitionValues;
    // std::vector<double> values = CLI::firstIterationTriangleModularity();
    // cout << "values " << values[0] << endl;
    // double previousNumberMotifsInPartitions = values[0];
    // double numberMotifsGraph = values[1];
    // double previousNumberMotifsRandomGraphPartitions = values[2];
    // double numberMotifsRandomGraph = values[3];
    // currentModularity = values[4];

    
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
                // double currentPartitionModularity = CLI::trianangleModularityPreCalculated(motifValues);
                double currentPartitionModularity = CLI::motifModularity();
                // cout << "pnmp2 " << values[0] << endl;
                // vector<double> currentPartitionValues = CLI::changingNodeTriangleModularity(values, chosenNode, chosenNodePartition);
                // currentPartitionModularity = currentPartitionValues[4];
                if (currentPartitionModularity > currentModularity)
                {
                    currentModularity = currentPartitionModularity;
                    betterPartition = i;
                    // bestPartitionValues = currentPartitionValues;
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
            // values = bestPartitionValues;

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
