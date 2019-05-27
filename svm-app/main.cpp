#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "svmcontroller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();

//    SVMController c;
//    c.openTrainFile("/home/pavlo/Desktop/pr/dw/datasets/svmguide1.txt.scale");
//    c.openPredictInputFile("/home/pavlo/Desktop/pr/dw/datasets/svmguide1.t.scale");
//    c.openPredictOutputFile("/home/pavlo/Desktop/pr/dw/svmguide1.test");
//    c.setPredictPrintFunction(&printf);
//    c.readProblem();
//    c.trainModel();
//    c.predict();

//    c.openPredictInputFile("/home/pavlo/Desktop/pr/dw/datasets/svmguide1.t.scale");
//    c.openPredictOutputFile("/home/pavlo/Desktop/pr/dw/svmguide1.test");
//    c.setModelFilePath("./model2");
//    c.setC(2.0).setGamma(2.0);
//    c.trainModel();
//    c.predict();
//    return 0;
}
