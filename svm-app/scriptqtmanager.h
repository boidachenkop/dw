#ifndef SCRIPTQTMANAGER_H
#define SCRIPTQTMANAGER_H
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QCoreApplication>

#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>

class ScriptQtManager
{
public:
    static int runCheckData(QString filepath);
    static void runFeatureSelection(QString filepath, int n_features, QString pattern);
    static std::string runPlot(QString filepath, int n_features, std::vector<double> labels, bool density, double band_width);
};

#endif // SCRIPTQTMANAGER_H
