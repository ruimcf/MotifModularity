import sys
from subprocess import call


def parseArgs():
    if len(sys.argv) <= 2:
        print "usage: pyhton rangeModularity <min-communities> <max-communities>"
        return

    startCommunities = int(sys.argv[1])
    endCommunities = int(sys.argv[2])

    if endCommunities < startCommunities:
        print "The end interval cannot be less than the start."
        return

    # networkFile = "networks/zachary-karate.txt"
    networkFile = "networks/tripartite-demo.txt"
    seed = str(18)
    # motifFile = "motifs/triangle.motif"
    motifFile = "motifs/tripartite_4_nodes.motif"
    directedOrUndirected = "-ud"
    weightedOrUnweighted = "-uw"

    for i in range(startCommunities, endCommunities+1):
        call(["/Users/rui.fonseca/Workspace/Tese/MotifModularity/motifModularity", "-n", networkFile,
              weightedOrUnweighted, directedOrUndirected, "-s", seed, "-m", motifFile, "-nc", str(i)])


parseArgs()
print "Done."
