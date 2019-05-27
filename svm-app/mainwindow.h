#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QScrollBar>
#include "svmcontroller.h"


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


private:
    Ui::MainWindow *ui;
    SVMController *svm;

    QString train_file_path;
    QString test_file_path;
};

#endif // MAINWINDOW_H
