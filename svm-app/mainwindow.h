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
#include "availabilityhandler.h"
#include "scriptqtmanager.h"
#include "filemanager.h"
#include "outputhandler.h"

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
    void on_output_textEdit_textChanged();

    void updateNFeaturesLabel(int n_features);
    void updateNRowsLabel(int n_rows);
    void updateNClassesLabel(int n_classes);
    void updateFSLineEdit(int n_features);
    void updateTrainFilepathLabel(QString filepath, bool correct);
    void updateTestFilepathLabel(QString filepath, bool correct);
    void updateModelFilepathLabel(QString filepath);

private:
    int parseParameters();
    void setDefaultSVMParams();

    Ui::MainWindow *ui;
    SVMController *svm;

    AvailabilityHandler *availability_handler;
    FileManager *file_manager;
    OutputHandler *output_handler;


    std::string rm_tmp_plot_cmd{"rm -f "};
};

#endif // MAINWINDOW_H
