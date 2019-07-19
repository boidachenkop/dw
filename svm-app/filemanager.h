#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <fstream>
#include <iostream>


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
    int getNFeatures();
private:
    int getNFeatures(QString);

    QString _train_input_filepath{};
    QString _test_input_filepath{};
    QString _model_filepath{};

    QLabel* _train_filepath_label;
    QLabel* _test_filepath_label;
    QLabel* _model_filepath_label;

    QLineEdit* _fs_lineEdit;

    int _n_features{-1};
    int _test_n_features{-1};
};

#endif // FILEMANAGER_H
