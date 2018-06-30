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
double bestModularity;
int *bestPartition;

/**
 * -----------------------
 * --- PARSE ARGUMENTS ---
 * -----------------------
 */
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

/**
 * -----------------------------
 * --- RESULTS FILES HELPERS ---
 * -----------------------------
 */

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

/**
 * -----------------
 * --- SET NODES ---
 * -----------------
 */
// how to stop relying on this function?
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

/**
 * ------------
 * --- MAIN ---
 * ------------
 */

void printMotifValues(MotifValues values)
{
    cout << "Motif Values: " << values.numberMotifsInCommunities << " " << values.numberMotifsInGraph << " " <<  values.degreeMotifsInCommunities << " " << values.degreeMotifsRandomGraph << endl;
}

void printMotifVariableValues(MotifVariableValues values)
{
    cout << "Motif Variable Values: " << values.numberMotifsInCommunities << " " << values.degreeMotifsInCommunities << endl;
}

void printMotifConstantValues(MotifConstantValues values)
{
    cout << "Motif Constant Values: " << values.numberMotifsInGraph << " " << values.degreeMotifsRandomGraph << endl; 
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

        total = 0;
        double modularity = CLI::triangleModularity();
        printf("Triangle modularity: %f\nTotal: %d\n", modularity, total);

        total = 0;
        double motifModularity = CLI::motifModularity();
        printf("Motif modularity: %f\nTotal: %d\n", motifModularity, total);

        total = 0;
        double optimizedMotifModularity = CLI::optimizedMotifModularity();
        printf("Optimized Motif modularity: %f\nTotal: %d\n", optimizedMotifModularity, total);

        total = 0;
        MotifValues values = CLI::optimizedMotifModularityValues();
        cout << "Optimized motif Modularity values: " << CLI::motifModularityFromValues(values) << endl << "Total: " << total << endl;

        total = 0;
        MotifConstantValues motifConstantValues = CLI::getMotifConstantValues();
        MotifVariableValues motifVariableValues = CLI::getMotifVariableValues();
        double constantValuesMotifModularity = CLI::motifModularityFromValues(motifConstantValues, motifVariableValues);
        printf("Optimized Motif modularity with constant values: %f\n", constantValuesMotifModularity);

        int changingNode = 0;

        MotifVariableValues toChangeNodeValues = CLI::nodeVariableValues(changingNode);
        networkPartition.setNodeCommunity(changingNode, 1);
        cout << "CHANGED" << endl;
        total = 0;
        MotifValues newValues = CLI::optimizedMotifModularityValues();
        cout << "Modularity: " << motifModularityFromValues(newValues) << endl << " Total: " << total << endl;
        MotifVariableValues changedNodeValues = CLI::nodeVariableValues(changingNode);
        MotifValues changingNodeMotifValuesValues = changingNodeMotifValues(values, toChangeNodeValues, changedNodeValues);
        cout << "Calculated Changing Node modularity: " << CLI::motifModularityFromValues(changingNodeMotifValuesValues) << endl;
 
    }

    getchar();
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
}

/**
 * ---------------------------
 * --- TRIANGLE MODULARITY ---
 * ---------------------------
 */
double CLI::triangleModularity()
{
    int n = g->numNodes();

    MotifValues values = { 0, 0, 0, 0 };
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            for (int k = j + 1; k < n; k++)
            {
                total++;
                values.numberMotifsInCommunities += CLI::maskedWeight(i, j) * CLI::maskedWeight(j, k) * CLI::maskedWeight(i, k);
                values.numberMotifsInGraph += g->hasEdge(i, j) * g->hasEdge(j, k) * g->hasEdge(i, k);
                values.degreeMotifsInCommunities += CLI::maskedNullcaseWeight(i, j) * CLI::maskedNullcaseWeight(j, k) * CLI::maskedNullcaseWeight(i, k);
                values.degreeMotifsRandomGraph += CLI::nullcaseWeight(i, j) * CLI::nullcaseWeight(j, k) * CLI::nullcaseWeight(i, k);
            }
        }
    }

    return CLI::motifModularityFromValues(values);;
}

/**
 * ------------------------
 * --- CICLE MODULARITY ---
 * ------------------------
 */
double CLI::cicleModularity(int size)
{
    MotifValues motifValues = { 0, 0, 0, 0};
    combination.clear();
    CLI::cicleModularityIteration(0, size, &motifValues);
    return CLI::motifModularityFromValues(motifValues);
}

void CLI::cicleModularityIteration(int offset, int k, MotifValues *values)
{
    if (k == 0)
    {
        CLI::combinationCicleModularity(values);
        return;
    }
    for (int i = offset; i < g->numNodes(); i++)
    {
        combination.push_back(nodes[i]);
        CLI::cicleModularityIteration(i + 1, k - 1, values);
        combination.pop_back();
    }
}

void CLI::combinationCicleModularity(MotifValues *values)
{
    long g1 = CLI::maskedWeight(combination.back(), combination.front());
    long g2 = g->hasEdge(combination.back(), combination.front());
    long g3 = CLI::maskedNullcaseWeight(combination.back(), combination.front());
    long g4 = CLI::nullcaseWeight(combination.back(), combination.front());

    for (int i = 0; i < combination.size() - 1; i++)
    {
        g1 *= CLI::maskedWeight(combination[i], combination[i + 1]);
        g2 *= g->hasEdge(combination[i], combination[i + 1]);
        g3 *= CLI::maskedNullcaseWeight(combination[i], combination[i + 1]);
        g4 *= CLI::nullcaseWeight(combination[i], combination[i + 1]);
    }

    total++;
    values->numberMotifsInCommunities += g1;
    values->numberMotifsInGraph += g2;
    values->degreeMotifsInCommunities += g3;
    values->degreeMotifsRandomGraph += g4;
}

/**
 *  -------------------------
 *  ---  MOTIF MODULARITY ---
 *  -------------------------
 */

double CLI::motifModularity()
{
    MotifValues motifValues = { 0, 0, 0, 0 };
    CLI::setNodes();
    CLI::nodeCombination(0, &motifValues);
    return CLI::motifModularityFromValues(motifValues);
}

void CLI::nodeCombination(int offset, MotifValues *values)
{
    if(offset == motif.getSize())
    {
        CLI::countCombinationMotifs(values);
        return;
    }
    for (int i = 0; i < g->numNodes(); i++)
    {
        if(!used[i])
        {
            used[i] = true;
            combination.push_back(nodes[i]);
            //Aqui posso verificar: as edges criadas são validas para a motif? os nodes estão de acordo as regras da motif?
            CLI::nodeCombination(offset + 1, values);
            combination.pop_back();
            used[i] = false;
        }
    } 
}

void CLI::countCombinationMotifs(MotifValues *values)
{

    if(!CLI::combinationObeysOrbitRules()) return;
    bool motifEdgesCheck = CLI::combinationHasMotifEdges();
    bool motifCommunitiesCheck = CLI::combinationHasMotifCommunities();
    int combinationWeights = CLI::combinationNullcaseWeights();

    if (motifEdgesCheck)
    {
        //This partition contains a motif
        if(motifCommunitiesCheck) 
            values->numberMotifsInCommunities += 1;
        //Total graph contains a motif
        values->numberMotifsInGraph += 1;
    }

    if(motifCommunitiesCheck) 
        values->degreeMotifsInCommunities += combinationWeights;

    values->degreeMotifsRandomGraph += combinationWeights;
    total++;
}

bool CLI::combinationObeysOrbitRules()
{
    const vector<vector<int> > & orbitRules = motif.getOrbitRulesWithOrder();
    for(int i = 0; i < orbitRules.size(); i++)
    {
        if(combination.at(orbitRules[i][0]) >= combination.at(orbitRules[i][1])){
            return false;
        }
    }
    return true;
}

// Check if the combination edges are according the motif
// i.e. check if the combination is an occurence of the motif
bool CLI::combinationHasMotifEdges()
{
    for(int i = 0; i < combination.size(); i++)
    {
        for(int j = 0; j < combination.size(); ++j)
        {
            if(motif.hasEdgeWithOrder(i, j))
            {
                 //if has edge in motif, it needs to have edge on the graph
                if(!g->hasEdge(combination[i], combination[j]))
                    return false;
            } 
            else {
                // otherwise, it needs to not have the edge
                if(g->hasEdge(combination[i], combination[j]))
                    return false;
            }
        }
    } 
    return true;
}

// Check if the combination communities are in accordance with the motif communities
bool CLI::combinationHasMotifCommunities()
{
    for(int nodeA = 0; nodeA < combination.size(); ++nodeA)
    {
        for(int nodeB = nodeA + 1; nodeB < combination.size(); ++nodeB)
        {
            // If one of the nodes can be in any community we continue
            if(motif.getCommunityWithOrder(nodeA) == -1 || motif.getCommunityWithOrder(nodeB) == -1){
                continue;
            }
            // If the communities are different in the motif, they have
            // to be different in the partition
            else if (motif.getCommunityWithOrder(nodeA) != motif.getCommunityWithOrder(nodeB)){
                if(CLI::kronecker(combination[nodeA], combination[nodeB]))
                {
                    return false;
                }
            }
            // If the communities are the same on the motif for the pair,
            // they have to be the same on the partition
            else {
                if(!CLI::kronecker(combination[nodeA], combination[nodeB]))
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


/**
 * ----------------------------------
 * --- OPTIMIZED MOTIF MODULARITY ---
 * ----------------------------------
 */

double CLI::optimizedMotifModularity()
{
    MotifValues motifValues = CLI::optimizedMotifModularityValues();
    return CLI::motifModularityFromValues(motifValues);
}

MotifValues CLI::optimizedMotifModularityValues()
{
    MotifValues motifValues = { 0, 0, 0, 0 };
    CLI::setNodes();
    CLI::optimizedMotifModularityValuesIteration(0, true, true, &motifValues); 
    return motifValues;
}

void CLI::optimizedMotifModularityValuesIteration(int offset, bool edgesCheck, bool communitiesCheck, MotifValues *values)
{
    if(offset == motif.getSize())
    {
        int combinationWeights = CLI::combinationNullcaseWeights();

        if(edgesCheck && communitiesCheck)
            values->numberMotifsInCommunities += 1;

        if(edgesCheck)
            values->numberMotifsInGraph += 1;

        if(communitiesCheck)
            values->degreeMotifsInCommunities += combinationWeights;

        values->degreeMotifsRandomGraph += combinationWeights;
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

            CLI::optimizedMotifModularityValuesIteration(offset + 1, newEdgesCheck, newCommunitiesCheck, values);
            combination.pop_back();
            used[i] = false;
        }
    } 
}

bool CLI::optimizedCombinationOrbitRules()
{
    const vector< vector<int> >& orbitRules = motif.getOrbitRulesSizeWithOrder(combination.size()-1);
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
    int addedNodePos = combination.size() - 1;
    for(int i = 0; i < addedNodePos; ++i)
    {
        //if has edge in motif, it needs to have edge on the graph
        if(motif.hasEdgeWithOrder(addedNodePos, i)) {
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
    return true;
}

// new node maintains the communities of the motif?
bool CLI::optimizedCombinationHasMotifCommunities()
{
    int addedNodePos = combination.size()-1;
    
    // If the added node can be in any community, its all good
    if(motif.getCommunityWithOrder(addedNodePos) == -1)
        return true;
    
    for(int i = 0; i < addedNodePos; i++)
    {
        // If the node can be in any community we continue
        if (motif.getCommunityWithOrder(i) == -1) continue;

        // If the communities are different in the motif, they have
        // to be different in the partition
        if (motif.getCommunityWithOrder(i) != motif.getCommunityWithOrder(addedNodePos)){
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

/**
 * -----------------------
 * --- CONSTANT VALUES ---
 * -----------------------
 */

MotifConstantValues CLI::getMotifConstantValues()
{
    MotifConstantValues *ptr, values;
    ptr = &values;
    ptr->numberMotifsInGraph = 0;
    ptr->degreeMotifsRandomGraph = 0;
    CLI::setNodes();
    CLI::getMotifConstantValuesIteration(0, ptr);
    return values;
}

void CLI::getMotifConstantValuesIteration(int offset, MotifConstantValues *ptr)
{
    if(offset == motif.getSize())
    {
        int combinationWeights = CLI::combinationNullcaseWeights();
        bool edgesCheck = CLI::combinationHasMotifEdges(); 

        if(edgesCheck)
            ptr->numberMotifsInGraph += 1;

        ptr->degreeMotifsRandomGraph += combinationWeights;
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
            CLI::getMotifConstantValuesIteration(offset + 1, ptr);
            combination.pop_back();
            used[i] = false;
        }
    } 

}

MotifVariableValues CLI::getMotifVariableValues()
{
    MotifVariableValues motifVariableValues = { 0, 0 };
    CLI::setNodes();
    CLI::getMotifVariableValuesIteration(0, true, &motifVariableValues);
    return motifVariableValues;
}

void CLI::getMotifVariableValuesIteration(int offset, bool edgesCheck, MotifVariableValues *values)
{
    if(offset == motif.getSize())
    {

        if(edgesCheck)
            values->numberMotifsInCommunities += 1;
        
        int combinationWeights = CLI::combinationNullcaseWeights();
        values->degreeMotifsInCommunities += combinationWeights;

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
            if(!optimizedCombinationHasMotifCommunities()){
                combination.pop_back();
                used[i] = false;
                continue;
            }

            bool newEdgesCheck = edgesCheck;
            if(edgesCheck)
                newEdgesCheck = optimizedCombinationHasMotifEdges(); 
            
            CLI::getMotifVariableValuesIteration(offset + 1, newEdgesCheck, values);
            combination.pop_back();
            used[i] = false;
        }
    } 
}

/**
 * ---------------------
 * --- CHANGING NODE ---
 * ---------------------
 */

MotifVariableValues CLI::nodeVariableValues(int changingNode)
{
    MotifVariableValues changingNodeValues;
    changingNodeValues.degreeMotifsInCommunities = 0;
    changingNodeValues.numberMotifsInCommunities = 0;
    used[changingNode] = true;
    for(int i = 0; i < motif.getSize(); i++)
    {
        CLI::nodeVariableValuesIteration(changingNode, i, 0, &changingNodeValues);
    }
    used[changingNode] = false;
    return changingNodeValues;
}

void CLI::nodeVariableValuesIteration(int changingNode, int position, int offset, MotifVariableValues *changingNodeValues)
{
    if(offset == motif.getSize())
    {
        int combinationWeights = CLI::combinationNullcaseWeights();

        bool edgesCheck = combinationHasMotifEdges(); 
        if(edgesCheck)
            changingNodeValues->numberMotifsInCommunities += 1;
        
        changingNodeValues->degreeMotifsInCommunities += combinationWeights;

        return;
    }
    if(offset == position)
    {
        combination.push_back(changingNode);
        // cut down on simetric motif
        if(!optimizedCombinationOrbitRules()){
            combination.pop_back();
            return;
        };
        bool communitiesCheck = optimizedCombinationHasMotifCommunities();
        if(!communitiesCheck)
        {
            combination.pop_back();
            return;
        }

        CLI::nodeVariableValuesIteration(changingNode, position, offset+1, changingNodeValues);
        combination.pop_back();
    }
    else
    {
        for (int i = 0; i < g->numNodes(); i++)
        {
            if(!used[i])
            {
                used[i] = true;
                combination.push_back(nodes[i]);
                // cut down on simetric motif
                if(!optimizedCombinationOrbitRules())
                {
                    combination.pop_back();
                    used[i] = false;
                    continue;
                } 
                bool communitiesCheck = optimizedCombinationHasMotifCommunities();
                if(!communitiesCheck)
                {
                    combination.pop_back();
                    used[i] = false;
                    continue;
                }
                CLI::nodeVariableValuesIteration(changingNode, position, offset + 1, changingNodeValues);
                combination.pop_back();
                used[i] = false;
            }
        } 
    }
}


MotifValues CLI::changingNodeMotifValues(MotifValues allPreviousValues, MotifVariableValues beforeChangeValues, MotifVariableValues afterChangeValues)
{
    MotifValues values;
    values.numberMotifsInCommunities = allPreviousValues.numberMotifsInCommunities + afterChangeValues.numberMotifsInCommunities - beforeChangeValues.numberMotifsInCommunities;
    values.numberMotifsInGraph = allPreviousValues.numberMotifsInGraph;
    values.degreeMotifsInCommunities = allPreviousValues.degreeMotifsInCommunities + afterChangeValues.degreeMotifsInCommunities - beforeChangeValues.degreeMotifsInCommunities;
    values.degreeMotifsRandomGraph = allPreviousValues.degreeMotifsRandomGraph;
   return values;
}



/**
 * -------------------------
 * --- GREEDY ALGORITHMS ---
 * -------------------------
 */

int numberForEvenTrianglePartitions(int numNodes)
{
    return (int)ceil(numNodes / 3.0);
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

double CLI::singleNodeGreedyAlgorithm()
{
    cout << "--- Starting greedy ---" << endl;
    int chosenNode, chosenIndex, chosenNodePartition, betterPartition, numPartitions;
    MotifValues betterValues;
    double bestModularity, currentModularity;
    vector<int> allNodes;

    numPartitions = 4;
    // numPartitions = numberForEvenTrianglePartitions(g->numNodes());
    networkPartition.randomPartition(numPartitions);
    MotifValues currentValues = CLI::optimizedMotifModularityValues();
    currentModularity = CLI::motifModularityFromValues(currentValues);

    // currentModularity = CLI::optimizedMotifModularity();
    
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
        //before change get values
        MotifVariableValues beforeChangeNodeValues = CLI::nodeVariableValues(chosenNode); 
        for (int i = 0; i < numPartitions; i++)
        {
            if (i != chosenNodePartition)
            {
                networkPartition.setNodeCommunity(chosenNode, i);
                MotifVariableValues afterChangeNodeValues = CLI::nodeVariableValues(chosenNode);
                MotifValues currentPartitionModularityValues = CLI::changingNodeMotifValues(currentValues, beforeChangeNodeValues, afterChangeNodeValues);
                double currentPartitionModularity = CLI::motifModularityFromValues(currentPartitionModularityValues);
                if (currentPartitionModularity > currentModularity)
                {
                    currentModularity = currentPartitionModularity;
                    betterValues = currentPartitionModularityValues;
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
            currentValues = betterValues;
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
    MotifConstantValues motifConstantValues = CLI::getMotifConstantValues();

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

/**
 * ------------------------------------------
 * --- Motif Calculation Helper Functions ---
 * ------------------------------------------
 */
double CLI::motifModularityFromValues(MotifValues values)
{
    return static_cast<double>(values.numberMotifsInCommunities) / values.numberMotifsInGraph - static_cast<double>(values.degreeMotifsInCommunities) / values.degreeMotifsRandomGraph; 
}

double CLI::motifModularityFromValues(MotifConstantValues constantValues, MotifVariableValues variableValues)
{
    return static_cast<double>(variableValues.numberMotifsInCommunities) / constantValues.numberMotifsInGraph - static_cast<double>(variableValues.degreeMotifsInCommunities) / constantValues.degreeMotifsRandomGraph; 
}

double CLI::motifModularityFromValues(long v1, long v2, long v3, long v4)
{
    return static_cast<double>(v1) / v2 - static_cast<double>(v3) / v4; 
}

/**
 * ------------------------------------------
 * --- Motif CALCULATION HELPER FUNCTIONS ---
 * ------------------------------------------
 */

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

/**
 * -----------------------------------
 * --- CREATE ALL GRAPH PARTITIONS ---
 * -----------------------------------
 */

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
