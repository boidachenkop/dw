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
    static int runHoldout(int type, QString dataset_filepath, int test_part_percent, QString train_part, QString test_part);
    static int runConvert2SvmFormat(QString filepath, QString outfilepath, QString sep, QString dec_sep, bool label_first);
    static int runGetDencityColumn(QString filepath, QString outfilepath);
    static int runGridSearch(QString filepath, QString log2c, QString log2g, QString cv_nfold, bool animation);

};

#endif // SCRIPTQTMANAGER_H
