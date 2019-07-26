#include <QStringList>
#include <QProcess>
#include <QCoreApplication>
#include <stdlib.h>
#include <iostream>
#include <sstream>

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
}

std::string ScriptQtManager::runPlot(FileManager* file_manager, bool density, double band_width)
{
    for(auto item : file_manager->getLabels()){
        std::cout<<item<<std::endl;
    }
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
            std::cout<<"here"<<std::endl;
            plot_cmd<<"N = "<<n_lines<<"; Delta = "<<band_width<<"; "<<"TimeStart = time(0.0); set table $Data;  set samples N; plot '"
                   <<save_filepath<<R"('  u 1:2 w table;
                                    unset table
                                    print sprintf("Data generated: %.3f sec",time(0.0)-TimeStart)
                                    TimeStart = time(0.0)
                                    stats $Data nooutput
                                    RowCount = STATS_records
                                    array ColX[RowCount]
                                    array ColY[RowCount]
                                    array ColC[RowCount]
                                    do for [i=1:RowCount] {
                                    set table $Dummy
                                    plot $Data u (ColX[$0+1]=$1,0):(ColY[$0+1]=$2,0) with table
                                    unset table
                                    }
                                    print sprintf("Data put into arrays: %.3f sec",time(0.0)-TimeStart)


                                    # look at each datapoint and its sourrounding
                                    do for [i=1:RowCount] {
                                    # print sprintf("Datapoint %g of %g",i,RowCount)
                                    x0 = ColX[i]
                                    y0 = ColY[i]
                                    # count the datapoints with distances <Delta around the datapoint of interest
                                    set table $Occurrences
                                    plot $Data u ((abs(x0-$1)<Delta) & (abs(y0-$2)<Delta) ? 1 : 0):(1) smooth frequency
                                    unset table
                                    # extract the number from $Occurrences which will be used to color the datapoint
                                    set table $Dummmy
                                    plot $Occurrences u (c0=$2,0):($0) every ::1::1 with table
                                    unset table
                                    ColC[i] = c0
                                    }

                                    # put the arrays into a dataset again
                                    set print $Data
                                    do for [i=1:RowCount] {
                                    print sprintf("%g\t%g\t%g",ColX[i],ColY[i],ColC[i])
                                    }
                                    set print
                                    print sprintf("Duration: %.3f sec",time(0.0)-TimeStart)

                                    set palette rgb 33,13,10
                                    plot $Data u 1:2:3 w p ps 1 pt 7 lc palette z notitle
                                    #set dgrid3d 30,30
                                    #set hidden3d
                                    #splot $Data u 1:2:3 with lines lc palette z notitle, \
                                    #'fc2d.tmp' u 1:2:3
                                    ### end of code
                                    pause mouse close)";
            gp<<plot_cmd.str();
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
