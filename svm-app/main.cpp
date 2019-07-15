#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    FILE* file;
    file = fopen("tstfile.txt", "w");
    double dbl = 0.5876;
    fprintf(file, "%.17f", dbl);
    fclose(file);
    return a.exec();
}
