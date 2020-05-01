import matplotlib.pyplot as plt
from matplotlib.widgets import Button
import argparse

class PointGen:
    def __init__(self, filename):
        self.__filename = filename
        self.__points = {"-1":[], "1":[]}
        self.__color = ["red","blue"]
        self.__first_group = True
        self.__fig, self.__ax_plot = plt.subplots()
        plt.subplots_adjust(bottom=0.2)
        self.__ax_plot.set_xlim([-1, 1])
        self.__ax_plot.set_ylim([-1, 1])
        self.__cid = self.__fig.canvas.mpl_connect('button_press_event', self.onclick)

        self.__ax_button = plt.axes([0.75, 0.05, 0.17, 0.075])
        self.__button_change_group = Button(self.__ax_button, 'Change Group')
        self.__button_change_group.on_clicked(self.changeGroup)
        (self.__xmc,self.__ymc),(self.__xMc,self.__yMc) = self.__button_change_group.label.clipbox.get_points()
        # self.__ax_button = plt.axes([0.55, 0.05, 0.17, 0.075])
        # self.__button_save = Button(self.__ax_button, 'Save Data')
        # self.__button_change_group.on_clicked(self.save)
        # (self.__xms,self.__yms),(self.__xMs,self.__yMs) = self.__button_save.label.clipbox.get_points()

    def onclick(self, event):
        if event.xdata != None and event.ydata != None:
            # if click not in axes, prevent capture onclick event when clicking on button
            if not (self.__xmc<event.x<self.__xMc and self.__ymc<event.y<self.__yMc): 
                    # and not (self.__xms<event.x<self.__xMs and self.__yms<event.y<self.__yMs):
                if self.__first_group:
                    self.__ax_plot.plot(event.xdata, event.ydata, '.', markersize=9, color="red")
                    self.__points["-1"].append([event.xdata, event.ydata])
                else:
                    self.__ax_plot.plot(event.xdata, event.ydata, '.', markersize=9, color="blue")
                    self.__points["1"].append([event.xdata, event.ydata])
                self.__fig.canvas.draw()

    def save(self):
        with open(self.__filename, "w") as output:
            for key in self.__points.keys():
                for item in self.__points[key]:
                    output.write(key + " 1:" + str(item[0]) + " 2:" + str(item[1]) + '\n')

    def changeGroup(self, event):
        self.__first_group = not self.__first_group

    def plot(self):
        plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Click on canvas, generate data ourself")
    parser.add_argument("filename", help="file to save generated data", type=str)
    args = parser.parse_args()
    p = PointGen(args.filename)
    p.plot()
    p.save()
