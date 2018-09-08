import sys


def create_tsv():
    if len(sys.argv) <= 1:
        print "usage: python edge-list-to-tsv.py <file-path>"
        return

    filename = sys.argv[1]
    with open(filename) as f:
        with open(filename+".tsv", "w") as nf:
            nf.write("Source\tTarget\n")
            for line in f:
                number1, number2 = line.split()
                nf.write(number1+"\t"+number2+"\n")


def main():
    create_tsv()
    print "Done."


if __name__ == '__main__':
    main()
