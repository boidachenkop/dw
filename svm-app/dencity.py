import numpy as np
import scipy.stats as st
import sys

def getDensityColumn(inputfile, outputfile):
    x = []
    y = []

    with open(inputfile, "r") as input:
        lines = input.readlines()
        x = [0]*len(lines)
        y = [0]*len(lines)
        i = 0
        for line in lines:
            line = line.split()
            for item in line:
                if "1:" in item:
                    x[i] = float(item.split(":")[1])
                elif "2:" in item:
                    y[i] = float(item.split(":")[1])
            i+=1

    # Create meshgrid
    values = np.vstack([x, y])
    z = st.gaussian_kde(values).evaluate(values)

    with open(outputfile, "w") as output:
        for i in range(0, len(x)):
            output.write(str(x[i])+" "+str(y[i])+ " "+str(z[i])+"\n")

if __name__ == "__main__":
    if len(sys.argv) == 3:
        getDensityColumn(sys.argv[1], sys.argv[2])
    else:
        print("Usage ./%s inputfile outputfile" % sys.argv[0])
