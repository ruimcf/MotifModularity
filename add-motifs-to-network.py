import sys


def addMotifsToNetwork(networkFile, motifFile, numberOfMotifs, isNetworkDirected):
    with open(motifFile, "r") as mf:
        numberOfNodes = int(next(mf))
        direction = next(mf)
        isDirected = False
        if direction == "directed":
            isDirected = True
        motifCommunities = [int(community) for community in next(mf).split()]
        print("communities", motifCommunities)

        # MOTIF ADJ LIST & MATRIX
        motifAdjList = []
        motifAdjMatrix[[0] * numberOfNodes for k in range(numberOfNodes)]
        for i in range(0, numberOfNodes):
            pair = next(mf).split()
            pair[0] = int(pair[0]) - 1    # subtract one
            pair[1] = int(pair[1]) - 1    # to match indexes
            motifAdjList.append(pair)
            motifAdjMatrix[pair[0]][pair[1]] = 1
            if not isDirected:
                motifAdjMatrix[pair[1]][pair[0]] = 1

        # NETWORK NODES LIST
        maxNumberOfNodes = -1
        nodesList = []
        with open(networkFile, "r") as fd:
            for line in fd:
                pair = line.split()
                firstNode = int(pair[0]) - 1    # subtract one
                secondNode = int(pair[1]) - 1   # to match indexes
                nodesList.append(firstNode)
                nodesList.append(secondNode)
                if firstNode > maxNumberOfNodes:
                    maxNumberOfNodes = firstNode
                if secondNode > maxNumberOfNodes:
                    maxNumberOfNodes = secondNode
        maxNumberOfNodes += 1  # account for the subtraction

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

        # ADD MOTIFS TO NETWORK
        with open(networkFile, "a") as fd:
            for i in range(0, numberOfMotifs):
                nodes = []
                # choose nodes that obey the rules
                for j in range(0, numberOfNodes):
                    finished = False
                    while not finished:
                        number = random.randint(0, len(nodesList))
                        if nodesList[number] not in nodes:
                            nodes.append(nodesList[number])
                            if not new_node_has_forbidden_edges(networkAdjMatrix, motifAdjMatrix, nodes):
                                finished = True
                            else:
                                nodes.pop()

                for pair in motifAdjList:
                    firstNode = nodes[pair[0]]
                    secondNode = nodes[pair[1]]
                    # Only add edge if it did not exist yet
                    if networkAdjMatrix[firstNode][secondNode] != 1:
                        fd.write(str(firstNode)+" "+str(secondNode)+"\n")
                        networkAdjMatrix[firstNode][secondNode] = 1
                for j in range(numberOfNodes):
                    if motifCommunities[j] != -1:
                        networkCommunities[nodesList[j]] = motifCommunities[j]


# nodes have to obey the edge rules
def new_node_has_forbidden_edges(networkAdjMatrix, motifAdjMatrix, chosenNodes):
    new_node = chosenNodes[-1]
    new_node_pos = len(chosenNodes) - 1

    for i in range(len(chosenNodes) - 1):
        # not supposed to have an edge
        if motifAdjMatrix[i][new_node_pos] == 0:
            if networkAdjMatrix[chosenNodes[i]][chosenNodes[new_node_pos]] == 1:
                return true
        if motifAdjMatrix[new_node_pos][i] == 0:
            if networkAdjMatrix[chosenNodes[new_node_pos]][chosenNodes[i]] == 1:
                return true


def new_node_has_forbidden_community(networkCommunities, motifCommunities, chosenNodes):
    new_node_pos = len(chosenNodes) - 1
    if motifCommunities[new_node_pos] == -1:
        return False
    for i in range(new_node_pos):
        if motifCommunities[i] == -1:
            continue
        if motifCommunities[i] == motifCommunities[new_node_pos]:
            if networkCommunities[chosenNodes[i]] == -1 or networkCommunities[chosenNodes[new_node_pos]] == -1:
                # redefine communities?


def main():
    if len(sys.argv) <= 4:
        print "usage: python add-motifs-to-network.py -m <motif-file> -n <network-file> -nm <number-of-motifs>"

    i = 0
    motifFile = ""
    networkFile = ""
    numberOfMotifs = 0
    isNetworkDirected = false
    while i < len(sys.argv):
        if(sys.argv[i] == "-m"):
            i += 1
            motifFile = sys.argv[i]

        elif sys.argv[i] == "-n":
            i += 1
            networkFile = sys.argv[i]

        elif sys.argv[i] == "-nc":
            i += 1
            numberOfMotifs = int(sys.argv[i])
        elif sys.argv[i] == "-d":
            isNetworkDirected = true
        i += 1

    print "Motif file", motifFile
    print "Network file", networkFile
    print "Number of motifs", numberOfMotifs
    print "Network is directed?", isNetworkDirected


if __name__ == '__main__':
    main()
