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

        # make sure all the motif nodes are on the same community
        for community in motifCommunities:
            if community != motifCommunities[0]:
                print "Motif does not have the same communities for all nodes!!!"
                return

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


def applyPartitionMap(networkFile, motifFile, partitionMap, isNetworkDirected):
    motifAdjList, motifAdjMatrix, motifCommunities, numberOfNodes = readMotifFile(
        motifFile)

    maxNumberOfNodes, nodesList, networkAdjMatrix, networkCommunities = readNetworkFile(
        networkFile, isNetworkDirected)

    newFile = networkFile+".with_motifs"
    copyfile(networkFile, newFile)

    # check if number of nodes fit in network
    count = 0
    numberOfMotifs = 0
    for partitionData in partitionMap:
        count += partitionData.get("numberOfNodes")
        numberOfMotifs += partitionData.get("numberOfMotifs")
    print "number of motifs", numberOfMotifs
    if count > len(nodesList):
        print "Too many nodes in partitions for the network, ", str(count), ">", str(len(
            nodesList))
        return

    # apply the partition communities to the network
    nodesForEachCommunity = {}
    remainingNodes = list(nodesList)
    for partitionData in partitionMap:
        partitionId = partitionData.get("partitionId")
        nodesForEachCommunity[partitionId] = []
        for node in range(partitionData.get("numberOfNodes")):
            randomPos = randint(0, len(remainingNodes) - 1)
            chosenNode = remainingNodes.pop(randomPos)
            networkCommunities[chosenNode] = partitionId
            nodesForEachCommunity[partitionId].append(chosenNode)

    # ADD MOTIFS TO NETWORK
    with open(newFile, "a") as fd:
        for partitionData in partitionMap:
            for i in range(partitionData.get("numberOfMotifs")):
                nodes = []
                # choose nodes that obey the rules
                availableNodes = list(
                    nodesForEachCommunity[partitionData.get("partitionId")])
                for j in range(numberOfNodes):
                    finished = False
                    while not finished:
                        number = randint(0, len(availableNodes) - 1)
                        if nodesInCommunity[number] not in nodes:
                            nodes.append(nodesInCommunity[number])
                            if not new_node_has_forbidden_edges(networkAdjMatrix, motifAdjMatrix, nodes):
                                finished = True
                            else:
                                nodes.pop()

                # might chose a motif that already exists, and might screw one that already exists!
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
                        if not isNetworkDirected:
                            networkAdjMatrix[secondNode][firstNode] = 1

    with open(newFile+".real_communities", "w") as fd:
        print "Communities"
        a = ""
        i = 0
        for community in networkCommunities:
            a += str(i)+":"+str(community)+" "
            i += 1
            fd.write(str(community)+"\n")
        print a


def readPartitionMap(partitionMapFile):
    with open(partitionMapFile) as fd:
        partitionMap = []
        next(fd)  # Read header
        for line in fd:
            data = line.split()
            partitionMap.append(
                {"partitionId": int(data[0]), "numberOfNodes": int(data[1]), "numberOfMotifs": int(data[2])})
        return partitionMap


def parseArgs():
    if len(sys.argv) <= 4:
        print "usage: python add-motifs-to-network.py -m <motif-file> -n <network-file> -nm <number-of-motifs> -s 3"

    i = 0
    motifFile = ""
    networkFile = ""
    partitionMapFile = ""
    isNetworkDirected = False
    seedNumber = None
    while i < len(sys.argv):
        if(sys.argv[i] == "-m"):
            i += 1
            motifFile = sys.argv[i]

        elif sys.argv[i] == "-n":
            i += 1
            networkFile = sys.argv[i]

        elif sys.argv[i] == "-p":
            i += 1
            partitionMapFile = sys.argv[i]
        elif sys.argv[i] == "-d" or sys.argv[i] == "--directed":
            isNetworkDirected = True
        elif sys.argv[i] == "-s" or sys.argv[i] == "--seed":
            i += 1
            seedNumber = int(sys.argv[i])
        i += 1

    return (motifFile, networkFile, partitionMapFile, isNetworkDirected, seedNumber)


def main():
    motifFile, networkFile, partitionMapFile, isNetworkDirected, seedNumber = parseArgs()
    if seedNumber != None:
        print "Using seed", seedNumber
        seed(seedNumber)

    partitionMap = readPartitionMap(partitionMapFile)

    applyPartitionMap(networkFile, motifFile,
                      partitionMap, isNetworkDirected)


if __name__ == '__main__':
    main()
    print "Done."
