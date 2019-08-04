import random
import sys

def getLabel(line):
    tokens = line.split()
    return tokens[0]

def holdout(dataset, test_percent, train_part, test_part):
    linenum=1;
    for_train = {}
    for_test = {}
    lines = dataset.readlines()
    for line in lines:
        label = getLabel(line)
        if not label in for_train:
            for_train[label] = set()
        for_train[label].update([linenum])
        linenum+=1
    for label in for_train.keys():
        if not label in for_test:
            for_test[label] = set()
        for_test[label] = set(random.sample(for_train[label],
                round((test_percent/100)*len(for_train[label]))))
        for_train[label].symmetric_difference_update(for_test[label])
    # write training part
    for label in for_train.keys():
        for n in for_train[label]:
            train_part.write(lines[n-1])
    train_part.close()
    # write testing part
    for label in for_test.keys():
        for n in for_test[label]:
            test_part.write(lines[n-1])
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
        try:
            tr_part = open(sys.argv[3], "w");
        except IOError:
            tr_part = open(sys.argv[3], "x");
        try:
            tst_part = open(sys.argv[4], "w");
        except IOError:
            tst_part = open(sys.argv[4], "x");
        holdout(dataset, test_percent, tr_part, tst_part)

