#include <QFile>
#include <QScrollBar>
#include <QTextStream>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QMovie>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstdio>

#include "loadingindicator.h"
#include "svmscale.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    svm(new SVMController())
{
    ui->setupUi(this);

    output_handler = new OutputHandler();
    output_handler->startReaderThread();
    output_handler->setCmdOutput(true);

    availability_handler = new AvailabilityHandler();
    file_manager = new FileManager();

    // output textEdit read only
    ui->output_textEdit->setReadOnly(true);
    //set default params
    setDefaultSVMParams();

    LoadingIndicator::setIndicatorLabel(ui->loading_label);

    //info update
    connect(file_manager, &FileManager::updateTrainNLines, ui->train_rows_label, &QLabel::setText);
    connect(file_manager, &FileManager::updateTrainNClasses, ui->train_classes_label, &QLabel::setText);
    connect(file_manager, &FileManager::updateTrainNFeatures, ui->train_features_label, &QLabel::setText);
    connect(file_manager, &FileManager::updateTestNLines, ui->test_rows_label, &QLabel::setText);
    connect(file_manager, &FileManager::updateTestNClasses, ui->test_classes_label, &QLabel::setText);
    connect(file_manager, &FileManager::updateTestNFeatures, ui->test_features_label, &QLabel::setText);
    connect(file_manager, &FileManager::updateModelReady, ui->showModel_checkBox, &QCheckBox::setEnabled);
    connect(file_manager, &FileManager::updateTrainInputFilepath, this, &MainWindow::updateTrainFilepathLabel);
    connect(file_manager, &FileManager::updateTestInputFilepath, this, &MainWindow::updateTestFilepathLabel);
    connect(file_manager, &FileManager::updateModelFilepath, this, &MainWindow::updateModelFilepathLabel);
    connect(file_manager, &FileManager::updateFeatureSelection, ui->pattern_lineEdit, &QLineEdit::setText);
    connect(output_handler, &OutputHandler::updateOutput, this, &MainWindow::updateOutputTextEdit);

    //availability
    //parameters
    connect(availability_handler, &AvailabilityHandler::degreeEnabled, ui->degree_spinBox, &QSpinBox::setEnabled);
    connect(availability_handler, &AvailabilityHandler::gammaEnabled, ui->gamma_lineEdit, &QLineEdit::setEnabled);
    connect(availability_handler, &AvailabilityHandler::coef0Enabled, ui->coef0_lineEdit, &QLineEdit::setEnabled);
    connect(availability_handler, &AvailabilityHandler::cEnabled, ui->C_lineEdit, &QLineEdit::setEnabled);
    connect(availability_handler, &AvailabilityHandler::nuEnabled, ui->nu_lineEdit, &QLineEdit::setEnabled);
    connect(availability_handler, &AvailabilityHandler::pEnabled, ui->P_lineEdit, &QLineEdit::setEnabled);
    //train and test buttons + validation
    connect(availability_handler, &AvailabilityHandler::trainEnabled, ui->train_pushButton, &QPushButton::setEnabled);
    connect(availability_handler, &AvailabilityHandler::testEnabled, ui->test_pushButton, &QPushButton::setEnabled);
    connect(availability_handler, &AvailabilityHandler::testEnabled, ui->validation_label, &QLabel::setEnabled);
    connect(availability_handler, &AvailabilityHandler::testEnabled, ui->validation_percent_label, &QLabel::setEnabled);
    connect(availability_handler, &AvailabilityHandler::testEnabled, ui->validation_lineEdit, &QLineEdit::setEnabled);
    //tabs
    connect(availability_handler, &AvailabilityHandler::cvEnabled, ui->crossValidation_tab, &QWidget::setEnabled);
    connect(availability_handler, &AvailabilityHandler::scalingEnabled, ui->scaling_tab, &QWidget::setEnabled);
    connect(availability_handler, &AvailabilityHandler::visualizationEnabled, ui->visualization_tab, &QWidget::setEnabled);
    connect(availability_handler, &AvailabilityHandler::fsEnabled, ui->feature_selection_tab, &QWidget::setEnabled);
    connect(availability_handler, &AvailabilityHandler::cvSBEnabled, ui->cv_spinBox, &QSpinBox::setEnabled);
    connect(availability_handler, &AvailabilityHandler::convertEnabled, ui->format_tab, &QSpinBox::setEnabled);
    connect(availability_handler, &AvailabilityHandler::gridSearchEnabled, ui->gridSearch_tab, &QSpinBox::setEnabled);
    connect(ui->svmType_comboBox, SIGNAL(currentIndexChanged(int)), availability_handler, SLOT(filterSVMTypeParams(int)));
    connect(ui->kernel_comboBox, SIGNAL(currentIndexChanged(int)), availability_handler,SLOT(filterKernelParams(int)));
    //info labels visibility
    connect(availability_handler, &AvailabilityHandler::trainInfoVisible, ui->train_classesText_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::trainInfoVisible, ui->train_classes_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::trainInfoVisible, ui->train_featuresText_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::trainInfoVisible, ui->train_features_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::trainInfoVisible, ui->train_rowsText_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::trainInfoVisible, ui->train_rows_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::trainInfoVisible, ui->train_spacer_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::testInfoVisible, ui->test_classesText_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::testInfoVisible, ui->test_classes_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::testInfoVisible, ui->test_featuresText_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::testInfoVisible, ui->test_features_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::testInfoVisible, ui->test_rowsText_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::testInfoVisible, ui->test_rows_label, &QLabel::setVisible);
    connect(availability_handler, &AvailabilityHandler::testInfoVisible, ui->test_spacer_label, &QLabel::setVisible);

    //tabs and buttons unabled on start
    availability_handler->cvTabEnabled(false)
            .scalingTabEnabled(false)
            .visualizationTabEnabled(false)
            .featureSelectionTabEnabled(false)
            .convertTabEnabled(false)
            .gridSearchTabEnabled(false)
            .trainButtonEnabled(false)
            .testButtonEnabled(false)
            .cvPercentLabelVisible(false);

    //filter params
    availability_handler->filterSVMTypeParams(ui->svmType_comboBox->currentIndex());
    availability_handler->filterKernelParams(ui->kernel_comboBox->currentIndex());

    //info invisible
    availability_handler->trainInfoVisible(false);
    availability_handler->testInfoVisible(false);
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
            .setCrossvalidation(ui->crossValidation_checkBox->isChecked(),
                                ui->cv_spinBox->value());
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
    on_y_scale_checkBox_toggled(false);

    ui->grid_cv_spinBox->setValue(5);
    ui->grid_log2c_lineEdit->setText("-5,15,2");
    ui->grid_log2g_lineEdit->setText("3,-15,-2");
}

void MainWindow::on_chooseDataset_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose train dataset", file_manager->getLastOpenedPath());
    if(file_manager->setDatasetFilepath(path, file_manager->TRAIN) == 0){
        availability_handler->
                trainButtonEnabled(true)
                .scalingTabEnabled(true)
                .featureSelectionTabEnabled(true)
                .cvTabEnabled(true)
                .gridSearchTabEnabled(true)
                .trainInfoLabelsVisible(true)
                .convertTabEnabled(file_manager->isTestOK() ? false : true);
        if(file_manager->getNFeatures() <= 3){
            availability_handler->visualizationTabEnabled(true);
        }else{
            availability_handler->visualizationTabEnabled(false);
        }
        svm->setModelFilePath(file_manager->getModelFilepath().toStdString());
    }else{
        availability_handler->
            trainButtonEnabled(false)
            .scalingTabEnabled(false)
            .visualizationTabEnabled(false)
            .featureSelectionTabEnabled(false)
            .cvTabEnabled(false)
            .gridSearchTabEnabled(false)
            .trainInfoLabelsVisible(false)
            .convertTabEnabled(true);

    }
}

void MainWindow::on_choose_tstFile_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose test dataset", file_manager->getLastOpenedPath());
    if(file_manager->setDatasetFilepath(path, file_manager->TEST) == 0){
        availability_handler->testButtonEnabled(true)
                .testInfoLabelsVisible(true)
                .convertTabEnabled(file_manager->isTrainOK() ? false : true);
    }else{
        availability_handler->testButtonEnabled(false)
                .testInfoLabelsVisible(false)
                .convertTabEnabled(true);
    }
}

void MainWindow::on_modelFile_toolButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, "Save model", file_manager->getLastOpenedPath());
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
   file_manager->updateModelReady(true);
}

void MainWindow::on_test_pushButton_clicked()
{
    bool convert_ok;
    int validation_percent = QString(ui->validation_lineEdit->text()).toInt(&convert_ok);
    if(convert_ok){
        ui->validation_lineEdit->setStyleSheet("");
        svm->openPredictInputFile(file_manager->getTestFilepath().toStdString());
        svm->openPredictOutputFile(svm->getPredictOutputFilePath()); //default svmcontroller path
        svm->predict();

        if(ScriptQtManager::runHoldout(1, file_manager->getTrainFilepath(), validation_percent,
                                    "null", file_manager->getTrainFilepath()+".tmp") != -1){

            svm->openPredictInputFile((file_manager->getTrainFilepath()+".tmp").toStdString());
            svm->openPredictOutputFile((file_manager->getTrainFilepath()+".tmpout").toStdString());
            std::cout<<"Validation ";
            fflush(stdout);
            svm->predict();
            system("rm -rf "+(file_manager->getTrainFilepath()+".tmp ").toLocal8Bit()+(file_manager->getTrainFilepath()+".tmpout").toLocal8Bit());
        }
    }else{
        ui->validation_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
    }

}

void MainWindow::on_scale_toolButton_clicked()
{
    char *range_filepath = (char*)"/tmp/svm-app-scale-range";

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
        file_manager->setDatasetFilepath(file_manager->getTrainFilepath()+".scale", file_manager->TRAIN);
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
            file_manager->setDatasetFilepath(file_manager->getTestFilepath()+".scale", file_manager->TEST);
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
    bool ok = false;
    double bandwidth = QString(ui->bandwidth_lineEdit->text()).toDouble(&ok);
    if(ok){
        ui->bandwidth_lineEdit->setStyleSheet("border-color: black;");
        ScriptQtManager::runPlot(file_manager,
                                 ui->densityPlot_checkBox->isChecked(),
                                 ui->showModel_checkBox->isChecked(),
                                 bandwidth);
    }else{
        ui->bandwidth_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
    }
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
    ScriptQtManager::runFeatureSelection(file_manager->getTrainFilepath(), file_manager->getNFeatures(), ui->pattern_lineEdit->text());

    file_manager->setDatasetFilepath(file_manager->getTrainFilepath() + ".fselected", file_manager->TRAIN);

    if(!file_manager->getTestFilepath().isEmpty()){
        ScriptQtManager::runFeatureSelection(file_manager->getTestFilepath(), file_manager->getNFeatures(), ui->pattern_lineEdit->text());

        file_manager->setDatasetFilepath(file_manager->getTestFilepath() + ".fselected", file_manager->TEST);
    }
    if(file_manager->getNFeatures() <= 3){
        availability_handler->visualizationTabEnabled(true);
    }else{
        availability_handler->visualizationTabEnabled(false);
    }
}

void MainWindow::on_output_textEdit_textChanged()
{
    ui->output_textEdit->verticalScrollBar()->setValue(
                ui->output_textEdit->verticalScrollBar()->maximum());
}

void MainWindow::updateTrainFilepathLabel(QString filepath, bool correct)
{
    if(correct){
        ui->chooseDataset_label->setStyleSheet("QLabel{color : black;}");
        ui->chooseDataset_label->setText(filepath);
    }else{
        ui->chooseDataset_label->setStyleSheet("QLabel{color : red;}");
        ui->chooseDataset_label->setText(filepath);
    }
}

void MainWindow::updateTestFilepathLabel(QString filepath, bool correct)
{
    if(correct){
        ui->tstFile_path_label->setStyleSheet("QLabel{color : black;}");
        ui->tstFile_path_label->setText(filepath);
    }else{
        ui->tstFile_path_label->setStyleSheet("QLabel{color : red;}");
        ui->tstFile_path_label->setText(filepath);
    }
}

void MainWindow::updateModelFilepathLabel(QString filepath)
{
    ui->modelFile_path_label->setText(filepath);
}

void MainWindow::updateOutputTextEdit(QString text)
{
    ui->output_textEdit->moveCursor(QTextCursor::End);
    ui->output_textEdit->insertPlainText(text);
    ui->output_textEdit->moveCursor(QTextCursor::End);
}

void MainWindow::on_cvType_comboBox_currentIndexChanged(int index)
{
    switch(index){
        case 0: // n-fold
            availability_handler->cvSpinBoxEnabled(true);
            availability_handler->cvPercentLabelVisible(false);
            ui->cv_spinBox->setMaximum(file_manager->getNLines()-1);
            ui->cv_spinBox->setValue(0);
        break;
        case 1: //leave one out
            availability_handler->cvSpinBoxEnabled(false);
            availability_handler->cvPercentLabelVisible(false);
            ui->cv_spinBox->setMaximum(file_manager->getNLines());
            ui->cv_spinBox->setValue(file_manager->getNLines()-1);
        break;
        case 2: // percentage
            availability_handler->cvSpinBoxEnabled(true);
            availability_handler->cvPercentLabelVisible(true);
            ui->cv_spinBox->setMaximum(100);
            ui->cv_spinBox->setValue(0);
        break;
    }
}

void MainWindow::on_holdout_pushButton_clicked()
{
    bool ok;
    double test_percent = ui->holdout_testPart_lineEdit->text().toDouble(&ok);
    if(ok){
        ui->holdout_testPart_lineEdit->setStyleSheet("");
        QString train_part = file_manager->getTrainFilepath()+".ho_train";
        QString test_part = file_manager->getTrainFilepath()+".ho_test";
        int exitCode = ScriptQtManager::runHoldout(ui->holdout_comboBox->currentIndex(),
                                    file_manager->getTrainFilepath(),
                                    test_percent,
                                    train_part, test_part);
        if(exitCode == 0){
            file_manager->setDatasetFilepath(train_part, file_manager->TRAIN);
            file_manager->setDatasetFilepath(test_part, file_manager->TEST);
            availability_handler->trainButtonEnabled(true)
                    .testButtonEnabled(true)
                    .trainInfoLabelsVisible(true)
                    .testInfoLabelsVisible(true);
        }else{
            availability_handler->trainButtonEnabled(false)
                    .testButtonEnabled(false);
            std::cout<<"Holdout error."<<std::endl;
        }
    }else{
        ui->holdout_testPart_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
    }
}

void MainWindow::on_convert_pushButton_clicked()
{
    if(ui->feature_sep_lineEdit->text().isEmpty()){
        ui->feature_sep_lineEdit->setStyleSheet("border-style: outset; border-width: 1px; border-color: red;");
        return;
    }else{
        ui->feature_sep_lineEdit->setStyleSheet("");
    }
    QString dec_sep;
    bool label_first;
    dec_sep = ui->dec_sep_comboBox->currentIndex() == 0 ? dec_sep = "." : ",";
    label_first =  ui->label_at_end_checkBox->isChecked() ? false : true;
    int exitCode = 0;
    if(!file_manager->isTrainOK()){
        exitCode = ScriptQtManager::runConvert2SvmFormat(file_manager->getTrainFilepath(),
                                                             file_manager->getTrainFilepath()+".libsvm",
                                                             ui->feature_sep_lineEdit->text(),
                                                             dec_sep,
                                                             label_first);
        if(exitCode == 0){
            file_manager->setDatasetFilepath(file_manager->getTrainFilepath()+".libsvm", file_manager->TRAIN);
            availability_handler->
                    trainButtonEnabled(true)
                    .scalingTabEnabled(true)
                    .featureSelectionTabEnabled(true)
                    .cvTabEnabled(true)
                    .trainInfoLabelsVisible(true)
                    .convertTabEnabled(file_manager->isTestOK() ? false : true);
        }
    }
    if(!file_manager->isTestOK()){
        exitCode = ScriptQtManager::runConvert2SvmFormat(file_manager->getTestFilepath(),
                                                             file_manager->getTestFilepath()+".libsvm",
                                                             ui->feature_sep_lineEdit->text(),
                                                             dec_sep,
                                                             label_first);
        if(exitCode == 0){
            file_manager->setDatasetFilepath(file_manager->getTestFilepath()+".libsvm", file_manager->TEST);
            availability_handler->testButtonEnabled(true)
                    .testInfoLabelsVisible(true)
                    .convertTabEnabled(file_manager->isTrainOK() ? false : true);
        }
    }
}

void MainWindow::on_grid_pushButton_clicked()
{
    ScriptQtManager::runGridSearch(file_manager->getTrainFilepath(),
                                   ui->grid_log2c_lineEdit->text(),
                                   ui->grid_log2g_lineEdit->text(),
                                   QString::number(ui->grid_cv_spinBox->value()),
                                   ui->grid_anim_checkBox->isChecked());
}

void MainWindow::on_genData_toolButton_clicked()
{

}
