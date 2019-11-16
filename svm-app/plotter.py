import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from scipy.stats import gaussian_kde
import argparse
import sys
import os

class LibsvmPlot:
    def __init__(self, filename, nfeatures, dencity=False, bandwidth=0.1,
            model_filename=""):
        self.__density = dencity
        self.__filename = filename
        self.__nfeatures = nfeatures
        self.__bandwidth = bandwidth
        if len(model_filename) != 0:
            self.__show_model = True
        else:
            self.__show_model = False
        self.__model_filename = model_filename
        if self.__show_model and len(self.__model_filename) == 0:
            print("Error: model file is not set")
            sys.exit(1)

        self.__points = {}
        self.__parse__()

    def __parse__(self):
        with open(self.__filename) as finput:
            for line in finput:
                point = []
                line = line.split();

                record = {}
                for i in range(1, len(line)):
                    label_feature = line[i].split(":")
                    if len(label_feature) != 2:
                        print("Error: wrong format: " + line[i])
                        return -1;
                    record[int(label_feature[0])] = float(label_feature[1])

                for i in range(1, self.__nfeatures+1):
                    if i in record.keys():
                        point.append(record[i])
                    else:
                        point.append(0)
                label = int(line[0])
                if label not in self.__points.keys():
                    self.__points[label] = []
                self.__points[label].append(point)
        self.__nclasses = len(self.__points.keys())

    def print(self):
        for key in self.__points.keys():
            for item in self.__points[key]:
                print(str(key) + " : " + str(item))
                
    def getNClasses(self):
        return self.__nclasses

    def getNFeatures(self):
        return self.__nfeatures

    def getFilename(self):
        return self.__filename

    def plot(self):
        if self.__nfeatures == 1:
            for key in self.__points.keys():
                x = [item[0] for item in self.__points[key]]
                y = [0]*len(self.__points[key])
                plt.scatter(x, y, s=2)
                if self.__density:
                    x.sort()
                    density = gaussian_kde(x)
                    plt.plot(x, density(x))
        elif self.__nfeatures == 2:
            x_all = []
            y_all = []
            x = []
            y = []
            for key in self.__points.keys():
                tmp_x = [item[0] for item in self.__points[key]]
                tmp_y =  [item[1] for item in self.__points[key]]
                x.append(tmp_x)
                y.append(tmp_y)
                x_all+=tmp_x
                y_all+=tmp_y
            x_min = min(x_all)
            x_max = max(x_all) 
            y_min = min(y_all)
            y_max = max(y_all)
            nbins = int((x_max-x_min)/self.__bandwidth)
            if self.__show_model:
                # generate grid
                xi, yi = np.mgrid[x_min:x_max:nbins*1j,
                        y_min:y_max:nbins*1j]
                with open("/tmp/svmapp_plot_pred_in", "w") as pred_in:
                    for i in range(len(xi)):
                        for j in range(len(xi)):
                            # for every record put first found label, that has no 
                            # effect on predicted labels
                            pred_in.write(str(list(self.__points.keys())[0]) + 
                                    " 1:"+str(xi[i][j]) + " 2:"+str(yi[i][j]) + "\n")
                # write data to file
                options = " -q " + "/tmp/svmapp_plot_pred_in " + self.__model_filename + " /tmp/svmapp_plot_pred_out"
                os.system("./svm-predict" + options)
                zi = []
                with open("/tmp/svmapp_plot_pred_out") as pred_out:
                    for line in pred_out:
                        zi.append(int(line.strip())) 
                plt.pcolormesh(xi, yi, np.array(zi).reshape(xi.shape), shading="gouraud", cmap=plt.cm.BuGn_r)
            if self.__density:
                k = gaussian_kde([x_all,y_all])
                xi, yi = np.mgrid[x_min:x_max:nbins*1j,
                        y_min:y_max:nbins*1j]
                zi = k(np.vstack([xi.flatten(), yi.flatten()]))
                plt.contour(xi, yi, zi.reshape(xi.shape) )
            for i in range(len(x)):
                plt.scatter(x[i], y[i], s=2)

        elif self.__nfeatures == 3:
            fig = plt.figure()
            ax = fig.add_subplot(111, projection='3d')
            for key in self.__points.keys():
                x = [item[0] for item in self.__points[key]]
                y = [item[1] for item in self.__points[key]]
                z = [item[2] for item in self.__points[key]]
                ax.scatter(x, y, z, s=2)
        else:
            print("Unsupported number of features");
            sys.exit(1)
        plt.show()


# def __init__(self, filename, nfeatures, dencity=False, bandwidth=0.1,
#         show_model=False, model_filename=""):

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot file")
    parser.add_argument("filename", help="path to file with data to plot", type=str);
    parser.add_argument("-nf", dest="number_of_features", help="number of features", type=int, required=True);
    parser.add_argument("-dencity", help="show dencity", action="store_true");
    parser.add_argument("-bandwidth", help="band size used in dencity and model plots", type=float, required=True)
    parser.add_argument("-model", dest="modelfilename", default="", help="show model", type=str);
    args = parser.parse_args()
    lp = LibsvmPlot(args.filename, args.number_of_features, args.dencity, args.bandwidth, args.modelfilename)
    lp.plot()



