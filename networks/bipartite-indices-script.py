import sys
def increment_indices():
  if len(sys.argv) <= 1:
    print "usage: python bipartite.py <file-path>"
    return

  filename = sys.argv[1]
  with open(filename) as f:
    numberNodesFirstColumn = int(next(f).split()[-2])
    with open(filename+".bip_fixed", "w") as nf:
      for line in f:
        number1, number2 = line.split()
        fixed_number2 = str(int(number2) + numberNodesFirstColumn)
        nf.write(number1+" "+fixed_number2+"\n")

increment_indices()
print "Done."