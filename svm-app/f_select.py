import sys

def getRange(pattern):
    #parse pattern and generate list "1-3, 4, 6-8" -> [1, 2, 3, 4, 6, 7, 8]
    to_select = []
    ranges = [item.lstrip() for item in pattern.split(',')]
    for item in ranges:
        nfrom = []
        nto = []
        now_from = True
        for c in item:
            if now_from and c.isdigit():
                nfrom += c
            elif (not now_from) and c.isdigit():
                nto += c
            elif c == '-':
                now_from = False

        if len(nto) == 0:
            to_select.append(int("".join(nfrom)))
        else:
            f = int("".join(nfrom)); t = int("".join(nto))
            if t < f:
                print("wrong range from: "+str(f)+" to: "+str(t))
                sys.exit(1)
            to_select += list(range(f, t+1))
    return to_select


def select(file_path, n_feature, f_to_select):
    # select 'f_to_select' features from 'file_path'
    to_select = getRange(f_to_select)
    for item in to_select:
        if item > n_feature:
            print("Selected feature is out of range: "+str(item))
            sys.exit(1)
    to_remove = []
    for i in range(1, int(n_feature)+1): #reverse
        if i not in to_select:
            to_remove.append(i)

    input_file = open(file_path, 'r')
    records = []
    with open(file_path, 'r') as input_file:
        for line in input_file:
            line = line.split() 
            record = [None]*(n_feature+1)
            record[0] = line[0]
            for i in range(1, n_feature+1):
                feature = line[i].split(':') 
                record[int(feature[0])] = feature[1]
            records.append(record)

    for record in records: # delete columns
        for i in to_remove:
            record[i] = None
        tmp = list(filter(lambda x: x!=None, record))
        record.clear()
        record.extend(tmp)

    with open(file_path+".fselected", "w") as output_file:
        for record in records:
            output = [None]*len(record)
            output[0] = record[0]
            for i in range(1, len(record)):
                output[i] = str(i)+":"+str(record[i])
            output_file.write(" ".join(output)+"\n")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage %s file number_of_features selected_features" % (sys.argv[0]))
        sys.exit(1)
    else:
        select(sys.argv[1], int(sys.argv[2]), sys.argv[3])
