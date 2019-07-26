#include <QLabel>
#include <QLineEdit>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cctype>

#include "scriptqtmanager.h"
#include "filemanager.h"

FileManager::FileManager()
{
}

int FileManager::setTrainFilepath(QString filepath)
{
    //return 0 if file can be used othervise -1
    if(!filepath.isEmpty()){
        if(ScriptQtManager::runCheckData(filepath) == 0){
            _train_input_filepath = filepath;
            _model_filepath = filepath + ".model";
            _n_features = parseFile(filepath);

            emit updateTrainInputFilepath(filepath, true);
            emit updateModelFilepath(filepath + ".model");
            emit updateNLines(_n_lines);
            emit updateNClasses(getNClasses());
            emit updateNFeatures(_n_features);
            return 0;
        }else{
            emit updateTrainInputFilepath(filepath, false);
            return -1;
        }
    }else if(!_train_input_filepath.isEmpty()){
        return 0;
    }else{
        return -1;
    }
}

int FileManager::setTestFilepath(QString filepath)
{
    if(!filepath.isEmpty()){
        if(ScriptQtManager::runCheckData(filepath) == 0){
            _test_input_filepath = filepath;
            emit updateTestInputFilepath(filepath, true);
            _test_n_features = parseFile(filepath);
            if((_n_features != -1) && (_n_features != _test_n_features)){
                emit updateTestInputFilepath(filepath, false);
                return -1;
            }
            return 0;
        }else{
            emit updateTestInputFilepath(filepath, false);
            return -1;
        }
    }else if(!_test_input_filepath.isEmpty()){
        return 0;
    }else{
        return -1;
    }
}

int FileManager::setModelFilepath(QString filepath)
{
    if(!filepath.isEmpty()){
        _model_filepath = filepath;
//        _model_filepath_label->setText(filepath);
        emit updateModelFilepath(filepath);
        return 0;
    }else if(!_model_filepath.isEmpty()){
        return 0;
    }else if(!_train_input_filepath.isEmpty()){
        _model_filepath = _train_input_filepath + ".model";
        return 0;
    }else{
        return -1;
    }
}

QString FileManager::getTrainFilepath()
{
    return _train_input_filepath;
}

QString FileManager::getTestFilepath()
{
    return _test_input_filepath;
}

QString FileManager::getModelFilepath()
{
    return _model_filepath;
}

std::vector<std::string> FileManager::getLabels()
{
    return std::vector<std::string>(_labels.begin(), _labels.end());
}

int FileManager::getNClasses()
{
    return static_cast<int>(_labels.size());
}

int FileManager::getNFeatures()
{
   return _n_features;
}

int FileManager::getNLines(){
    return _n_lines;
}


int FileManager::parseFile(QString data_filepath){
    //read file and parse number of features
    std::fstream data;
    data.open(data_filepath.toStdString(), std::fstream::in);
    if(!data.is_open()){
       printf("Can't open file %s", data_filepath.toLatin1().data());
       return -1;
    }
    std::string ndim_str;
    std::string line;
    int max_dim=0;
    int n_lines=0;
    while(std::getline(data, line)){
        if(!insertLabel(line)){
            std::cout<<"Line: "<<n_lines+1<<" Label can't be read."<<std::endl;
        }
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
        n_lines++;
        reverse(ndim_str.begin(), ndim_str.end());
        int cur_dim = stoi(ndim_str);
        max_dim = max_dim < cur_dim ? cur_dim : max_dim;
        ndim_str = "";
    }
    data.close();
    _n_lines = n_lines;
    return max_dim;
}

bool FileManager::insertLabel(std::string line)
{
    std::string label;
    auto c = line.begin();
    while(!isspace(*c) && *c != '\0'){
       label+=*c;
       c++;
    }
    try{
        std::stod(label);
        _labels.insert(label);
        label.clear();
        return true;
    }catch(std::exception &e){
        label.clear();
        return false;
    }
}

