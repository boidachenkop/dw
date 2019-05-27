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


    void openTrainFile(std::string train_file_name);
    void openModelFile(std::string);
    void openPredictInputFile(std::string);
    void openPredictOutputFile(std::string);

    //parameters setters
    void setSVMType(int type);
    void setKernel(int kernel);

    void readProblem();
    void trainModel();

    void predict();

    SVMController& setDegree(int degree){param.degree=degree; return *this;}
    SVMController& setGamma(int gamma){param.gamma=gamma; return *this;}
    SVMController& setCoef0(int coef0){param.coef0=coef0; return *this;}
    SVMController& setNu(double nu){param.nu=nu; return *this;}
    SVMController& setCasheSize(double cs){param.cache_size=cs; return *this;}
    SVMController& setC(double C){param.C=C; return *this;}
    SVMController& setEps(double eps){param.eps=eps; return *this;}
    SVMController& setP(double p){param.p=p; return *this;}
    SVMController& setShrinking(bool shrink){param.shrinking=shrink; return *this;}
    SVMController& setProbability(bool probability)
        {param.probability=probability; return *this;}
    SVMController& setCrossvalidation(bool cv, int f){cross_validation=cv; nr_fold=f; return *this;}
    SVMController& setWeight(int weight_label, double weight);
    void setModelFilePath(std::string fp){model_file_path = fp;}

    struct svm_parameter& getParams();
    std::string getModelFileName(){ return model_file_path; }
    std::string getPredictInputFileName(){ return predict_input_file_path; }
    std::string getPredictOutputFileName(){ return predict_out_file_path; }
    std::string getTrainFileName(){ return train_file_path; }

    int correctPrediction(){ return correct; }
    int totalPrediction(){ return total; }

    void setPredictProbability(bool pp){predict_probability=pp;}

    void setPredictPrintFunction(int (*f)(const char* s, ...));
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
    std::string train_file_path;
    std::string predict_input_file_path;
    std::string predict_out_file_path{"./predict_out.txt"};
    std::string model_file_path{"./model"};

    bool input_file_name_set{false};
    struct svm_parameter param;
    struct svm_problem prob;
    struct svm_model *model{nullptr};
    struct svm_node *x_space{nullptr};
    int cross_validation{0};
    int nr_fold{0};

    char *line{nullptr};
    int max_line_len{1024};
    FILE* input_train{nullptr};
    FILE* input_predict{nullptr};
    FILE* output_predict{nullptr};

    bool model_read{false};

    //predict
    int correct{};
    int total{};
    int predict_probability{0};
    int max_nr_attr{64};
    struct svm_node *x;

    int (*info)(const char *fmt, ...); //print function
};



#endif // SVMCONTROLLER_H
