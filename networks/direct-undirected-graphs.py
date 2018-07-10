import sys
def duplicate_edges():
  if len(sys.argv) <= 1:
    print "usage: python direct-undirected-graphs.py <file-path>"
    return

  filename = sys.argv[1]
  with open(filename) as f:
    with open(filename+".directed", "w") as nf:
      for line in f:
        number1, number2 = line.split()
        nf.write(number1+" "+number2+"\n")
        nf.write(number2+" "+number1+"\n")

duplicate_edges()
print "Done."