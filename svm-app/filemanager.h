#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <set>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string.h>

#include "scriptqtmanager.h"

class FileManager
{
public:
    FileManager(QLabel* train_label, QLabel* test_label,
                QLabel* model_label, QLineEdit* fs_lineEdit);
    int setTrainFilepath(QString);
    int setTestFilepath(QString);
    int setModelFilepath(QString);

    QString getTrainFilepath();
    QString getTestFilepath();
    QString getModelFilepath();
    std::vector<std::string> getLabels();
    int getNClasses();
    int getNFeatures();
    int getNLines();

private:
    int parseFile(QString);

    QString _train_input_filepath{};
    QString _test_input_filepath{};
    QString _model_filepath{};

    QLabel* _train_filepath_label;
    QLabel* _test_filepath_label;
    QLabel* _model_filepath_label;

    QLineEdit* _fs_lineEdit;

    int _n_features{-1};
    int _test_n_features{-1};
    int _n_lines{-1};

    std::set<std::string> _labels;
};

#endif // FILEMANAGER_H
