import sys
from random import randint, seed
from shutil import copyfile


def readMotifFile(motifFile):
    numberOfNodes = 0
    motifCommunities = []
    motifAdjList = []
    motifAdjMatrix = []
    with open(motifFile, "r") as mf:
        numberOfNodes = int(next(mf))
        direction = next(mf).strip()
        isDirected = False
        if direction == "directed":
            isDirected = True
        motifCommunities = [int(community) for community in next(mf).split()]

        # MOTIF ADJ LIST & MATRIX
        motifAdjList = []
        motifAdjMatrix = [[0] * numberOfNodes for k in range(numberOfNodes)]
        for line in mf:
            pair = line.split()
            pair[0] = int(pair[0]) - 1    # subtract one
            pair[1] = int(pair[1]) - 1    # to match indexes
            motifAdjList.append(pair)
            motifAdjMatrix[pair[0]][pair[1]] = 1
            if not isDirected:
                motifAdjMatrix[pair[1]][pair[0]] = 1
    # print "MotifAdjList", motifAdjList
    # print "MotifAdjMatrix", motifAdjMatrix
    # print "MotifCommunities", motifCommunities
    # print "Number of nodes", numberOfNodes
    return (motifAdjList, motifAdjMatrix, motifCommunities, numberOfNodes)


def readNetworkFile(networkFile, isNetworkDirected):
    # NETWORK NODES LIST
    maxNumberOfNodes = -1
    nodesList = []
    with open(networkFile, "r") as fd:
        for line in fd:
            pair = line.split()
            firstNode = int(pair[0]) - 1    # subtract one
            secondNode = int(pair[1]) - 1   # to match indexes
            if firstNode not in nodesList:
                nodesList.append(firstNode)
            if secondNode not in nodesList:
                nodesList.append(secondNode)
            if firstNode > maxNumberOfNodes:
                maxNumberOfNodes = firstNode
            if secondNode > maxNumberOfNodes:
                maxNumberOfNodes = secondNode
    maxNumberOfNodes += 1  # account for the subtraction
    numberOfNodes = len(nodesList)
    if maxNumberOfNodes != numberOfNodes:
        print "Number of nodes and nodes indexes do not match! But it is ok."

    # NETWORK ADJ LIST & MATRIX
    networkAdjMatrix = [
        [0] * maxNumberOfNodes for k in range(maxNumberOfNodes)]
    with open(networkFile, "r") as fd:
        for line in fd:
            pair = line.split()
            firstNode = int(pair[0]) - 1
            secondNode = int(pair[1]) - 1
            networkAdjMatrix[firstNode][secondNode] = 1
            if not isNetworkDirected:
                networkAdjMatrix[secondNode][firstNode] = 1

    networkCommunities = [-1] * maxNumberOfNodes

    # print "maxNumberOfNodes", maxNumberOfNodes
    # print "numberOfNodes", numberOfNodes
    # print "nodes list", nodesList
    # print "networkAdjMatrix", networkAdjMatrix
    # print "networkCommunities", networkCommunities
    return (maxNumberOfNodes, nodesList, networkAdjMatrix, networkCommunities)


# nodes have to obey the edge rules
def new_node_has_forbidden_edges(networkAdjMatrix, motifAdjMatrix, chosenNodes):
    new_node_pos = len(chosenNodes) - 1
    new_node = chosenNodes[new_node_pos]

    for i in range(new_node_pos):
        # not supposed to have an edge
        if motifAdjMatrix[i][new_node_pos] == 0:
            if networkAdjMatrix[chosenNodes[i]][chosenNodes[new_node_pos]] == 1:
                return True
        if motifAdjMatrix[new_node_pos][i] == 0:
            if networkAdjMatrix[chosenNodes[new_node_pos]][chosenNodes[i]] == 1:
                return True
    return False


def new_node_has_forbidden_community(networkCommunities, motifCommunities, chosenNodes):
    new_node_pos = len(chosenNodes) - 1
    new_node = chosenNodes[new_node_pos]
    if motifCommunities[new_node_pos] == -1:
        return False

    communitiesChanged = []
    # some community was alreaddy attributed to this node
    # changing it would break a previous motif occurrence
    # a node will always have the same community as specified on the motif
    if networkCommunities[new_node] != -1:
        if networkCommunities[new_node] != motifCommunities[new_node_pos]:
            return True
    else:
        networkCommunities[new_node] = motifCommunities[new_node_pos]
        communitiesChanged.append(new_node)

    for i in range(new_node_pos):
        current_node = chosenNodes[i]
        if motifCommunities[i] == -1:
            continue

        if motifCommunities[i] == motifCommunities[new_node_pos]:
            if networkCommunities[current_node] != networkCommunities[new_node]:
                for node in communitiesChanged:
                    networkCommunities[node] = -1
                return True

        if motifCommunities[i] != motifCommunities[new_node_pos]:
            if networkCommunities[current_node] == networkCommunities[new_node]:
                for node in communitiesChanged:
                    networkCommunities[node] = -1
                return True


def addMotifsToNetwork(networkFile, motifFile, numberOfMotifs, isNetworkDirected):
    motifAdjList, motifAdjMatrix, motifCommunities, numberOfNodes = readMotifFile(
        motifFile)

    maxNumberOfNodes, nodesList, networkAdjMatrix, networkCommunities = readNetworkFile(
        networkFile, isNetworkDirected)

    newFile = networkFile+".with_motifs"
    copyfile(networkFile, newFile)

    # ADD MOTIFS TO NETWORK
    with open(newFile, "a") as fd:
        for i in range(numberOfMotifs):
            nodes = []
            # choose nodes that obey the rules
            for j in range(numberOfNodes):
                finished = False
                while not finished:
                    number = randint(0, len(nodesList) - 1)
                    if nodesList[number] not in nodes:
                        nodes.append(nodesList[number])
                        good_node = (not new_node_has_forbidden_edges(networkAdjMatrix, motifAdjMatrix, nodes)
                                     and not new_node_has_forbidden_community(networkCommunities, motifCommunities, nodes))
                        if good_node:
                            finished = True
                        else:
                            nodes.pop()
            print "New motif, nodes:", nodes
            for pair in motifAdjList:
                firstNode = nodes[pair[0]]
                secondNode = nodes[pair[1]]
                # Only add edge if it did not exist yet
                if networkAdjMatrix[firstNode][secondNode] != 1:
                    print "Added an edge", firstNode+1, secondNode+1
                    # add one because of the subtraction
                    fd.write(str(firstNode+1)+" "+str(secondNode+1)+"\n")
                    networkAdjMatrix[firstNode][secondNode] = 1

    with open(newFile+".real_communities", "w") as fd:
        for community in networkCommunities:
            fd.write(str(community)+"\n")


def parseArgs():
    if len(sys.argv) <= 4:
        print "usage: python add-motifs-to-network.py -m <motif-file> -n <network-file> -nm <number-of-motifs> -s 3"

    i = 0
    motifFile = ""
    networkFile = ""
    numberOfMotifs = 0
    isNetworkDirected = False
    seedNumber = None
    while i < len(sys.argv):
        if(sys.argv[i] == "-m"):
            i += 1
            motifFile = sys.argv[i]

        elif sys.argv[i] == "-n":
            i += 1
            networkFile = sys.argv[i]

        elif sys.argv[i] == "-nm":
            i += 1
            numberOfMotifs = int(sys.argv[i])
        elif sys.argv[i] == "-d" or sys.argv[i] == "--directed":
            isNetworkDirected = True
        elif sys.argv[i] == "-s" or sys.argv[i] == "--seed":
            i += 1
            seedNumber = int(sys.argv[i])
        i += 1

    # print "Motif file", motifFile
    # print "Network file", networkFile
    # print "Number of motifs", numberOfMotifs
    # print "Network is directed?", isNetworkDirected

    return (motifFile, networkFile, numberOfMotifs, isNetworkDirected, seedNumber)


def main():
    motifFile, networkFile, numberOfMotifs, isNetworkDirected, seedNumber = parseArgs()
    if seedNumber != None:
        print "Using seed", seedNumber
        seed(seedNumber)

    addMotifsToNetwork(networkFile, motifFile,
                       numberOfMotifs, isNetworkDirected)
    # readNetworkFile(networkFile, isNetworkDirected)


if __name__ == '__main__':
    main()
    print "Done."
