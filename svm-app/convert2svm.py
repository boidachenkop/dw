import sys

# filepath - path to file, your cap
# sep - feature separator
# dec_sep - decimal separator in float
def convert(filepath, outfile, sep, dec_sep, label_first):
    try:
        output = open(outfile, "w")
    except IOError:
        output = open(outfile, "x")
    with open(filepath, "r", encoding='utf-8-sig') as input_file:
        for line in input_file:
            out = []
            line = line.split(sep)
            if label_first == "1":
                out.append(line[0].strip())
                start_range=1
                end_range=len(line)
            else:
                out.append(line[-1].strip())
                start_range=0
                end_range=len(line)-1

            feature_counter=1
            for i in range(start_range, end_range):
                out.append((str(feature_counter) + ":" + line[i].strip()).replace(dec_sep,"."))
                feature_counter+=1 
            line = out
            line = " ".join(line)
            output.write(line+'\n')
            

if __name__ == "__main__":
    if len(sys.argv) == 6:
        if sys.argv[2] == sys.argv[3]:
            print("Feature separator and double ")
        convert(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
    else:
        print("Usage:"+sys.argv[0]+" filepath outfile sep dec_sep label_first(0 or 1)")
    
