#include <QStringList>
#include <QProcess>
#include <QCoreApplication>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "loadingindicator.h"
#include "scriptqtmanager.h"

int ScriptQtManager::runCheckData(QString filepath)
{
    QProcess py_script;
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/checkdata.py"<<filepath;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    if(py_script_out == "No error.\n"){
        return 0;
    }else{
        printf("%s\n", py_script_out.toLatin1().data());
        fflush(stdout);
        return -1;
    }
}

void ScriptQtManager::runFeatureSelection(QString filepath, int n_features, QString pattern)
{
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/f_select.py"<<filepath<<QString::number(n_features)<<pattern;
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
}

int ScriptQtManager::runPlot(FileManager* file_manager, bool density, bool model, double band_width)
{
    QString filepath = file_manager->getTrainFilepath();
    int n_features = file_manager->getNFeatures();
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/plotter.py"<<filepath<<"-nf"<<QString::number(n_features)
       <<"-bandwidth"<<QString::number(band_width);
    if(n_features == 1){
        if(density){
            args<<"-dencity";
        }
    }else if(n_features == 2){
        if(density){
            args<<"-dencity";
        }
        if(model){
            args<<"-model"<<file_manager->getModelFilepath();
        }
    }else if(n_features == 3){

    }
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished(-1);
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
    return py_script.exitCode();
}

int ScriptQtManager::runHoldout(int type, QString dataset_filepath, int test_part_percent, QString train_part, QString test_part)
{
    QStringList args;
    if(type == 0){ // classification
        args<<QCoreApplication::applicationDirPath()+"/h4c.py"<<dataset_filepath<<
              QString::number(test_part_percent)<<train_part<<test_part;
    }else if(type == 1){ // regression
        args<<QCoreApplication::applicationDirPath()+"/h4r.py"<<dataset_filepath<<
              QString::number(test_part_percent)<<train_part<<test_part;
    }
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
    return py_script.exitCode();
}

int ScriptQtManager::runConvert2SvmFormat(QString filepath, QString outfilepath, QString sep, QString dec_sep, bool label_first)
{
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/convert2svm.py"<<filepath<<outfilepath<<
          sep<<dec_sep<<(label_first ? "1" : "0");
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
    return py_script.exitCode();
}

void ScriptQtManager::runGridSearch(QString filepath, QString log2c, QString log2g, QString cv_nfold, bool animation)
{
    QStringList args;
    QProcess py_script;
    args<<QCoreApplication::applicationDirPath()+"/grid.py"<<"-out"<<"null"<<"-log2c"<<log2c<<"-log2g"<<log2g<<"-v"<<cv_nfold;
    if(!animation){
        args<<"-gnuplot"<<"null";
    }
    args<<filepath;
    py_script.setProcessChannelMode(QProcess::ForwardedOutputChannel);
//    py_script.startDetached("python3", args);
    py_script.start("python3", args);
    py_script.waitForFinished(60000);
}

int ScriptQtManager::runGenerateData(QString filepath)
{
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/pointsgen.py"<<filepath;
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
    return py_script.exitCode();
}







