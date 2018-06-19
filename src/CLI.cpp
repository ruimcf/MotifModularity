#include "CLI.h"
#include "Random.h"
#include <ctime>

using namespace std;

int __number = 0;
Graph *CLI::g;
vector<int> CLI::nodes;
vector<int> CLI::combination;
vector<bool> CLI::used;
bool CLI::directed, CLI::weighted, CLI::readPartition, CLI::readMotif;
string CLI::networkFile;
ofstream CLI::resultsFile;
string CLI::partitionFile;
string CLI::motifFile;
int CLI::seed;
ArrayPartition CLI::networkPartition;
Motif CLI::motif;
int total = 0;
long CLI::n1 = 0, CLI::n2 = 0, CLI::n3 = 0, CLI::n4 = 0;
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


    if (readPartition)
    {
        networkPartition.readPartition(partitionFile.c_str());
        cout << "Partition read: " << networkPartition.toStringPartitionByNode() << endl;
        printf("Num nodes: %d\n", g->numNodes());
        networkPartition.writePartitionFile(networkFile);
        double modularity = CLI::triangleModularity();
        printf("Triangle modularity: %f\nTotal: %d\n", modularity, total);
        total = 0;
        double motifModularity = CLI::motifModularity();
        printf("Motif modularity: %f\nTotal: %d\n", motifModularity, total);
        total = 0;
        double optimizedMotifModularity = CLI::optimizedMotifModularity();
        printf("Optimized Motif modularity: %f\nTotal: %d\n", optimizedMotifModularity, total);
        total = 0;
        CLI::getMotifConstantValues();
        cout << "Total " << total << endl;
    }
    else
    {
        networkPartition.randomPartition(2);
    }

    getchar();
    for (int i = 0; i < g->numNodes(); i++)
    {
        nodes.push_back(i);
    }

    clock_t begin = clock();
    CLI::singleNodeTestAllGreedyAlgorithm();
    clock_t end = clock();
    double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Elapsed seconds: " << elapsedSecs << endl;
    CLI::closeResultsFile();
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

    cout << "n2: " << numberMotifsGraph << " n4: " << static_cast<long>(numberMotifsRandomGraph) << endl;
    return motifModularity;
}

void CLI::setNodes()
{
    combination.clear();
    used.clear();
    nodes.clear();
    nodes.reserve(g->numNodes());
    used.reserve(g->numNodes());
    combination.reserve(g->numNodes());
    for (int i = 0; i < g->numNodes(); i++)
    {
        nodes.push_back(i);
        used.push_back(false);
    }
}


double CLI::cicleModularity(int size)
{
    n1 = n2 = n3 = n4 = 0;
    combination.clear();
    CLI::iterateCombinations(0, size);
    return static_cast<double>(n1) / n2 - static_cast<double>(n3) / n4;
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

MotifConstantValues CLI::getMotifConstantValues()
{
    cout << "Motif Contant values" << endl;
    MotifConstantValues *ptr, values;
    ptr = &values;
    ptr->numberMotifsInGraph = 0;
    ptr->degreeMotifsRandomGraph = 0;
    CLI::setNodes();
    CLI::motifConstantValuesIteration(0, ptr);
    return values;
}

void CLI::motifConstantValuesIteration(int offset, MotifConstantValues *ptr)
{
    if(offset == motif.getSize())
    {
        int combinationWeights = CLI::combinationNullcaseWeights();
        bool edgesCheck = CLI::combinationHasMotifEdges(); 

        if(edgesCheck)
            ptr->numberMotifsInGraph += 1;

        ptr->degreeMotifsRandomGraph += combinationWeights;
        total++;
        return;
    }
    for (int i = 0; i < g->numNodes(); i++)
    {
        if(!used[i])
        {
            used[i] = true;
            combination.push_back(nodes[i]);
            // cut down on simetric motif
            if(!optimizedCombinationOrbitRules()){
                combination.pop_back();
                used[i] = false;
                continue;
            } 
            CLI::motifConstantValuesIteration(offset + 1, ptr);
            combination.pop_back();
            used[i] = false;
        }
    } 

}

double CLI::optimizedMotifModularity()
{
    n1 = n2 = n3 = n4 = 0;
    CLI::setNodes();
    CLI::optimizedNodeCombination(0, true, true);
    return static_cast<double>(n1) / n2 - static_cast<double>(n3) / n4;
}

void CLI::optimizedNodeCombination(int offset, bool edgesCheck, bool communitiesCheck)
{
    if(offset == motif.getSize())
    {
        int combinationWeights = CLI::combinationNullcaseWeights();

        if(edgesCheck && communitiesCheck)
            n1 += 1;
        if(edgesCheck)
            n2 += 1;
        if(communitiesCheck)
            n3 += combinationWeights;

        n4 += combinationWeights;
        total++;
        return;
    }
    for (int i = 0; i < g->numNodes(); i++)
    {
        if(!used[i])
        {
            used[i] = true;
            combination.push_back(nodes[i]);
            // cut down on simetric motif
            if(!optimizedCombinationOrbitRules()){
                combination.pop_back();
                used[i] = false;
                continue;
            } 
            bool newEdgesCheck = edgesCheck, newCommunitiesCheck = communitiesCheck;
            if(edgesCheck)
                newEdgesCheck = optimizedCombinationHasMotifEdges(); 
            if(communitiesCheck)
                newCommunitiesCheck = optimizedCombinationHasMotifCommunities();

            CLI::optimizedNodeCombination(offset + 1, newEdgesCheck, newCommunitiesCheck);
            combination.pop_back();
            used[i] = false;
        }
    } 
}

bool CLI::optimizedCombinationOrbitRules()
{
    const vector< vector<int> >& orbitRules = motif.getOrbitRulesSize(combination.size()-1);
    for(int i = 0; i < orbitRules.size(); i++)
    {
        if(combination.at(orbitRules[i][0]) >= combination.at(orbitRules[i][1])){
            return false;
        }
    } 
    return true;
}

// the new node added complies with the motif edges?
bool CLI::optimizedCombinationHasMotifEdges()
{
    const vector< vector<int> >& adjacencyMatrix = motif.getAdjacencyMatrix();
    int addedNodePos = combination.size() -1;
    for(int i = 0; i < addedNodePos; i++)
    {
        //if has edge in motif, it needs to have edge on the graph
        if(adjacencyMatrix[addedNodePos][i]) {
            if(!g->hasEdge(combination[addedNodePos], combination[i])){
                return false;
            }
        } else {
            // otherwise, it needs to not have the edge
            if(g->hasEdge(combination[addedNodePos], combination[i])){
                return false;
            }
        }
    } 
    if (motif.isDirected()){
        for(int i = 0; i < addedNodePos; i++)
        {
            if(adjacencyMatrix[i][addedNodePos]) {
                if(!g->hasEdge(combination[i], combination[addedNodePos])){
                    return false;
                }
            } else {
                if(g->hasEdge(combination[i], combination[addedNodePos])){
                    return false;
                }
            }
        } 
    }
    return true;
}

// new node maintains the communities of the motif?
bool CLI::optimizedCombinationHasMotifCommunities()
{
    vector<int> communities = motif.getCommunities();
    int addedNodePos = combination.size()-1;
    for(int i = 0; i < addedNodePos; i++)
    {
        // If the added node can be in any community, its all good
        if(communities[addedNodePos] == -1) return true;

        // If one of the nodes can be in any community we continue
        if (communities[i] == -1) continue;

        // If the communities are different in the motif, they have
        // to be different in the partition
        if (communities[i] != communities[addedNodePos]){
            if(CLI::kronecker(combination[i], combination[addedNodePos]))
                return false;
        }
        // If the communities are the same on the motif for the pair,
        // they have to be the same on the partition
        else 
            if(!CLI::kronecker(combination[i], combination[addedNodePos]))
                return false;
    }
    return true;
}

double CLI::motifModularity()
{
    n1 = n2 = n3 = n4 = 0;
    CLI::setNodes();
    CLI::nodeCombination(0);
    cout << "n2: " << n2 << " n4: " << n4 << endl;
    return static_cast<double>(n1) / n2 - static_cast<double>(n3) / n4;
}

void CLI::nodeCombination(int offset)
{
    if(offset == motif.getSize())
    {
        CLI::countCombinationMotifs();
        return;
    }
    for (int i = 0; i < g->numNodes(); i++)
    {
        if(!used[i])
        {
            used[i] = true;
            combination.push_back(nodes[i]);
            //Aqui posso verificar: as edges criadas são validas para a motif? os nodes estão de acordo as regras da motif?
            CLI::nodeCombination(offset + 1);
            combination.pop_back();
            used[i] = false;
        }
    } 
}

void CLI::countCombinationMotifs()
{
    //I need to check different permutations of this set of nodes
    //But for now I will use only the given one
    const vector<vector<int> > & orbitRules = motif.getOrbitRules();
    for(int i = 0; i < orbitRules.size(); i++)
    {
        if(combination.at(orbitRules[i][0]) >= combination.at(orbitRules[i][1])){
            return;
        }
    }
    total++;

    bool motifEdgesCheck = CLI::combinationHasMotifEdges();
    bool motifCommunitiesCheck = CLI::combinationHasMotifCommunities();
    int combinationWeights = CLI::combinationNullcaseWeights();

    if (motifEdgesCheck)
    {
        //This partition contains a motif
        if(motifCommunitiesCheck) 
            n1 += 1;
        //Total graph contains a motif
        n2 += 1;
    }

    if(motifCommunitiesCheck) 
        n3 += combinationWeights;

    n4 += combinationWeights;
}

// Check if the combination edges are according the motif
// i.e. check if the combination is an occurence of the motif
bool CLI::combinationHasMotifEdges()
{
    const vector< vector<int> > & adjacencyList = motif.getAdjacencyList();
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
    const vector< vector<int> > & adjacencyList = motif.getAdjacencyList();
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
    cout << "--- Starting greedy ---" << endl;
    int chosenNode, chosenIndex, chosenNodePartition, betterPartition, numPartitions;
    double bestModularity, currentModularity;
    vector<int> allNodes;

    numPartitions = 4;
    // numPartitions = numberForEvenPartitions(g->numNodes());
    networkPartition.randomPartition(numPartitions);

    currentModularity = CLI::optimizedMotifModularity();
    
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
                double currentPartitionModularity = CLI::optimizedMotifModularity();
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

/**
 * Greedy algorithm that tests the all the possible one node community changes and choses the one
 * that increases the modularity the greatest
 */
double CLI::singleNodeTestAllGreedyAlgorithm()
{
    cout << "--- Starting greedy test all ---" << endl;
    int chosenNode, chosenIndex, chosenNodePartition, numPartitions;
    double bestModularity, currentModularity;
    vector<int> allNodes;

    numPartitions = 4;
    networkPartition.randomPartition(numPartitions);

    currentModularity = CLI::optimizedMotifModularity();
    cout << "Current Modularity " << currentModularity << endl;

    allNodes.reserve(g->numNodes());
    for (int i = 0; i < g->numNodes(); ++i)
    {
        allNodes.push_back(i);
    }
    vector<int> availableNodes(allNodes);

    int bestNodeToChange = -1, bestCommunity = -1;
    bool hasImprovenments = true;
    while(hasImprovenments)
    {
        bestNodeToChange = -1;
        bestCommunity = -1;
        for(int chosenIndex = 0; chosenIndex < availableNodes.size(); ++chosenIndex)
        {
            chosenNode = availableNodes[chosenIndex];
            chosenNodePartition = networkPartition.getNodeCommunity(chosenNode);
            for (int i = 0; i < numPartitions; i++)
            {
                if (i != chosenNodePartition)
                {
                    networkPartition.setNodeCommunity(chosenNode, i);
                    double currentPartitionModularity = CLI::optimizedMotifModularity();
                    if (currentPartitionModularity > currentModularity)
                    {
                        currentModularity = currentPartitionModularity;
                        bestCommunity = i;
                        bestNodeToChange = chosenNode;
                    }
                }
            }
            networkPartition.setNodeCommunity(chosenNode, chosenNodePartition);
        }
        if (bestNodeToChange == -1)
        {
            hasImprovenments = false;
        }
        else
        {
            networkPartition.setNodeCommunity(bestNodeToChange, bestCommunity);

            stringstream ss;
            ss << "Current modularity: " << currentModularity << endl;
            ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
            cout << ss.str();
            writeLineToFile(ss.str());
        }
    }

    stringstream ss;
    ss << "Best modularity: " << currentModularity << endl;
    ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
    cout << ss.str();
    writeLineToFile(ss.str());

    return currentModularity;
}
