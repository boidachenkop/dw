#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    svm(new SVMController())
{
    ui->setupUi(this);

    // buttons unabled on start
    ui->train_pushButton->setEnabled(false);
    ui->test_pushButton->setEnabled(false);
    ui->scale_toolButton->setEnabled(false);
    ui->visualize_pushButton->setEnabled(false);
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

    on_y_scale_checkBox_toggled(false);

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

    //output always down
    ui->output_textEdit->verticalScrollBar()->setValue(
                ui->output_textEdit->verticalScrollBar()->maximum());
}

void MainWindow::on_chooseDataset_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose train dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    if(!path.isEmpty()){
        train_file_path = path;
        ui->chooseDataset_label->setText(train_file_path);
        ui->train_pushButton->setEnabled(true);
        ui->scale_toolButton->setEnabled(true);
        if(getNFeatures(train_file_path.toStdString()) <= 3){
            ui->visualize_pushButton->setEnabled(true);
        }else{
            ui->visualize_pushButton->setEnabled(false);
        }
        //update model file path
        svm->setModelFilePath(train_file_path.toStdString()+".model");
        ui->modelFile_path_label->setText(train_file_path+QString::fromUtf8(".model"));
    }else if(!train_file_path.isEmpty()){
        ui->train_pushButton->setEnabled(true);
        ui->scale_toolButton->setEnabled(true);
        if(getNFeatures(train_file_path.toStdString()) <= 3){
            ui->visualize_pushButton->setEnabled(true);
        }else{
            ui->visualize_pushButton->setEnabled(false);
        }
        //update model file path
        svm->setModelFilePath(train_file_path.toStdString()+".model");
        ui->modelFile_path_label->setText(train_file_path+QString::fromUtf8(".model"));
    }else{
        ui->train_pushButton->setEnabled(false);
        ui->scale_toolButton->setEnabled(false);
        ui->visualize_pushButton->setEnabled(false);
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
        ui->eps_lineEdit->setStyleSheet("");
    }else{
        ui->eps_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
    }
    //model path:
    ui->modelFile_path_label->setText(QString::fromStdString(svm->getModelFilePath()));
}

void MainWindow::on_test_pushButton_clicked()
{
    svm->openPredictInputFile(test_file_path.toStdString());
    svm->openPredictOutputFile(svm->getPredictOutputFilePath());
    svm->predict();

    updateOutput();
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

void MainWindow::on_scale_toolButton_clicked()
{
    char *range_filepath = "/tmp/svm-app-scale-range";

    svmscale scale_train(train_file_path.toStdString(),
                   train_file_path.toStdString()+".scale");
    scale_train.setLowerUpper(ui->lowerLimit_doubleSpinBox->value(),
                        ui->upperLimit_doubleSpinBox->value());
    if(ui->y_scale_checkBox->isChecked()){ // y scaling
        scale_train.setYLowerUpper(ui->y_lowerLimit_doubleSpinBox->value(),
                                   ui->y_upperLimit_doubleSpinBox->value());
    }
    scale_train.setSaveFilename(range_filepath);
    if(scale_train.check()){
        scale_train.scale();
        //update filepathes and ui
        train_file_path = QString::fromStdString(train_file_path.toStdString()+".scale");
        ui->chooseDataset_label->setText(train_file_path);
    }
    updateOutput();
    if(!test_file_path.isEmpty()){
        svmscale scale_test(test_file_path.toStdString(),
                            test_file_path.toStdString()+".scale");
        scale_test.setLowerUpper(ui->lowerLimit_doubleSpinBox->value(),
                                 ui->upperLimit_doubleSpinBox->value());
        scale_test.setRestoreFilename(range_filepath);
        if(ui->y_scale_checkBox->isChecked()){ // y scaling
            scale_train.setYLowerUpper(ui->y_lowerLimit_doubleSpinBox->value(),
                                       ui->y_upperLimit_doubleSpinBox->value());
        }
        if(scale_test.check()){
            scale_test.scale();
            //update filepathes and ui
            test_file_path = QString::fromStdString(test_file_path.toStdString()+".scale");
            ui->tstFile_path_label->setText(test_file_path);
        }
        updateOutput();
    }
}

void MainWindow::on_y_scale_checkBox_toggled(bool checked)
{
   ui->y_lowerLimit_doubleSpinBox->setEnabled(checked);
   ui->y_upperLimit_doubleSpinBox->setEnabled(checked);
   ui->y_lowerLimit_label->setEnabled(checked);
   ui->y_upperLimit_label->setEnabled(checked);
}

#include "gnuplot_i.hpp"
void MainWindow::on_visualize_pushButton_clicked()
{
    int n_features = getNFeatures(train_file_path.toStdString());
    //prepare unlabled file
    std::string prepare_data_cmd;
    if(n_features == 1){
        prepare_data_cmd = R"( awk '{if($2 == ""){$2="0.0"} gsub("[0-9]+:", "", $2); print $2, $1}' )";
    }else if(n_features == 2){
        prepare_data_cmd = R"( awk '{if($3 == ""){$3="0.0"} if($2 == ""){$2="0.0"} gsub("[0-9]+:", "", $2); gsub("[0-9]+:", "", $3); print $2, $3, $1}' )";
    }else if(n_features == 3){
        prepare_data_cmd = R"( awk '{if($2 == ""){$2="0.0"} if($3 == ""){$3="0.0"} if($4 == ""){$4="0.0"} gsub("[0-9]+:", "", $2); gsub("[0-9]+:", "", $3); gsub("[0-9]+:", "", $4); print $2, $3, $4, $1}' )";
    }
    prepare_data_cmd+=train_file_path.toStdString()+" > "+train_file_path.toStdString()+".tmp";
    system(prepare_data_cmd.c_str());
    Gnuplot gp;
    gp.plotfile_xy(train_file_path.toStdString()+".tmp")<<"pause mouse close";
    //plot with gnuplot
//    std::string plot_cmd;
//    if(n_features == 3){
//        plot_cmd = "gnuplot -e \"splot '";
//    }else{
//        plot_cmd = "gnuplot -e \"plot '";
//    }
//    plot_cmd+=train_file_path.toStdString()+".tmp' with points palette; pause 5; reread\"";
//    system(plot_cmd.c_str());

    //remove .tmp
    std::string rm_cmd = "rm "+train_file_path.toStdString()+".tmp";
//    system(rm_cmd.c_str());
}

int getNFeatures(std::string data_filepath){
    //read 1st line from file and parse number of features
    std::fstream data;
    data.open(data_filepath, std::fstream::in);
    std::string ndim_str;
    std::string line;
    std::getline(data, line);
    data.close();
    for(auto c = line.end(); c != line.begin(); c--){
        if(*c == ':'){
            c--;
            while(*c != ' '){
                ndim_str+=*c;
                c--;
            }
            break;
        }
    }
    reverse(ndim_str.begin(), ndim_str.end());
    return stoi(ndim_str);
}
