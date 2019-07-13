#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QScrollBar>
#include <QTextStream>
#include <QFileDialog>
#include <QStringList>
#include <QProcess>
#include <QByteArray>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstdio>

#include "svmcontroller.h"
#include "svmscale.h"

int getNFeatures(std::string);

namespace Ui {
class MainWindow;
}

struct Output{
    static QString out;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_chooseDataset_toolButton_clicked();

    void on_train_pushButton_clicked();

    void on_choose_tstFile_toolButton_clicked();

    void on_test_pushButton_clicked();


    void on_svmType_comboBox_currentIndexChanged(int index);

    void on_kernel_comboBox_currentIndexChanged(int index);

    void on_crossValidation_checkBox_toggled(bool checked);

    void on_modelFile_toolButton_clicked();

    void on_scale_toolButton_clicked();

    void on_y_scale_checkBox_toggled(bool checked);

    void on_visualize_pushButton_clicked();

    void on_select_pushButton_clicked();

private:
    void filterSVMTypeParams(int);
    void filterKernelParams(int);

    void updateOutput();

    Ui::MainWindow *ui;
    SVMController *svm;

    QString train_file_path;
    QString test_file_path;

    std::string stdout_redirect_path;
    FILE* std_old;

    std::string rm_tmp_plot_cmd{"rm -f "}; //command for remove tmp files using for plotting
};

#endif // MAINWINDOW_H
