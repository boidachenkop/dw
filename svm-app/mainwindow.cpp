#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <cstdio>

QString Output::out = ""; // global output variable

void printToQString(const char* v){
    Output::out += QString::fromUtf8(v);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    svm(new SVMController())
{
    ui->setupUi(this);

    // train button unabled from begining
    ui->train_pushButton->setEnabled(false);
    ui->test_pushButton->setEnabled(false);
    // output textEdit read only
    ui->output_textEdit->setReadOnly(true);

    //set default params
    ui->svmType_comboBox->setCurrentIndex(svm->getParams().svm_type);
    ui->kernel_comboBox->setCurrentIndex(svm->getParams().kernel_type);

    ui->degree_spinBox->setValue(svm->getParams().degree);
    ui->gamma_doubleSpinBox->setValue(svm->getParams().gamma);
    ui->coef_doubleSpinBox->setValue(svm->getParams().coef0);

    ui->C_doubleSpinBox->setValue(svm->getParams().C);
    ui->eps_lineEdit->setText(QString::number(svm->getParams().eps));
    ui->nu_doubleSpinBox->setValue(svm->getParams().nu);

    ui->P_doubleSpinBox->setValue(svm->getParams().p);
    ui->shrinking_checkBox->setChecked(svm->getParams().shrinking);
    ui->prob_checkBox->setChecked(svm->getParams().probability);

    svm_set_print_string_function(printToQString); // print to Output::out

    //TODO: output textedit always bottom
//    ui->output_textEdit->
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_chooseDataset_toolButton_clicked()
{
    train_file_path = QFileDialog::getOpenFileName(this, "Choose dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    ui->chooseDataset_label->setText(train_file_path);
    ui->train_pushButton->setEnabled(true);
}

void MainWindow::on_tstFile_toolButton_clicked()
{
    test_file_path = QFileDialog::getOpenFileName(this, "Choose dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    ui->tstFile_path_label->setText(test_file_path);
    ui->test_pushButton->setEnabled(true);
}

void MainWindow::on_train_pushButton_clicked()
{
    bool ok=false;

    svm->openTrainFile(train_file_path.toStdString());
    svm_set_print_string_function(printToQString); // print to Output::out

    svm->setSVMType(ui->svmType_comboBox->currentIndex());
    svm->setKernel(ui->kernel_comboBox->currentIndex());

    QString epsString = ui->eps_lineEdit->text();
    //kernel params
    svm->setDegree(ui->degree_spinBox->value())
            .setGamma(ui->gamma_doubleSpinBox->value())
            .setCoef0(ui->coef_doubleSpinBox->value());
    //svm type params
    svm->setC(ui->C_doubleSpinBox->value())
            .setEps(QString(ui->eps_lineEdit->text()).toDouble(&ok))
            .setNu(ui->nu_doubleSpinBox->value())
            .setP(ui->P_doubleSpinBox->value())
            .setShrinking(ui->shrinking_checkBox->isChecked())
            .setProbability(ui->prob_checkBox->isChecked());
    if(ok){
        svm->readProblem();
        svm->trainModel();
        ui->output_textEdit->setText(Output::out);
    }else{
        ui->eps_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
    }
    //model path:
    ui->modelFile_path_label->setText(QString::fromStdString(svm->getModelFileName()));
}

void MainWindow::on_test_pushButton_clicked()
{
    svm->openPredictInputFile(test_file_path.toStdString());
    svm->openPredictOutputFile(svm->getPredictOutputFileName());
    svm->predict();
    int correct = svm->correctPrediction();
    int total = svm->totalPrediction();
    char accuracy_str[50];
    sprintf(accuracy_str, "Accuracy = %g%% (%d/%d) (classification)\n", (double)correct/total*100,correct,total);
    printToQString(accuracy_str);
    ui->output_textEdit->setText(Output::out);
}
