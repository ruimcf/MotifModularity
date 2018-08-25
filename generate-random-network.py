import sys
import random


def createNetwork(numberOfNodes, probabilityOfEdge, isDirected):
    with open("networks/generated_network.txt", 'w') as fd:
        if not isDirected:
            for i in range(1, numberOfNodes+1):
                for j in range(i+1, numberOfNodes+1):
                    randomNumber = random.random()
                    if randomNumber <= probabilityOfEdge:
                        fd.write(str(i)+"\t"+str(j)+"\n")
        else:
            for i in range(1, numberOfNodes+1):
                for j in range(1, numberOfNodes+1):
                    if i != j:
                        randomNumber = random.random()
                        if randomNumber <= probabilityOfEdge:
                            fd.write(str(i)+"\t"+str(j)+"\n")


def parseArgs():
    if len(sys.argv) < 3:
        print "usage: python generate-random-network -n <num-nodes> -p <prob-edge> [-d | --directed | -u | --undirected]"

    isDirected = False
    numberOfNodes = 0
    probabilityOfEdge = 0
    i = 1
    while i < len(sys.argv):
        if(sys.argv[i] == "-d" or sys.argv[i] == "--directed"):
            isDirected = True

        elif (sys.argv[i] == "-u" or sys.argv[i] == "--undirected"):
            isDirected = False

        elif sys.argv[i] == "-n":
            i = i+1
            if i >= len(sys.argv):
                print "Insuficient arguments '-n <num_nodes>'"
                return
            numberOfNodes = int(sys.argv[i])

        elif sys.argv[i] == "-p":
            i = i+1
            if i >= len(sys.argv):
                print "Insuficient arguments '-p <prob_edge>'"
                return
            probabilityOfEdge = float(sys.argv[i])

        i += 1

    print "Num nodes", numberOfNodes
    print "Probability", probabilityOfEdge
    if probabilityOfEdge < 0 or probabilityOfEdge > 1:
        print "Use probability between 0 and 1!"
        return

    createNetwork(numberOfNodes, probabilityOfEdge, isDirected)


parseArgs()
print "Done."
