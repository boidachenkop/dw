#ifndef SCRIPTQTMANAGER_H
#define SCRIPTQTMANAGER_H
#include <QString>
#include <vector>
#include <string>

#include "filemanager.h"

class ScriptQtManager
{
public:
    static int runCheckData(QString filepath);
    static void runFeatureSelection(QString filepath, int n_features, QString pattern);
    static std::string runPlot(FileManager* file_manager, bool density, double band_width);
};

#endif // SCRIPTQTMANAGER_H
