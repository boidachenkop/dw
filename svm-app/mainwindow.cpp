#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <utility>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    svm(new SVMController())
{
    ui->setupUi(this);

    availability_handler = new AvailabilityHandler(ui->params_horizontalLayout, ui->tabWidget,
                                                   ui->train_pushButton, ui->test_pushButton);

    //tabs and buttons unabled on start
    availability_handler->cvTabEnabled(false)
            .scalingTabEnabled(false)
            .visualizationTabEnabled(false)
            .featureSelectionTabEnabled(false)
            .trainButtonEnabled(false)
            .testButtonEnabled(false);

    // output textEdit read only
    ui->output_textEdit->setReadOnly(true);

    //set default params
    setDefaultSVMParams();

    //filter params
    availability_handler->filterSVMTypeParams(ui->svmType_comboBox->currentIndex());
    availability_handler->filterKernelParams(ui->kernel_comboBox->currentIndex());

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

    //remove .tmp plot files
    std::cout<<rm_tmp_plot_cmd<<std::endl;
    system(rm_tmp_plot_cmd.c_str());
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

void MainWindow::setDefaultSVMParams(){
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
}

void MainWindow::on_chooseDataset_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose train dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    if(!path.isEmpty()){
        QStringList arguments{"./checkdata.py", path};
        QProcess py_script;
        py_script.start("python3", arguments);
        py_script.waitForFinished();
        QString py_script_out(py_script.readAllStandardOutput());
        if(py_script_out != "No error.\n"){
            ui->chooseDataset_label->setStyleSheet("QLabel{color : red;}");
            printf("%s\n", py_script_out.toLatin1().data());
            updateOutput();
        }else{
            ui->chooseDataset_label->setStyleSheet("QLabel{color : black;}");
            train_file_path = path;
            ui->chooseDataset_label->setText(train_file_path);
            availability_handler->
                    trainButtonEnabled(true)
                    .scalingTabEnabled(true)
                    .featureSelectionTabEnabled(true);
            svm->setNFeatures(getNFeatures(train_file_path.toStdString()));
            ui->pattern_lineEdit->setText("1-"+QString::number(svm->getNFeatures()));
            if(svm->getNFeatures() <= 3){
                availability_handler->visualizationTabEnabled(true);
            }else{
                availability_handler->visualizationTabEnabled(false);
            }
            //update model file path
            svm->setModelFilePath(train_file_path.toStdString()+".model");
            ui->modelFile_path_label->setText(train_file_path+QString::fromUtf8(".model"));
        }

    }else if(!train_file_path.isEmpty()){
        availability_handler->
                trainButtonEnabled(true)
                .scalingTabEnabled(true);
        if(getNFeatures(train_file_path.toStdString()) <= 3){
            availability_handler->visualizationTabEnabled(true);
        }else{
            availability_handler->visualizationTabEnabled(false);
        }
        //update model file path
        svm->setModelFilePath(train_file_path.toStdString()+".model");
        ui->modelFile_path_label->setText(train_file_path+QString::fromUtf8(".model"));
    }else{
        availability_handler->
                trainButtonEnabled(false)
                .scalingTabEnabled(false)
                .visualizationTabEnabled(false)
                .featureSelectionTabEnabled(false);
    }
}

void MainWindow::on_choose_tstFile_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose test dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    if(!path.isEmpty()){
        test_file_path = path;
        ui->tstFile_path_label->setText(test_file_path);
        availability_handler->testButtonEnabled(true);
    }else if(!test_file_path.isEmpty()){
        ui->tstFile_path_label->setText(test_file_path);
        availability_handler->testButtonEnabled(true);
    }else{
        availability_handler->testButtonEnabled(false);
    }
}

void MainWindow::on_modelFile_toolButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, "Save model", "/Users/pavlo/Desktop/pr/dw/datasets");
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

void MainWindow::on_svmType_comboBox_currentIndexChanged(int index){
    availability_handler->filterSVMTypeParams(index);
}

void MainWindow::on_kernel_comboBox_currentIndexChanged(int index){
    availability_handler->filterKernelParams(index);
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

    //plot with gnuplot
    Gnuplot gp;
//    gp<<"set terminal";
    std::string plot_cmd;
    if(n_features == 3){
        plot_cmd = "splot '";
    }else{
        plot_cmd = "plot '";
    }
    plot_cmd+=train_file_path.toStdString()+".tmp' with points palette; pause mouse close";
    gp<<plot_cmd;

    rm_tmp_plot_cmd += train_file_path.toStdString()+".tmp ";//remove file in d-tor
}

int getNFeatures(std::string data_filepath){
    //read file and parse number of features
    std::fstream data;
    data.open(data_filepath, std::fstream::in);
    if(!data.is_open()){
       printf("Can't open file %s", data_filepath.c_str());
       return -1;
    }
    std::string ndim_str;
    std::string line;
    int max_dim=0;
    while(std::getline(data, line)){
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
        int cur_dim = stoi(ndim_str);
        max_dim = max_dim < cur_dim ? cur_dim : max_dim;
        ndim_str = "";
    }
    data.close();
    return max_dim;
}

void MainWindow::on_select_pushButton_clicked()
{
    std::cout<<"nFeatures: "<<svm->getNFeatures()<<std::endl;
    QStringList arguments{"./f_select.py", train_file_path,
                QString::number(svm->getNFeatures()), ui->pattern_lineEdit->text()};
    QProcess py_script;
    py_script.start("python3", arguments);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardOutput());
    printf("%s\n", py_script_out.toLatin1().data());
    updateOutput();
    train_file_path = QString::fromStdString(train_file_path.toStdString() + ".fselected");
    ui->chooseDataset_label->setText(train_file_path);
    if(!test_file_path.isEmpty()){
        QStringList arguments{"./f_select.py", test_file_path,
                    QString::number(svm->getNFeatures()), ui->pattern_lineEdit->text()};
        QProcess py_script;
        py_script.start("python3", arguments);
        py_script.waitForFinished();
        QString py_script_out(py_script.readAllStandardOutput());
        printf("%s\n", py_script_out.toLatin1().data());
        updateOutput();
        test_file_path = QString::fromStdString(test_file_path.toStdString() + ".fselected");
        ui->tstFile_path_label->setText(test_file_path);
    }
    svm->setNFeatures(getNFeatures(train_file_path.toStdString()));
    std::cout<<"nFeatures: "<<svm->getNFeatures()<<std::endl;
    ui->pattern_lineEdit->setText("1-"+QString::number(svm->getNFeatures()));
}
