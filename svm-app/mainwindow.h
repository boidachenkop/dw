#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "svmcontroller.h"
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
    void on_modelFile_toolButton_clicked();
    void on_scale_toolButton_clicked();
    void on_y_scale_checkBox_toggled(bool checked);
    void on_visualize_pushButton_clicked();
    void on_select_pushButton_clicked();
    void on_output_textEdit_textChanged();
    void on_cvType_comboBox_currentIndexChanged(int index);

    void updateTrainFilepathLabel(QString filepath, bool correct);
    void updateTestFilepathLabel(QString filepath, bool correct);
    void updateModelFilepathLabel(QString filepath);
    void updateOutputTextEdit(QString text);

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
