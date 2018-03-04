# MotifModularity

Calculates Motif Modularity. 

Features:
  * Read graph from file
    - supply as the first argument to the program the path to a graph file
    - Graph File structure: series of lines, in each line `<NodeID> <NodeID> <ConnectionWeight>` 
    means the two nodes (NodeID is a number from 1 to infinite) are connected. 
    So far the graphs are unweighted, so ConnectionWeight should be 1.
  * createRandomPartition(maxPartitions):
      - Creates a random partition for the graph, assigning partition [0, maxPartitions) to each node.
  * Read partition from file
    - Provide to the second program argument the path to the partition file
    - This file contains in each line the partitionID (Number from 0 to maxPartitions) for the NodeID of that line.
  * Triangle Modularity
    - Calculates the triangle modularity for a given graph and partition
  * cicleModularity(size)
    - Calculates the modularity for a motif that is a cicle of size `size` for a given graph and partition
  * createAllPartitions
    - Iterates through all possible partitions for the given graph and finds the partition with highest modularity.
  * singleNodeGreedyAlgorithm
    - A greedy algorithm for finding a good partition that maximizes (triangle) modularity
    - Starts with a random partition createRandomPartition(numberOfNodes/3)
    - Choses a random node and checks the modularity when assigning it to a different partition group.
    - Places the node at the partition which gives the highest modularity
    - Stops after not finding a new better partition for X consecutive times

