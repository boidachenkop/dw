#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    svm(new SVMController())
{
    ui->setupUi(this);

    output_handler = new OutputHandler(ui->output_textEdit);
    output_handler->startReaderThread();

    availability_handler = new AvailabilityHandler(ui->params_horizontalLayout, ui->tabWidget,
                                                   ui->train_pushButton, ui->test_pushButton);
    file_manager = new FileManager(ui->chooseDataset_label, ui->tstFile_path_label,
                                   ui->modelFile_path_label, ui->pattern_lineEdit);
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

}

MainWindow::~MainWindow()
{
    system(rm_tmp_plot_cmd.c_str()); // remove tmp file for plot

    delete ui;
}

int MainWindow::parseParameters()
{
    bool gamma_ok=false;
    bool coef0_ok=false;

    bool C_ok=false;
    bool eps_ok=false;
    bool nu_ok=false;

    bool P_ok=false;

    svm->openTrainFile(file_manager->getTrainFilepath().toStdString());
    svm->setSVMType(ui->svmType_comboBox->currentIndex());
    svm->setKernel(ui->kernel_comboBox->currentIndex());

    QString epsString = ui->eps_lineEdit->text();
    //kernel params
    svm->setDegree(ui->degree_spinBox->value())
            .setGamma(QString(ui->gamma_lineEdit->text()).toDouble(&gamma_ok))
            .setCoef0(QString(ui->coef0_lineEdit->text()).toDouble(&coef0_ok));
    //svm type params
    svm->setC(QString(ui->C_lineEdit->text()).toDouble(&C_ok))
            .setEps(QString(ui->eps_lineEdit->text()).toDouble(&eps_ok))
            .setNu(QString(ui->nu_lineEdit->text()).toDouble(&nu_ok))
            .setP(QString(ui->P_lineEdit->text()).toDouble(&P_ok))
            .setShrinking(ui->shrinking_checkBox->isChecked())
            .setProbability(ui->prob_checkBox->isChecked())
            .setCrossvalidation(ui->crossValidation_checkBox->isChecked(), ui->nFold_spinBox->value());
    if(gamma_ok && coef0_ok && C_ok && eps_ok && nu_ok && P_ok){
        ui->gamma_lineEdit->setStyleSheet("");
        ui->coef0_lineEdit->setStyleSheet("");
        ui->C_lineEdit->setStyleSheet("");
        ui->eps_lineEdit->setStyleSheet("");
        ui->nu_lineEdit->setStyleSheet("");
        ui->P_lineEdit->setStyleSheet("");
        return 0;
    }else{
        if(!gamma_ok)
            ui->gamma_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
        if(!coef0_ok)
            ui->coef0_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
        if(!C_ok)
            ui->C_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
        if(!eps_ok)
            ui->eps_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
        if(!nu_ok)
            ui->nu_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
        if(!P_ok)
            ui->P_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
        return -1;
    }
    //model path:
    ui->modelFile_path_label->setText(QString::fromStdString(svm->getModelFilePath()));
}

void MainWindow::setDefaultSVMParams(){
    ui->svmType_comboBox->setCurrentIndex(svm->getParams().svm_type);
    ui->kernel_comboBox->setCurrentIndex(svm->getParams().kernel_type);

    ui->degree_spinBox->setValue(svm->getParams().degree);
    ui->gamma_lineEdit->setText(QString::number(svm->getParams().gamma));
    ui->coef0_lineEdit->setText(QString::number(svm->getParams().coef0));

    ui->C_lineEdit->setText(QString::number(svm->getParams().C));
    ui->eps_lineEdit->setText(QString::number(svm->getParams().eps));
    ui->nu_lineEdit->setText(QString::number(svm->getParams().nu));

    ui->P_lineEdit->setText(QString::number(svm->getParams().p));
    ui->shrinking_checkBox->setChecked(svm->getParams().shrinking);
    ui->prob_checkBox->setChecked(svm->getParams().probability);

    ui->crossValidation_checkBox->setChecked(svm->isCrossvalidation());
    on_crossValidation_checkBox_toggled(svm->isCrossvalidation());
    on_y_scale_checkBox_toggled(false);
}

void MainWindow::on_chooseDataset_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose train dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    if(file_manager->setTrainFilepath(path) == 0){
        availability_handler->
                trainButtonEnabled(true)
                .scalingTabEnabled(true)
                .featureSelectionTabEnabled(true)
                .cvTabEnabled(true);
        if(file_manager->getNFeatures() <= 3){
            availability_handler->visualizationTabEnabled(true);
        }else{
            availability_handler->visualizationTabEnabled(false);
        }
        svm->setNFeatures(file_manager->getNFeatures());
        svm->setModelFilePath(file_manager->getModelFilepath().toStdString());
    }else{
        availability_handler->
            trainButtonEnabled(false)
            .scalingTabEnabled(false)
            .visualizationTabEnabled(false)
            .featureSelectionTabEnabled(false)
            .cvTabEnabled(false);

    }
}

void MainWindow::on_choose_tstFile_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose test dataset", "/home/pavlo/Desktop/pr/dw/datasets");
    if(file_manager->setTestFilepath(path) == 0){
        availability_handler->testButtonEnabled(true);
    }else{
        availability_handler->testButtonEnabled(false);
    }
}

void MainWindow::on_modelFile_toolButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, "Save model", "/Users/pavlo/Desktop/pr/dw/datasets");
    if(file_manager->setModelFilepath(path) == -1){
       printf("Model file is not set!");
    }else{
        svm->setModelFilePath(path.toStdString());
    }
}

void MainWindow::on_train_pushButton_clicked()
{
   if(parseParameters() == 0){
        svm->readProblem();
        svm->trainModel();
   }
}

void MainWindow::on_test_pushButton_clicked()
{
    svm->openPredictInputFile(file_manager->getTestFilepath().toStdString());
    svm->openPredictOutputFile(svm->getPredictOutputFilePath()); //default svmcontroller path
    svm->predict();
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

    svmscale scale_train(file_manager->getTrainFilepath().toStdString(),
                   file_manager->getTrainFilepath().toStdString()+".scale");
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
        file_manager->setTrainFilepath(file_manager->getTrainFilepath()+".scale") ;
    }
    if(!file_manager->getTestFilepath().isEmpty()){
        svmscale scale_test(file_manager->getTestFilepath().toStdString(),
                            file_manager->getTestFilepath().toStdString()+".scale");
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
            file_manager->setTestFilepath(file_manager->getTestFilepath()+".scale");
        }
    }
}

void MainWindow::on_y_scale_checkBox_toggled(bool checked)
{
   ui->y_lowerLimit_doubleSpinBox->setEnabled(checked);
   ui->y_upperLimit_doubleSpinBox->setEnabled(checked);
   ui->y_lowerLimit_label->setEnabled(checked);
   ui->y_upperLimit_label->setEnabled(checked);
}

void MainWindow::on_visualize_pushButton_clicked()
{
    ScriptQtManager::runPlot(file_manager->getTrainFilepath(), svm->getNFeatures(), true);
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
    int i=0;
    while(std::getline(data, line)){
        bool colon_met = false;
        for(auto c = line.end(); c != line.begin(); c--){
            if(*c == ':'){
                colon_met = true;
                c--;
                while(*c != ' '){
                    ndim_str+=*c;
                    c--;
                }
                break;
            }
        }
        if(!colon_met){
            ndim_str+='0';
        }
        i++;
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
    ScriptQtManager::runFeatureSelection(file_manager->getTrainFilepath(), svm->getNFeatures(), ui->pattern_lineEdit->text());

    file_manager->setTrainFilepath(file_manager->getTrainFilepath() + ".fselected");

    if(!file_manager->getTestFilepath().isEmpty()){
        ScriptQtManager::runFeatureSelection(file_manager->getTestFilepath(), svm->getNFeatures(), ui->pattern_lineEdit->text());

        file_manager->setTestFilepath(file_manager->getTestFilepath() + ".fselected");
    }
    svm->setNFeatures(file_manager->getNFeatures());
}

void MainWindow::on_output_textEdit_textChanged()
{
    ui->output_textEdit->verticalScrollBar()->setValue(
                ui->output_textEdit->verticalScrollBar()->maximum());
}
