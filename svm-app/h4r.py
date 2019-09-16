import random
import sys

def getLabel(line):
    tokens = line.split()
    return tokens[0]

def holdout(dataset, test_percent, train_part, test_part):
    data = [ (random.random(), line) for line in dataset ]
    data.sort()
    datalen = len(data)
    trainsize = datalen - round((test_percent/100)*datalen)
    i=1
    for _, line in data:
        if i < trainsize:
            i+=1
            if train_part != None:
                train_part.write( line )
        else:
            if test_part != None:
                test_part.write( line )

    if train_part != None:
        print("Train part will be ready")
        train_part.close()

    if test_part != None:
        print("Test part will be ready")
        test_part.close()



if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage %s dataset train_part_percentage train_part_save_to test_part_save_to" % (sys.argv[0]))
        sys.exit(1)
    else:
        try:
            test_percent = float(sys.argv[2])
        except ValueError:
            print("Can't convert train_part_percentage to float. Exit 1")
            sys.exit(1)
        try:
            dataset = open(sys.argv[1], "r");
        except IOError:
            print("Can't open dataset file. Exit 1")
            sys.exit(1)
            
        if sys.argv[3] != "null":
            try:
                tr_part = open(sys.argv[3], "w");
            except IOError:
                tr_part = open(sys.argv[3], "x");
        else:
            tr_part = None

        if sys.argv[4] != "null":
            try:
                tst_part = open(sys.argv[4], "w");
            except IOError:
                tst_part = open(sys.argv[4], "x");
        else:
            tst_part = None
        holdout(dataset, test_percent, tr_part, tst_part)

