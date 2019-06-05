#include "svmvisualizer.h"
#include <iostream>

svmvisualizer::svmvisualizer(std::string data_filepath)
{
    data.open(data_filepath, std::fstream::in);
    n_dim = parseNDim();
}

bool svmvisualizer::parse(){
    if(n_dim == 1){
        for(std::string line; std::getline(data, line);){
            std::string y_, f1_;
            auto c = line.begin();
            while(*c != ' '){
                if(*c != '+'){
                    y_+=*c;
                }
                c++;
            }
            for(; c != line.end(); c++){
                if(*c == '+'){ c++; }
                if(*c == ':' && *(c-1) == '1'){
                    c++;
                    while(*c != ' '){
                        f1_+=*c;
                        c++;
                    }
                }
            }
            y.push_back(std::stod(y_));
            if(f1_.empty()){
                f1.push_back(0);
            }else{
                f1.push_back(std::stof(f1_));
            }
        }
    }else if(n_dim == 2){
        for(std::string line; std::getline(data, line);){
            std::string y_, f1_, f2_;
            auto c = line.begin();
            while(*c != ' '){
                if(*c != '+'){
                    y_+=*c;
                }
                c++;
            }
            for(; c != line.end(); c++){
                if(*c == '+'){ c++; }
                if(*c == ':' && *(c-1) == '1'){
                    c++;
                    while(*c != ' '){
                        f1_+=*c;
                        c++;
                    }
                }
                if(*c == ':' && *(c-1) == '2'){
                    c++;
                    while(*c != ' '){
                        f2_+=*c;
                        c++;
                    }
                }
            }
            y.push_back(std::stod(y_));
            if(f1_.empty()){
                f1.push_back(0);
            }else{
                f1.push_back(std::stof(f1_));
            }
            if(f2_.empty()){
                f2.push_back(0);
            }else{
                f2.push_back(std::stof(f2_));
            }
        }
    }else if(n_dim == 3){
        for(std::string line; std::getline(data, line);){
            std::string y_, f1_, f2_, f3_;
            auto c = line.begin();
            while(*c != ' '){
                if(*c != '+'){
                    y_+=*c;
                }
                c++;
            }
            for(; c != line.end(); c++){
                if(*c == '+'){ c++; }
                if(*c == ':' && *(c-1) == '1'){
                    c++;
                    while(*c != ' '){
                        f1_+=*c;
                        c++;
                    }
                }
                if(*c == ':' && *(c-1) == '2'){
                    c++;
                    while(*c != ' '){
                        f2_+=*c;
                        c++;
                    }
                }
                if(*c == ':' && *(c-1) == '3'){
                    c++;
                    while(*c != ' '){
                        f3_+=*c;
                        c++;
                    }
                }
            }
            y.push_back(std::stod(y_));
            if(f1_.empty()){
                f1.push_back(0);
            }else{
                f1.push_back(std::stof(f1_));
            }
            if(f2_.empty()){
                f2.push_back(0);
            }else{
                f2.push_back(std::stof(f2_));
            }
            if(f3_.empty()){
                f3.push_back(0);
            }else{
                f3.push_back(std::stof(f2_));
            }
        }
    }else{
        return false;
    }
    return true;
}

void svmvisualizer::visualize()
{

    if(n_dim == 1){

    }else if(n_dim == 2){

    }else if(n_dim == 3){

    }
}



int svmvisualizer::parseNDim(){
    std::string ndim_str;
    std::string line;
    std::getline(data, line);
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
    data.seekg(data.beg);//to file begining
    return stoi(ndim_str);
}

int svmvisualizer::getNDim(){
    return n_dim;
}
