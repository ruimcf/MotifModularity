import sys
from subprocess import call


def parseArgs():
    if len(sys.argv) <= 2:
        print "usage: pyhton rangeModularity <network-file>"
        return

    seedList = [3915, 6058, 7494, 8024, 6453, 9705, 9597, 4777, 1277, 5521]
    networkFile = sys.argv[1]
    # networkFile = "networks/tripartite-demo.txt"
    motifFile = "motifs/triangle.motif"
    # motifFile = "motifs/tripartite_4_nodes.motif"
    directedOrUndirected = "-ud"
    weightedOrUnweighted = "-uw"
    numberCommunities = 2

    for i in seedList:
        call(["/Users/rui.fonseca/Workspace/Tese/MotifModularity/motifModularity", "-n", networkFile,
              weightedOrUnweighted, directedOrUndirected, "-s", seed, "-m", motifFile, "-nc", numberCommunities])

    for i in seedList:
        call(["/Users/rui.fonseca/Workspace/Tese/MotifModularity/motifModularity", "-n", networkFile,
              weightedOrUnweighted, directedOrUndirected, "-s", seed, "-m", motifFile, "-nc", numberCommunities, "--not-optimized"])


parseArgs()
print "Done."
