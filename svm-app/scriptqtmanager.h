#ifndef SCRIPTQTMANAGER_H
#define SCRIPTQTMANAGER_H
#include <QString>
#include <QStringList>
#include <QProcess>

#include <stdlib.h>


class ScriptQtManager
{
public:
    ScriptQtManager();
    int runCheckData(QString filepath);
    void runFeatureSelection(QString filepath, int n_features, QString pattern);
    std::string runPlot(QString filepath, int n_features);
private:
    QString python{"python3 "};
};

#endif // SCRIPTQTMANAGER_H
