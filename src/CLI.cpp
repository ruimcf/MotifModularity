#include "CLI.h"
#include "Random.h"
#include <ctime>

using namespace std;

int __number = 0;

Graph *CLI::g;
vector<int> CLI::nodes;
vector<int> CLI::combination;
vector<long> CLI::combinationWeightsArray;
vector<bool> CLI::used;
bool CLI::directed, CLI::weighted, CLI::readPartition, CLI::readMotif;
string CLI::networkFile;
string CLI::networkFileName;
ofstream CLI::resultsFile;
string CLI::partitionFile;
string CLI::motifFile;
int CLI::seed;
ArrayPartition CLI::networkPartition;
Motif CLI::motif;
long total = 0;
double bestModularity;
int *bestPartition;
int CLI::numberOfCommunities;
bool CLI::hasNumberOfCommunities;
bool CLI::notOptimized;
bool CLI::noMotifOrder;

//unique id to identify this run, random number between 1 and 1000000
int uniqueIdentifier;

/**
 * -----------------------
 * --- PARSE ARGUMENTS ---
 * -----------------------
 */
int CLI::parseArgs(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Use with more arg");
        return -1;
    }

    readPartition = false;
    readMotif = false;
    directed = false;
    weighted = false;
    hasNumberOfCommunities = false;
    seed = time(NULL);
    notOptimized = false;
    noMotifOrder = false;

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
                return -1;
            }
            readPartition = true;
            partitionFile = argv[i];
        }
        else if (arg == "--network" || arg == "-n")
        {
            if (++i > argc)
            {
                cout << "network few args" << endl;
                return -1;
            }
            networkFile = argv[i];

            networkFileName = networkFile;

            // Remove directory if present.
            // Do this before extension removal incase directory has a period character.
            const size_t last_slash_idx = networkFileName.find_last_of("\\/");
            if (std::string::npos != last_slash_idx)
            {
                networkFileName.erase(0, last_slash_idx + 1);
            }

            // Remove extension if present.
            const size_t period_idx = networkFileName.rfind('.');
            if (std::string::npos != period_idx)
            {
                networkFileName.erase(period_idx);
            }

            cout << "FILENAME " << networkFileName << endl;
        }
        else if (arg == "--seed" || arg == "-s")
        {
            if (++i >= argc)
            {
                cout << "seed few args" << endl;
                return -1;
            }
            cout << i << " " << argc << endl;
            seed = atoi(argv[i]);
        }

        else if (arg == "--motif" || arg == "-m")
        {
            if (++i > argc)
            {
                cout << "motif few args" << endl;
                return -1;
            }
            readMotif = true;
            motifFile = argv[i];
        }
        else if (arg == "--number-of-communities" || arg == "-nc")
        {
            if (++i > argc)
            {
                cout << "number of communities few args" << endl;
                return -1;
            }
            hasNumberOfCommunities = true;
            numberOfCommunities = stoi(argv[i]);
        }

        else if (arg == "--not-optimized") 
        {
            notOptimized = true;
        }

        else if (arg == "--no-motif-order") 
        {
            noMotifOrder = true;
        }
    }

    if(!hasNumberOfCommunities)
    {
        cout << "Please provide number of communities, ex.: '--number-of-communities 3'" << endl;
        return -1;
    }

    if (networkFile.empty())
    {
        cout << "no network given" << endl;
        return -1;
    }

    if (!readMotif){
        cout << "please provide a motif file" << endl;
        return -1;
    }

    return 1;
}

/**
 * -----------------------------
 * --- RESULTS FILES HELPERS ---
 * -----------------------------
 */

void CLI::openResultsFile()
{
    string resultPath = "resultsCompareMotifOrder/results_" + networkFileName;
    resultsFile.open(resultPath, ios::out | ios::app);
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

void CLI::registerConfigs()
{
    
    string configPath = "configMap.txt";
    ofstream file;
    file.open(configPath, ios::out | ios::app);
    if (file.is_open())
    {
        file << "ID: " << uniqueIdentifier << "\tNetFile: " << networkFile <<
        "\tMotFile: " << motifFile << "\tnc: " << numberOfCommunities <<
        "\tPartition: " << partitionFile << "\tseed: " << seed << 
        "\tdir: " << directed << "\t weight: " << weighted << endl;
        cout << "Registered configs" << endl;
    }
    else
    {
        cout << "Failed to open configMap.txt" << endl;
    }
    file.close();
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
    combination.reserve(g->numNodes());
    combinationWeightsArray.clear();
    combinationWeightsArray.reserve(g->numNodes());
    used.assign(g->numNodes(), false);
    // nodes.clear();
    // nodes.reserve(g->numNodes());
    // for (int i = 0; i < g->numNodes(); ++i)
    // {
    //     nodes.push_back(i);
    // }
}


void CLI::writeNetworkToGephiData() 
{
    ofstream file;
    std::string filePath = "gephi-data/networks/"+networkFileName+"_" + to_string(uniqueIdentifier) + ".nodes.tsv";
    file.open(filePath, ios::out);
    if(!file.is_open()){
        cout << "Error opening partition file for writing" << endl;
        return;
    }
    file << "Id" << endl;
    for(int i = 0; i < g->numNodes(); i++){
        file << i+1 << endl;
    }

    cout << "Wrote nodes file " << filePath << endl;
}

/**
 * ------------
 * --- MAIN ---
 * ------------
 */

void printVector(vector<int> v, string name)
{
    cout << "Vector " + name +":";
    for(int i = 0; i < v.size(); ++i)
    {
        cout << " " << v[i];
    }
    cout << endl;
}

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
    srand(time(0));
    uniqueIdentifier = rand() % (1000000 + 1);
    int parseResult = CLI::parseArgs(argc, argv);
    if(parseResult < 0) return;

    CLI::openResultsFile();
    string argsInfo = "Number of Communities: " + to_string(numberOfCommunities) + " | Motif: " + motifFile + " | SEED: " + to_string(seed) + "\n";
    CLI::writeLineToFile(argsInfo);

    Random::seed(seed);

    //CLI::registerConfigs();
    g = new GraphMatrix();
    GraphUtils::readFileTxt(g, networkFile.c_str(), directed, weighted);

    // if (!noWriteFiles)
    // writeNetworkToGephiData();

    networkPartition.setNumberNodes(g->numNodes());

    motif.readFromFile(motifFile);
    motif.print();

    
    // if (readPartition)
    // {
    //     networkPartition.readPartition(partitionFile.c_str());
    //     cout << "Partition read: " << networkPartition.toStringPartitionByNode() << endl;
    //     //     if (!noWriteFiles)
    //     // networkPartition.writePartitionFile(networkFileName, "real-partition", uniqueIdentifier);
    // }
    // else {
    //     networkPartition.randomPartition(numberOfCommunities);
    //     //     if (!noWriteFiles)
    //     // networkPartition.writePartitionFile(networkFileName, "random-partition", uniqueIdentifier);
    //     cout << "Partition created: " << networkPartition.toStringPartitionByNode() << endl;
    // }
 
    nodes.clear();
    nodes.reserve(g->numNodes());
    for (int i = 0; i < g->numNodes(); i++)
    {
        nodes.push_back(i);
    }

    // total = 0;
    // double modularity = CLI::triangleModularity();
    // printf("Triangle modularity: %f\nTotal: %ld\n", modularity, total);

    // total = 0;
    // double motifModularity = CLI::motifModularity();
    // printf("Motif modularity: %f\nTotal: %d\n", motifModularity, total);

    total = 0;
    // double optimizedMotifModularity = CLI::optimizedMotifModularity();
    // printf("Optimized Motif modularity: %f\nTotal: %ld\n", optimizedMotifModularity, total);

    clock_t begin = clock();
    CLI::singleNodeGreedyAlgorithm();
    clock_t end = clock();
    double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Elapsed seconds: " << elapsedSecs << endl << endl;
    if(notOptimized)
    {
        writeLineToFile("NOT OPTIMIZED Elapsed seconds: " + to_string(elapsedSecs) +"\n");
    }
    else {
        writeLineToFile("OPTIMIZED Elapsed seconds: " + to_string(elapsedSecs) +"\n");
    } 

    
    // for(int i = 0; i < 5; ++i)
    // {
    //     total = 0;
    //     clock_t begin_ = clock();
    //     MotifValues values = CLI::optimizedMotifModularityValues();
    //     cout << "Optimized motif Modularity values: " << CLI::motifModularityFromValues(values) << endl << "Total: " << total << endl;
    //     printMotifValues(values);
    //     clock_t end_ = clock();
    //     double elapsedSecs_ = double(end_ - begin_) / CLOCKS_PER_SEC;
    //     cout << "Elapsed seconds: " << elapsedSecs_ << endl << endl;
    // }

    // total = 0;
    // MotifConstantValues motifConstantValues = CLI::getMotifConstantValues();
    // MotifVariableValues motifVariableValues = CLI::getMotifVariableValues();
    // double constantValuesMotifModularity = CLI::motifModularityFromValues(motifConstantValues, motifVariableValues);
    // printf("Optimized Motif modularity with constant values: %f\n", constantValuesMotifModularity);

    // int changingNode = 1;

    // MotifVariableValues toChangeNodeValues = CLI::nodeVariableValues(changingNode);
    // networkPartition.setNodeCommunity(changingNode, 1);
    // cout << "CHANGED" << endl;
    // total = 0;
    // MotifValues newValues = CLI::optimizedMotifModularityValues();
    // cout << "Modularity: " << motifModularityFromValues(newValues) << endl << " Total: " << total << endl;
    // MotifVariableValues changedNodeValues = CLI::nodeVariableValues(changingNode);
    // MotifValues changingNodeMotifValuesValues = changingNodeMotifValues(values, toChangeNodeValues, changedNodeValues);
    // cout << "Calculated Changing Node modularity: " << CLI::motifModularityFromValues(changingNodeMotifValuesValues) << endl;

    // clock_t begin = clock();
    // CLI::singleNodeGreedyAlgorithm();
    // clock_t end = clock();
    // double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    // cout << "Elapsed seconds: " << elapsedSecs << endl << endl;
    // CLI::writeLineToFile("Elapsed seconds: " + to_string(elapsedSecs) + "\n\n");
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
                // cout << "combination " << i << " " << j << " " << k << endl;
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
    long long g1 = CLI::maskedWeight(combination.back(), combination.front());
    long long g2 = g->hasEdge(combination.back(), combination.front());
    long long g3 = CLI::maskedNullcaseWeight(combination.back(), combination.front());
    long long g4 = CLI::nullcaseWeight(combination.back(), combination.front());

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
    const vector<vector<int> > & orbitRules = noMotifOrder ? motif.getOrbitRules() :  motif.getOrbitRulesWithOrder();
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
    for(int i = 0; i < combination.size(); ++i)
    {
        for(int j = i+1; j < combination.size(); ++j)
        {
            // if has edge in motif, it needs to have edge on the graph
            if(noMotifOrder ? motif.hasEdge(i, j) : motif.hasEdgeWithOrder(i, j))
            {
                if(!g->hasEdge(combination[i], combination[j]))
                    return false;
            } 
            // otherwise, it needs to not have the edge
            else 
                if(g->hasEdge(combination[i], combination[j]))
                    return false;

            // also check the opposite orientation if the motif is directed
            if(motif.isDirected())
            {
                if(noMotifOrder ? motif.hasEdge(j, i) : motif.hasEdgeWithOrder(j, i))
                {
                    if(!g->hasEdge(combination[j], combination[i]))
                        return false;
                } 
                else
                    if(g->hasEdge(combination[j], combination[i]))
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
            if((noMotifOrder ? motif.getCommunity(nodeA) : motif.getCommunityWithOrder(nodeA)) == -1 || (noMotifOrder ? motif.getCommunity(nodeB) : motif.getCommunityWithOrder(nodeB)) == -1)
                continue;

            // If the communities are different in the motif, they have
            // to be different in the partition
            else if ((noMotifOrder ? motif.getCommunity(nodeA) : motif.getCommunityWithOrder(nodeA)) != (noMotifOrder ? motif.getCommunity(nodeB) : motif.getCommunityWithOrder(nodeB)))
            {
                if(CLI::kronecker(combination[nodeA], combination[nodeB]))
                    return false;
            }

            // If the communities are the same on the motif for the pair,
            // they have to be the same on the partition
            else 
                if(!CLI::kronecker(combination[nodeA], combination[nodeB]))
                    return false;
        }
    }
    return true;
}

// For each motif edges, calculate the nullcaseWeight and return the product of them
int CLI::combinationNullcaseWeights()
{
    const vector< vector<int> > & adjacencyList = noMotifOrder ? motif.getAdjacencyList() : motif.getAdjacencyListWithOrder();
    int product = 1;
    for(int i = 0; i < adjacencyList.size(); ++i)
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
        // int combinationWeights = combinationWeightsArray[combination.size() - 1];

        if(edgesCheck && communitiesCheck)
        {
            values->numberMotifsInCommunities += 1;
        }

        if(edgesCheck)
        {
            values->numberMotifsInGraph += 1;
        }

        if(communitiesCheck)
            values->degreeMotifsInCommunities += combinationWeights;

        values->degreeMotifsRandomGraph += combinationWeights;
        total++;
        return;
    }
    // for (int i = CLI::getOrbitRulesNextValidNode(); i < g->numNodes(); i++)
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

            // nextCombinationNullcaseWeights();
            CLI::optimizedMotifModularityValuesIteration(offset + 1, newEdgesCheck, newCommunitiesCheck, values);
            combination.pop_back();
            used[i] = false;
        }
    } 
}

bool CLI::optimizedCombinationOrbitRules()
{
    const vector< vector<int> >& orbitRules = noMotifOrder ? motif.getOrbitRulesSize(combination.size()-1) : motif.getOrbitRulesSizeWithOrder(combination.size()-1);
    for(int i = 0; i < orbitRules.size(); i++)
    {
        if(combination.at(orbitRules[i][0]) >= combination.at(orbitRules[i][1])){
            return false;
        }
    } 
    return true;
}

// The next node that we can add to the combination needs to agree to the most strict orbit rule
int CLI::getOrbitRulesNextValidNode()
{
    const vector< vector<int> >& orbitRules = noMotifOrder ? motif.getOrbitRulesSize(combination.size()) : motif.getOrbitRulesSizeWithOrder(combination.size());
    int maxNode = -1;
    for(int i = 0; i < orbitRules.size(); i++)
    {
        if(combination.at(orbitRules[i][0]) > maxNode){
            maxNode = combination.at(orbitRules[i][0]);
        }
    } 
    if (maxNode == -1)
    {
        return 0;
    }
    return maxNode + 1;
}

// the new node added complies with the motif edges?
bool CLI::optimizedCombinationHasMotifEdges()
{
    int addedNodePos = combination.size() - 1;
    for(int i = 0; i < addedNodePos; ++i)
    {
        //if has edge in motif, it needs to have edge on the graph
        if(noMotifOrder ? motif.hasEdge(addedNodePos, i) : motif.hasEdgeWithOrder(addedNodePos, i)) {
            if(!g->hasEdge(combination[addedNodePos], combination[i]))
                return false;
        }
        // otherwise, it needs to not have the edge
        else
            if(g->hasEdge(combination[addedNodePos], combination[i]))
                return false;
        
        // also check the opposite orientation if the motif is directed
        if(motif.isDirected())
        {
            if(noMotifOrder ? motif.hasEdge(i, addedNodePos) : motif.hasEdgeWithOrder(i, addedNodePos)) 
            {
                if(!g->hasEdge(combination[i], combination[addedNodePos]))
                    return false;
            } 
            else 
                if(g->hasEdge(combination[i], combination[addedNodePos]))
                    return false;
        }
    } 
    return true;
}

// new node maintains the communities of the motif?
bool CLI::optimizedCombinationHasMotifCommunities()
{
    int addedNodePos = combination.size()-1;
    
    // If the added node can be in any community, its all good
    if((noMotifOrder ? motif.getCommunity(addedNodePos) : motif.getCommunityWithOrder(addedNodePos)) == -1)
        return true;
    
    for(int i = 0; i < addedNodePos; ++i)
    {
        // If the node can be in any community we continue
        if ((noMotifOrder ? motif.getCommunity(i) : motif.getCommunityWithOrder(i)) == -1) continue;

        // If the communities are different in the motif, they have
        // to be different in the partition
        if ((noMotifOrder ? motif.getCommunity(i) : motif.getCommunityWithOrder(i)) != (noMotifOrder ? motif.getCommunity(addedNodePos) : motif.getCommunityWithOrder(addedNodePos)))
        {
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

// When a new node is added to the combination, calculate the nullcase weights of that node with the others
void CLI::nextCombinationNullcaseWeights()
{
    const vector< vector<int> > & adjacencyList = noMotifOrder ? motif.getAdjacencyListSize(combination.size() - 1) : motif.getAdjacencyListSizeWithOrder(combination.size() - 1);
    long product = (combination.size() == 1) ? 1 : combinationWeightsArray[combination.size() - 2];

    for(int i = 0; i < adjacencyList.size(); ++i)
    {
        product *= CLI::nullcaseWeight(combination[adjacencyList[i][0]], combination[adjacencyList[i][1]]);
    }

    combinationWeightsArray[combination.size() - 1] = product;
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
    writeLineToFile("-Single Node Greedy Algorithm-\n");
    int chosenNode, chosenIndex, chosenNodePartition, betterPartition;
    MotifValues betterValues;
    double bestModularity, currentModularity;
    vector<int> allNodes;

    networkPartition.randomPartition(numberOfCommunities);
    MotifValues currentValues = CLI::optimizedMotifModularityValues();
    currentModularity = CLI::motifModularityFromValues(currentValues);

    // currentModularity = CLI::optimizedMotifModularity();
    
    cout << "Initial Modularity " << currentModularity << endl;
    writeLineToFile("Initial Modularity: "+to_string(currentModularity)+"\n");
    //if (!noWriteFiles)
    networkPartition.writePartitionFile(networkFileName, "g-initial-partition", uniqueIdentifier);

    FailObject failObject;
    allNodes.reserve(g->numNodes());
    for (int i = 0; i < g->numNodes(); ++i)
    {
        allNodes.push_back(i);
    }
    vector<int> availableNodes(allNodes);

    while (!availableNodes.empty())
    {
        chosenIndex = Random::getInteger(0, availableNodes.size() - 1);
        chosenNode = availableNodes[chosenIndex];

        chosenNodePartition = networkPartition.getNodeCommunity(chosenNode);
        betterPartition = -1;
        //before change get values
        MotifVariableValues beforeChangeNodeValues = CLI::nodeVariableValues(chosenNode); 
        for (int i = 0; i < numberOfCommunities; i++)
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
            // stringstream ss;
            // ss << "Current modularity: " << currentModularity << endl;
            // ss << "Times failed: " << failObject.getConsecutiveTimesFailed() << endl;
            // ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
            // cout << ss.str();
            // printMotifValues(betterValues);
            // cout << "-----------" << endl;
            // writeLineToFile(ss.str());

            failObject.recordSuccess();
            availableNodes = allNodes;
        }
    }

    stringstream ss;
    ss << "Successfully incremented modularity " << failObject.getTimesSuccess() << " times" << endl;
    ss << "Best modularity: " << currentModularity << endl;
    // ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
    // ss << "Number of unique communities: " + to_string(networkPartition.getNumberOfDifferentPartitions()) << endl;
    cout << ss.str();
    
    writeLineToFile(ss.str());
    //if (!noWriteFiles)
    // networkPartition.writePartitionFile(networkFileName, "g-computed-partition", uniqueIdentifier);

    return currentModularity;
}

double CLI::singleNodeGreedyAlgorithmNotOptimized()
{
    cout << "--- Starting greedy ---" << endl;
    writeLineToFile("-Single Node Greedy Algorithm-\n");
    int chosenNode, chosenIndex, chosenNodePartition, betterPartition;
    double  currentModularity;
    vector<int> allNodes;

    networkPartition.randomPartition(numberOfCommunities);


    currentModularity = CLI::optimizedMotifModularity();
    
    cout << "Initial Modularity " << currentModularity << endl;
    writeLineToFile("Initial Modularity: "+to_string(currentModularity)+"\n");
    //if (!noWriteFiles)
    // networkPartition.writePartitionFile(networkFileName, "g-initial-partition", uniqueIdentifier);

    FailObject failObject;
    allNodes.reserve(g->numNodes());
    for (int i = 0; i < g->numNodes(); ++i)
    {
        allNodes.push_back(i);
    }
    vector<int> availableNodes(allNodes);

    while (!availableNodes.empty())
    {
        chosenIndex = Random::getInteger(0, availableNodes.size() - 1);
        chosenNode = availableNodes[chosenIndex];

        chosenNodePartition = networkPartition.getNodeCommunity(chosenNode);
        betterPartition = -1;
        for (int i = 0; i < numberOfCommunities; i++)
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
            // stringstream ss;
            // ss << "Current modularity: " << currentModularity << endl;
            // ss << "Times failed: " << failObject.getConsecutiveTimesFailed() << endl;
            // ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
            // cout << ss.str();
            // printMotifValues(betterValues);
            // cout << "-----------" << endl;
            // writeLineToFile(ss.str());

            failObject.recordSuccess();
            availableNodes = allNodes;
        }
    }

    stringstream ss;
    ss << "Successfully incremented modularity " << failObject.getTimesSuccess() << " times" << endl;
    ss << "Best modularity: " << currentModularity << endl;
    // ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
    // ss << "Number of unique communities: " + to_string(networkPartition.getNumberOfDifferentPartitions()) << endl;
    cout << ss.str();
    
    writeLineToFile(ss.str());
    //if (!noWriteFiles)
    // networkPartition.writePartitionFile(networkFileName, "g-computed-partition", uniqueIdentifier);

    return currentModularity;
}
/**
 * Greedy algorithm that tests the all the possible one node community changes and choses the one
 * that increases the modularity the greatest
 */
double CLI::singleNodeTestAllGreedyAlgorithm()
{
    cout << "--- Starting greedy test all ---" << endl;
    int chosenNode, chosenIndex, chosenNodePartition;
    double bestModularity, currentModularity;
    vector<int> allNodes;
    MotifConstantValues motifConstantValues = CLI::getMotifConstantValues();

    networkPartition.randomPartition(numberOfCommunities);

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
            for (int i = 0; i < numberOfCommunities; i++)
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

            // stringstream ss;
            // ss << "Current modularity: " << currentModularity << endl;
            // ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
            // cout << ss.str();
            // writeLineToFile(ss.str());
        }
    }

    stringstream ss;
    ss << "Best modularity: " << currentModularity << endl;
    ss << "Partition: " << networkPartition.toStringPartitionByNode() << endl;
    cout << ss.str();
    // writeLineToFile(ss.str());

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

double CLI::motifModularityFromValues(long long v1, long long v2, long long v3, long long v4)
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
