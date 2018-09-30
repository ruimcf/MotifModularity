import sys
from subprocess import call


def parseArgs():
    if len(sys.argv) <= 1:
        print "usage: pyhton rangeModularity <network-file>"
        return

    seedList = [3915, 6058, 7494, 8024, 6453, 9705, 9597, 4777, 1277, 5521]
    networkFile = sys.argv[1]
    # networkFile = "networks/tripartite-demo.txt"
    # motifFile = "motifs/triangle.motif"
    # motifFile = "motifs/tripartite_3_nodes.motif"
    motifFile = "motifs/path_3_nodes_directed.motif"
    directedOrUndirected = "-d"
    weightedOrUnweighted = "-uw"
    numberCommunities = "3"

    for seed in seedList:
        call(["/Users/rui.fonseca/Workspace/Tese/MotifModularity/motifModularity", "-n", networkFile,
              weightedOrUnweighted, directedOrUndirected, "-s", str(seed), "-m", motifFile, "-nc", numberCommunities])

    for seed in seedList:
        call(["/Users/rui.fonseca/Workspace/Tese/MotifModularity/motifModularity", "-n", networkFile,
              weightedOrUnweighted, directedOrUndirected, "-s", str(seed), "-m", motifFile, "-nc", numberCommunities, "--not-optimized"])


parseArgs()
print "Done."
