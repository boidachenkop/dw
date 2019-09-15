#include <QStringList>
#include <QProcess>
#include <QCoreApplication>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "loadingindicator.h"
#include "gnuplot_i.hpp"
#include "scriptqtmanager.h"

int ScriptQtManager::runCheckData(QString filepath)
{
    QProcess py_script;
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/checkdata.py"<<filepath;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    if(py_script_out == "No error.\n"){
        return 0;
    }else{
        printf("%s\n", py_script_out.toLatin1().data());
        fflush(stdout);
        return -1;
    }
}

void ScriptQtManager::runFeatureSelection(QString filepath, int n_features, QString pattern)
{
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/f_select.py"<<filepath<<QString::number(n_features)<<pattern;
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
}

std::string ScriptQtManager::runPlot(FileManager* file_manager, bool density, double band_width)
{
    Gnuplot gp;
    QString filepath = file_manager->getTrainFilepath();
    int n_features = file_manager->getNFeatures();
    int n_lines = file_manager->getNLines();
    std::vector<std::string> labels = file_manager->getLabels();
    std::vector<std::string> colors{"red", "blue", "green", "black", "orange", "purple", "cyan", "coral"};
    std::string to_remove = " ";
    std::string prepare_data_cmd;
    std::stringstream plot_cmd;
    std::string save_filepath = filepath.toStdString() + ".tmp";

    //remove feature numbers from dataset
    if(n_features == 1){
        prepare_data_cmd = R"( awk '{if($2 == ""){$2="0.0"} gsub("[0-9]+:", "", $2); print $2, 0, $1}' )";
    }else if(n_features == 2){
        prepare_data_cmd = R"( awk '{if($3 == ""){$3="0.0"} if($2 == ""){$2="0.0"} gsub("[0-9]+:", "", $2);
                            gsub("[0-9]+:", "", $3); print $2, $3, $1}' )";
    }else if(n_features == 3){
        prepare_data_cmd = R"( awk '{if($2 == ""){$2="0.0"} if($3 == ""){$3="0.0"} if($4 == ""){$4="0.0"} gsub("[0-9]+:", "", $2);
                            gsub("[0-9]+:", "", $3); gsub("[0-9]+:", "", $4); print $2, $3, $4, $1}' )";
    }else{
        return "";
    }
    prepare_data_cmd+=filepath.toStdString()+" > " + save_filepath;
    to_remove += save_filepath + " ";
    system(prepare_data_cmd.c_str());

    if(density){
        if(labels.size() > 8){
            std::cout<<"Too many classes. Density plot is possible if dataset has less than 8 classes"<<std::endl;
            return "";
        }
        std::vector<std::string> one_class_files;
        std::stringstream grepcmd;
        for(int i=0; i < (int)labels.size(); i++){
            one_class_files.push_back(save_filepath + std::to_string(i));
            grepcmd<<"grep ' "<<labels[i]<<"$' "<<save_filepath
                  <<" > "<<one_class_files.back()<<"; ";
            to_remove += one_class_files.back() + " ";
        }

        system(grepcmd.str().c_str());

        if(n_features == 1){
            plot_cmd<<"set key off;\n set border 3; \n bin_width = "<<band_width<<";"
                   <<"bin_number(x) = floor(x/bin_width);\n"<<"rounded(x) = bin_width * ( bin_number(x) + 0.5 );\n"
                  << "plot ";

            auto i = one_class_files.size();
            for(i=0; i < one_class_files.size(); i++){
                plot_cmd<<"'"<<one_class_files[i]<<"' using (rounded($1)):(1) smooth kdensity lt rgb \""<<colors[i]<<"\", ";
            }

            for(i=0; i < one_class_files.size(); i++){
                plot_cmd<<"'"<<one_class_files[i]<<"' lt rgb \""<<colors[i]<<"\"";
                if(i+1 < one_class_files.size()){
                    plot_cmd<<", ";
                }
            }

            plot_cmd<<"; pause mouse close";
            gp<<plot_cmd.str();
        }else if(n_features == 2){
            QString datafile = file_manager->getTrainFilepath()+".tmp";
            if(!runGetDencityColumn(file_manager->getTrainFilepath(), datafile)){
                plot_cmd<<"set dgrid3d 100,100;"<<"set contour base; set cntrparam bspline;"<<
                          "set style data lines; set view map; unset key; unset surface;"<<
                          "set cntrparam levels 15; splot \""<<datafile.toStdString()<<"\"; pause mouse close;";
                gp<<plot_cmd.str();
            }
        }else{
            return "";
        }
    }else{
        //plot with gnuplot
        if(n_features == 3){
            plot_cmd<<"splot '";
        }else{
            plot_cmd<<"plot '";
        }
        plot_cmd<<filepath.toStdString()<<".tmp' with points palette; pause mouse close";
        gp<<plot_cmd.str();

    }
    return to_remove;//remove file in d-tor
}

int ScriptQtManager::runHoldout(int type, QString dataset_filepath, int test_part_percent, QString train_part, QString test_part)
{
    QStringList args;
    if(type == 0){ // classification
        args<<QCoreApplication::applicationDirPath()+"/h4c.py"<<dataset_filepath<<
              QString::number(test_part_percent)<<train_part<<test_part;
    }else if(type == 1){ // regression
        args<<QCoreApplication::applicationDirPath()+"/h4r.py"<<dataset_filepath<<
              QString::number(test_part_percent)<<train_part<<test_part;
    }
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
    return py_script.exitCode();
}

int ScriptQtManager::runConvert2SvmFormat(QString filepath, QString outfilepath, QString sep, QString dec_sep, bool label_first)
{
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/convert2svm.py"<<filepath<<outfilepath<<
          sep<<dec_sep<<(label_first ? "1" : "0");
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
    return py_script.exitCode();
}

int ScriptQtManager::runGetDencityColumn(QString filepath, QString outfilepath)
{
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/dencity.py"<<filepath<<outfilepath;
    QProcess py_script;
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardError()+py_script.readAllStandardOutput());
    printf("%s", py_script_out.toLatin1().data());
    fflush(stdout);
    return py_script.exitCode();
}

void ScriptQtManager::runGridSearch(QString filepath, QString log2c, QString log2g, QString cv_nfold, bool animation)
{
    QStringList args;
    QProcess py_script;
    args<<QCoreApplication::applicationDirPath()+"/grid.py"<<"-out"<<"null"<<"-log2c"<<log2c<<"-log2g"<<log2g<<"-v"<<cv_nfold;
    if(!animation){
        args<<"-gnuplot"<<"null";
    }
    args<<filepath;
    py_script.setProcessChannelMode(QProcess::ForwardedOutputChannel);
//    py_script.startDetached("python3", args);
    py_script.start("python3", args);
    py_script.waitForFinished(60000);
}







