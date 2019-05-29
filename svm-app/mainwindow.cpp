#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <cstdio>


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

    ui->crossValidation_checkBox->setChecked(svm->isCrossvalidation());
    on_crossValidation_checkBox_toggled(svm->isCrossvalidation());

    //filter params
    filterSVMTypeParams(ui->svmType_comboBox->currentIndex());
    filterKernelParams(ui->kernel_comboBox->currentIndex());

    stdout_redirect_path = "/tmp/svm-app-out";
    std_old = stdout;
    stdout = fopen(stdout_redirect_path.c_str(), "w");
}

MainWindow::~MainWindow()
{
    if(fclose(stdout) != 0){
       std::cerr<<"Can't close stdout redirect file"<<std::endl;
    }
    stdout = std_old;
    delete ui;
}

void MainWindow::updateOutput(){
    // read redirected stdout and write to QTextEdit
    fflush(stdout);
    std::ifstream in_from_file(stdout_redirect_path);
    std::string output((std::istreambuf_iterator<char>(in_from_file)),
                       (std::istreambuf_iterator<char>()));
    ui->output_textEdit->setText(QString::fromStdString(output));
    in_from_file.close();
}

void MainWindow::on_chooseDataset_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose train dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    if(!path.isEmpty()){
        train_file_path = path;
        ui->chooseDataset_label->setText(train_file_path);
        ui->train_pushButton->setEnabled(true);
        //update model file path
        svm->setModelFilePath(train_file_path.toStdString()+".model");
        ui->modelFile_path_label->setText(train_file_path+QString::fromUtf8(".model"));
    }else if(!train_file_path.isEmpty()){
        ui->train_pushButton->setEnabled(true);
        //update model file path
        svm->setModelFilePath(train_file_path.toStdString()+".model");
        ui->modelFile_path_label->setText(train_file_path+QString::fromUtf8(".model"));
    }else{
        ui->train_pushButton->setEnabled(false);
    }
}

void MainWindow::on_choose_tstFile_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose test dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    if(!path.isEmpty()){
        test_file_path = path;
        ui->tstFile_path_label->setText(test_file_path);
        ui->test_pushButton->setEnabled(true);
    }else if(!test_file_path.isEmpty()){
        ui->tstFile_path_label->setText(test_file_path);
        ui->test_pushButton->setEnabled(true);
    }else{
        ui->test_pushButton->setEnabled(false);
    }
}

void MainWindow::on_modelFile_toolButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, "Save model", "/home/pavlo/Desktop/pr/dw/datasets");
    if(!path.isEmpty()){
        svm->setModelFilePath(path.toStdString());
        ui->modelFile_path_label->setText(QString::fromStdString(svm->getModelFilePath()));
    }else if(!svm->getModelFilePath().empty()){
        ui->modelFile_path_label->setText(QString::fromStdString(svm->getModelFilePath()));
    }
}

void MainWindow::on_train_pushButton_clicked()
{
    bool ok=false;

    svm->openTrainFile(train_file_path.toStdString());

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
            .setProbability(ui->prob_checkBox->isChecked())
            .setCrossvalidation(ui->crossValidation_checkBox->isChecked(), ui->nFold_spinBox->value());
    if(ok){
        svm->readProblem();
        svm->trainModel();
        updateOutput();
    }else{
        ui->eps_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
    }
    //model path:
    ui->modelFile_path_label->setText(QString::fromStdString(svm->getModelFilePath()));

    //output always down
    ui->output_textEdit->verticalScrollBar()->setValue(
                ui->output_textEdit->verticalScrollBar()->maximum());
}

void MainWindow::on_test_pushButton_clicked()
{
    svm->openPredictInputFile(test_file_path.toStdString());
    svm->openPredictOutputFile(svm->getPredictOutputFilePath());
    svm->predict();

    updateOutput();

    //output always down
    ui->output_textEdit->verticalScrollBar()->setValue(
                ui->output_textEdit->verticalScrollBar()->maximum());
}

void MainWindow::filterSVMTypeParams(int svm_type){
    QDoubleSpinBox *C = ui->C_doubleSpinBox;
    QDoubleSpinBox *nu = ui->nu_doubleSpinBox;
    QDoubleSpinBox *p = ui->P_doubleSpinBox;
    switch(svm_type){
        case 0: // C-SVC
            C->setEnabled(true);
            nu->setEnabled(false);
            p->setEnabled(false);
        break;
        case 1: // nu-SVC
            C->setEnabled(false);
            nu->setEnabled(true);
            p->setEnabled(false);
        break;
        case 2: // one class SVM
            C->setEnabled(false);
            nu->setEnabled(true);
            p->setEnabled(false);
        break;
        case 3: // epsilon-SVR
            C->setEnabled(true);
            nu->setEnabled(false);
            p->setEnabled(true);
        break;
        case 4: // nu-SVR
            C->setEnabled(true);
            nu->setEnabled(true);
            p->setEnabled(false);
        break;
    }
}

void MainWindow::filterKernelParams(int kernel){
    QSpinBox *degree = ui->degree_spinBox;
    QDoubleSpinBox *gamma = ui->gamma_doubleSpinBox;
    QDoubleSpinBox *coef0 = ui->coef_doubleSpinBox;
    switch(kernel){
        case 0: // Linear
            degree->setEnabled(true);
            gamma->setEnabled(false);
            coef0->setEnabled(false);
        break;
        case 1: // poly
            degree->setEnabled(false);
            gamma->setEnabled(true);
            coef0->setEnabled(true);
        break;
        case 2: // RBF
            degree->setEnabled(false);
            gamma->setEnabled(true);
            coef0->setEnabled(false);
        break;
        case 3: // Sigmoid
            degree->setEnabled(false);
            gamma->setEnabled(true);
            coef0->setEnabled(true);
        break;
        case 4: // Precomputed
            degree->setEnabled(false);
            gamma->setEnabled(false);
            coef0->setEnabled(false);
        break;
    }

}

void MainWindow::on_svmType_comboBox_currentIndexChanged(int index){
   filterSVMTypeParams(index);
}

void MainWindow::on_kernel_comboBox_currentIndexChanged(int index){
    filterKernelParams(index);
}

void MainWindow::on_crossValidation_checkBox_toggled(bool checked)
{
    ui->nFold_label->setEnabled(checked);
    ui->nFold_spinBox->setEnabled(checked);
}
