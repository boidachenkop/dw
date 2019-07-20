#include "scriptqtmanager.h"

int ScriptQtManager::runCheckData(QString filepath)
{
    QProcess py_script;
    QStringList args;
    args<<QCoreApplication::applicationDirPath()+"/checkdata.py"<<filepath;
    py_script.setStandardErrorFile("/tmp/jojo");
    py_script.setStandardErrorFile("/tmp/jojo");
    py_script.start("python3", args);
    py_script.waitForFinished();
    QString py_script_out(py_script.readAllStandardOutput());
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
    QString py_script_out(py_script.readAllStandardOutput()+py_script.readAllStandardError()); //DON'T NEED STD ERR ????????
    printf("%s", py_script_out.toLatin1().data());
}

#include "gnuplot_i.hpp"
std::string ScriptQtManager::runPlot(QString filepath, int n_features)
{
    //prepare unlabled file
    std::string prepare_data_cmd;
    if(n_features == 1){
        prepare_data_cmd = R"( awk '{if($2 == ""){$2="0.0"} gsub("[0-9]+:", "", $2); print $2, $1}' )";
    }else if(n_features == 2){
        prepare_data_cmd = R"( awk '{if($3 == ""){$3="0.0"} if($2 == ""){$2="0.0"} gsub("[0-9]+:", "", $2); gsub("[0-9]+:", "", $3); print $2, $3, $1}' )";
    }else if(n_features == 3){
        prepare_data_cmd = R"( awk '{if($2 == ""){$2="0.0"} if($3 == ""){$3="0.0"} if($4 == ""){$4="0.0"} gsub("[0-9]+:", "", $2); gsub("[0-9]+:", "", $3); gsub("[0-9]+:", "", $4); print $2, $3, $4, $1}' )";
    }
    prepare_data_cmd+=filepath.toStdString()+" > "+filepath.toStdString()+".tmp";
    system(prepare_data_cmd.c_str());

    //plot with gnuplot
    Gnuplot gp;
    std::string plot_cmd;
    if(n_features == 3){
        plot_cmd = "splot '";
    }else{
        plot_cmd = "plot '";
    }
    plot_cmd+=filepath.toStdString()+".tmp' with points palette; pause mouse close";
    gp<<plot_cmd;

    return filepath.toStdString()+".tmp ";//remove file in d-tor
}
