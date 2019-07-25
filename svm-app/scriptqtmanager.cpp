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

#include "gnuplot_i.hpp"
std::string ScriptQtManager::runPlot(QString filepath, int n_features, std::vector<double> labels, bool density, double band_width)
{
    std::string to_remove = "";
    //prepare unlabled file
    std::string save_filepath = filepath.toStdString() + ".tmp";
    to_remove += save_filepath+" ";
    std::string prepare_data_cmd;
    if(n_features == 1){
        prepare_data_cmd = R"( awk '{if($2 == ""){$2="0.0"} gsub("[0-9]+:", "", $2); print $2, 0, $1}' )";
    }else if(n_features == 2){
        prepare_data_cmd = R"( awk '{if($3 == ""){$3="0.0"} if($2 == ""){$2="0.0"} gsub("[0-9]+:", "", $2); gsub("[0-9]+:", "", $3); print $2, $3, $1}' )";
    }else if(n_features == 3){
        prepare_data_cmd = R"( awk '{if($2 == ""){$2="0.0"} if($3 == ""){$3="0.0"} if($4 == ""){$4="0.0"} gsub("[0-9]+:", "", $2); gsub("[0-9]+:", "", $3); gsub("[0-9]+:", "", $4); print $2, $3, $4, $1}' )";
    }else{
        return "";
    }
    prepare_data_cmd+=filepath.toStdString()+" > " + save_filepath;
    system(prepare_data_cmd.c_str());

    Gnuplot gp;
    std::stringstream plot_cmd;
    if(density){
        if(labels.size() > 8){
            std::cout<<"Too many classes. Density plot is possible if dataset has less than 8 classes"<<std::endl;
            return "";
        }
        std::vector<std::string> colors{"red", "blue", "green", "black", "orange", "purple", "cyan", "coral"};
        std::vector<std::string> one_class_files;
        std::stringstream grepcmd;
        for(int i=0; i < (int)labels.size(); i++){
            one_class_files.push_back(save_filepath + std::to_string(i));
            grepcmd<<"grep ' "<<(labels[i] > 0 ? "+" : "")<<std::to_string((int)labels[i])<<"$' "<<save_filepath
                    <<" > "<<one_class_files.back()<<"; ";
            to_remove += one_class_files.back();
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
