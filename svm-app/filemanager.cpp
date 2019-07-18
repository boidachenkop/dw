#include "filemanager.h"

FileManager::FileManager(QLabel* train_label, QLabel* test_label,
                         QLabel* model_label, QLineEdit* fs_lineEdit):
    _train_filepath_label(train_label),
    _test_filepath_label(test_label),
    _model_filepath_label(model_label),
    _fs_lineEdit(fs_lineEdit)
{
}

int FileManager::setTrainFilepath(QString filepath)
{
    //return 0 if file can be used othervise -1
    if(!filepath.isEmpty()){
        if(ScriptQtManager::runCheckData(filepath) == 0){
            _train_input_filepath = filepath;
            _model_filepath = filepath + ".model";
            _train_filepath_label->setStyleSheet("QLabel{color : black;}");
            _n_features = getNFeatures(filepath);

            _train_filepath_label->setText(filepath);
            _fs_lineEdit.setText("1-"+QString::number(_n_features));
            _model_filepath_label->setText(filepath + ".model");
            return 0;
        }else{
            _train_filepath_label->setStyleSheet("QLabel{color : red;}");
            _train_filepath_label->setText(filepath);
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
            _test_filepath_label->setStyleSheet("QLabel{color : black;}");
            _test_filepath_label->setText(filepath);
            _test_n_features = getNFeatures(filepath);
            if((_n_features != -1) && (_n_features != _test_n_features)){
                _test_filepath_label->setStyleSheet("QLabel{color : red;}");
                _test_filepath_label->repaint();
                return -1;
            }
            return 0;
        }else{
            _test_filepath_label->setStyleSheet("QLabel{color : red;}");
            _test_filepath_label->setText(filepath);
           return -1;
        }
    }else if(!_test_input_filepath.isEmpty()){
        return 0;
    }else{
        return -1;
    }
    return 0;
}

int FileManager::setModelFilepath(QString filepath)
{
    return 0;
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

int FileManager::getNFeatures()
{
   return _n_features;
}


int FileManager::getNFeatures(QString data_filepath){
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
