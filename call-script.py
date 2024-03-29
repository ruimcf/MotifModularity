import sys
import os
from subprocess import call


def parseArgs():
    if len(sys.argv) <= 1:
        print "usage: pyhton rangeModularity <network-file>"
        return

    seedList = [3915, 6058, 7494, 8024, 6453, 9705, 9597, 4777, 1277, 5521]
    networkFile = sys.argv[1]
    # networkFile = "networks/tripartite-demo.txt"
    motifFile = "motifs/triangle.motif"
    # motifFile = "motifs/tripartite_4_nodes.motif"
    directedOrUndirected = "-ud"
    weightedOrUnweighted = "-uw"
    numberCommunities = "2"

    for seed in seedList:
        call([os.getcwd() + "/motifModularity", "-n", networkFile,
              weightedOrUnweighted, directedOrUndirected, "-s", str(seed), "-m", motifFile, "-nc", numberCommunities])

    # for seed in seedList:
    #     call([os.getcwd() + "/motifModularity", "-n", networkFile,
    #           weightedOrUnweighted, directedOrUndirected, "-s", str(seed), "-m", motifFile, "-nc", numberCommunities, "--not-optimized"])


parseArgs()
print "Done."
