#ifndef SVMCONTROLLER_H
#define SVMCONTROLLER_H
#include <string>
#include <cstring>
#include <exception>
#include <errno.h>
#include "svm.h"

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

class SVMController{
public:
    SVMController();

    ~SVMController();

    void openTrainFile(std::string);
    void openModelFile(std::string);
    void openPredictInputFile(std::string);
    void openPredictOutputFile(std::string);

    //parameters setters
    void setSVMType(int type);
    void setKernel(int kernel);

    //core functions
    void readProblem();
    void trainModel();

    void predict();

    //setters
    SVMController& setDegree(int degree){param.degree=degree; return *this;}
    SVMController& setGamma(double gamma){param.gamma=gamma; return *this;}
    SVMController& setCoef0(double coef0){param.coef0=coef0; return *this;}
    SVMController& setNu(double nu){param.nu=nu; return *this;}
    SVMController& setCasheSize(double cs){param.cache_size=cs; return *this;}
    SVMController& setC(double C){param.C=C; return *this;}
    SVMController& setEps(double eps){param.eps=eps; return *this;}
    SVMController& setP(double p){param.p=p; return *this;}
    SVMController& setShrinking(bool shrink){param.shrinking=shrink; return *this;}
    SVMController& setProbability(bool probability){param.probability=probability; return *this;}
    SVMController& setCrossvalidation(bool cv, int f){cross_validation=cv; n_fold=f; return *this;}
    SVMController& setNFeatures(int n_features){this->n_features = n_features; return *this;}
    SVMController& setWeight(int weight_label, double weight);
    void setModelFilePath(std::string fp){model_file_path = fp;}
    void setPredictProbability(bool pp){predict_probability=pp;}
    void setPredictPrintFunction(int (*f)(const char* s, ...));

    //getters
    struct svm_parameter& getParams();
    bool isCrossvalidation(){return cross_validation;}
    int getNFold(){return n_fold;}
    std::string getModelFilePath(){return model_file_path;}
    std::string getPredictInputFilePath(){return predict_input_file_path;}
    std::string getPredictOutputFilePath(){return predict_out_file_path;}
    std::string getTrainFilePath(){return train_file_path;}
    int getCorrectPredicted(){return correct;}
    int getTotalPredicted(){return total;}
    int getNFeatures(){return n_features;}

private:
    void setDefaultParams();
    void doCrossValidation();

    char* readLine(FILE*);

private:
    struct input_error : public std::exception {
        char* errstr;
        input_error(int line_num){
            sprintf(errstr, "Wrong input format at line %d\n", line_num);
        }
        const char * what () const throw () {
          return errstr;
        }
    };

private:
    //files
    std::string train_file_path;
    std::string predict_input_file_path;
    std::string predict_out_file_path{"/tmp/predict_out.txt"};
    std::string model_file_path{"./model"};
    FILE* input_train{nullptr};
    FILE* input_predict{nullptr};
    FILE* output_predict{nullptr};
    bool input_file_name_set{false};

    //params
    struct svm_parameter param;
    struct svm_problem prob;
    struct svm_model *model{nullptr};
    struct svm_node *x_space{nullptr};
    int cross_validation{0};
    int n_fold{0};
    int n_features{-1};

    char *line{nullptr};
    int max_line_len{1024};
    bool model_read{false};

    //predict
    int correct{};
    int total{};
    int predict_probability{0};
    int max_nr_attr{64};
    struct svm_node *x;
    int (*info)(const char *fmt, ...); //predict print function(printf default)
};



#endif // SVMCONTROLLER_H
