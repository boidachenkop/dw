#include "svmcontroller.h"

SVMController::SVMController(){
    setDefaultParams();
}

SVMController::~SVMController(){
    if(model){
        svm_free_and_destroy_model(&model);
    }
    svm_destroy_param(&param);
    free(prob.y);
    free(prob.x);
    free(x_space);
    free(line);
    free(x);

    if(input_train){ fclose(input_train); }
    if(input_predict){ fclose(input_predict); }
    if(output_predict){ fclose(output_predict); }
}

void SVMController::openTrainFile(std::string ifn){
   train_file_path = ifn;
   input_file_name_set = true;
}

struct svm_parameter& SVMController::getParams(){
    return param;
}

void SVMController::setDefaultParams(){
    param.svm_type = C_SVC;
    param.kernel_type = RBF;

    param.degree = 3;
    param.gamma = 0;
    param.coef0 = 0;

    param.nu = 0.5;
    param.cache_size = 100;
    param.C = 1;
    param.eps = 1e-3;
    param.p = 0.1;
    param.shrinking = 1;
    param.probability = 0;
    param.nr_weight = 0;
    param.weight_label = nullptr;
    param.weight = NULL;

    //predict
    x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
}

char* SVMController::readLine(FILE* input){
    int len;

    if(fgets(line,max_line_len,input) == nullptr)
        return nullptr;

    while(strrchr(line,'\n') == nullptr)
    {
        max_line_len *= 2;
        line = (char *) realloc(line,max_line_len);
        len = (int) strlen(line);
        if(fgets(line+len,max_line_len-len,input) == NULL)
            break;
    }
    return line;
}

void SVMController::readProblem(){
    int max_index, inst_max_index, i;
    size_t elements, j;
    input_train = fopen(train_file_path.c_str(),"r");
    char *endptr;
    char *idx, *val, *label;

    if(input_train == nullptr)
    {
        fprintf(stderr,"can't open input file %s\n",train_file_path.c_str());
        exit(1);
    }

    prob.l = 0;
    elements = 0;

    line = Malloc(char,max_line_len);
    while(readLine(input_train)!=nullptr)
    {
        char *p = strtok(line," \t"); // label

        // features
        while(1)
        {
            p = strtok(nullptr," \t");
            if(p == nullptr || *p == '\n') // check '\n' as ' ' may be after the last feature
                break;
            ++elements;
        }
        ++elements;
        ++prob.l;
    }
    rewind(input_train);

    prob.y = Malloc(double,prob.l);
    prob.x = Malloc(struct svm_node *,prob.l);
    x_space = Malloc(struct svm_node,elements);

    max_index = 0;
    j=0;
    for(i=0;i<prob.l;i++)
    {
        inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
        readLine(input_train);
        prob.x[i] = &x_space[j];
        label = strtok(line," \t\n");
        if(label == nullptr) // empty line
            throw input_error(i+1);

        prob.y[i] = strtod(label,&endptr);
        if(endptr == label || *endptr != '\0')
            throw input_error(i+1);

        while(1)
        {
            idx = strtok(nullptr,":");
            val = strtok(nullptr," \t");
            if(val == nullptr)
            break;

            errno = 0;
            x_space[j].index = (int) strtol(idx,&endptr,10);
            if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
            throw input_error(i+1);
            else
            inst_max_index = x_space[j].index;

            errno = 0;
            x_space[j].value = strtod(val,&endptr);
            if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
            throw input_error(i+1);

            ++j;
        }

        if(inst_max_index > max_index)
            max_index = inst_max_index;
        x_space[j++].index = -1;
    }

    if(param.gamma == 0 && max_index > 0)
        param.gamma = 1.0/max_index;

    if(param.kernel_type == PRECOMPUTED)
        for(i=0;i<prob.l;i++)
        {
            if (prob.x[i][0].index != 0)
            {
                fprintf(stderr,"Wrong input format: first column must be 0:sample_serial_number\n");
                exit(1);
            }
            if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
            {
                fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
                exit(1);
            }
        }

    model_read = true;
}

void SVMController::doCrossValidation(){
    int i;
    int total_correct = 0;
    double total_error = 0;
    double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
    double *target = Malloc(double,prob.l);

    svm_cross_validation(&prob,&param,n_fold,target);
    if(param.svm_type == EPSILON_SVR ||
       param.svm_type == NU_SVR)
    {
        for(i=0;i<prob.l;i++)
        {
            double y = prob.y[i];
            double v = target[i];
            total_error += (v-y)*(v-y);
            sumv += v;
            sumy += y;
            sumvv += v*v;
            sumyy += y*y;
            sumvy += v*y;
        }
        printf("Cross Validation Mean squared error = %g\n",total_error/prob.l);
        printf("Cross Validation Squared correlation coefficient = %g\n",
            ((prob.l*sumvy-sumv*sumy)*(prob.l*sumvy-sumv*sumy))/
            ((prob.l*sumvv-sumv*sumv)*(prob.l*sumyy-sumy*sumy))
            );
    }
    else
    {
        for(i=0;i<prob.l;i++)
            if(target[i] == prob.y[i])
                ++total_correct;
        printf("Cross Validation Accuracy = %g%%\n",100.0*total_correct/prob.l);
    }
    fflush(stdout);
    free(target);
}

void SVMController::trainModel(){
    if(cross_validation)
    {
        doCrossValidation();
    }
    else
    {
        if(model){
            svm_free_and_destroy_model(&model);
        }
        model = svm_train(&prob,&param);
        if(svm_save_model(model_file_path.c_str(),model))
        {
            fprintf(stderr, "can't save model to file %s\n", model_file_path.c_str());
            exit(1);
        }
    }
}

void SVMController::setSVMType(int type){
    switch(type){
        case 0:
            param.svm_type = C_SVC;
            break;
        case 1:
            param.svm_type = NU_SVC;
            break;
        case 2:
            param.svm_type = ONE_CLASS;
            break;
        case 3:
            param.svm_type = EPSILON_SVR;
            break;
        case 4:
            param.svm_type = NU_SVR;
            break;
    }
}

void SVMController::setKernel(int kernel){
    switch(kernel){
        case 0:
            param.kernel_type = LINEAR;
            break;
        case 1:
            param.kernel_type = POLY;
            break;
        case 2:
            param.kernel_type = RBF;
            break;
        case 3:
            param.kernel_type = SIGMOID;
            break;
        case 4:
            param.kernel_type = PRECOMPUTED;
            break;
    }
}

SVMController& SVMController::setWeight(int weight_label, double weight){
    param.nr_weight++;
    param.weight_label = (int *)realloc(param.weight_label,
            sizeof(int)*param.nr_weight);

    param.weight = (double *)realloc(param.weight,sizeof(double)*param.nr_weight);
    param.weight_label[param.nr_weight-1] = weight_label;
    param.weight[param.nr_weight-1] = weight;
    return *this;
}

void SVMController::predict(){
    if(predict_probability)
    {
        if(svm_check_probability_model(model)==0)
        {
            fprintf(stderr,"Model does not support probabiliy estimates\n");
            exit(1);
        }
    }
    else
    {
        if(svm_check_probability_model(model)!=0)
            printf("Model supports probability estimates, but disabled in prediction.\n");
    }
    correct = 0;
    total = 0;
    double error = 0;
    double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

    int svm_type=svm_get_svm_type(model);
    int nr_class=svm_get_nr_class(model);
    double *prob_estimates=NULL;
    int j;
    if(predict_probability)
    {
        if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
            printf("Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma=%g\n",svm_get_svr_probability(model));
        else
        {
            int *labels=(int *) malloc(nr_class*sizeof(int));
            svm_get_labels(model,labels);
            prob_estimates = (double *) malloc(nr_class*sizeof(double));
            fprintf(output_predict,"labels");
            for(j=0;j<nr_class;j++)
                fprintf(output_predict," %d",labels[j]);
            fprintf(output_predict,"\n");
            free(labels);
        }
    }

    max_line_len = 1024;
    line = (char *)malloc(max_line_len*sizeof(char));
    while(readLine(input_predict) != NULL)
    {
        int i = 0;
        double target_label, predict_label;
        char *idx, *val, *label, *endptr;
        int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

        label = strtok(line," \t\n");
        if(label == NULL) // empty line
            throw input_error(total+1);

        target_label = strtod(label,&endptr);
        if(endptr == label || *endptr != '\0')
            throw input_error(total+1);

        while(1)
        {
            if(i>=max_nr_attr-1)	// need one more for index = -1
            {
                max_nr_attr *= 2;
                x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
            }

            idx = strtok(NULL,":");
            val = strtok(NULL," \t");

            if(val == NULL)
                break;
            errno = 0;
            x[i].index = (int) strtol(idx,&endptr,10);
            if(endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
                throw input_error(total+1);
            else
                inst_max_index = x[i].index;

            errno = 0;
            x[i].value = strtod(val,&endptr);
            if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
                throw input_error(total+1);

            ++i;
        }
        x[i].index = -1;

        if (predict_probability && (svm_type==C_SVC || svm_type==NU_SVC))
        {
            predict_label = svm_predict_probability(model,x,prob_estimates);
            fprintf(output_predict,"%g",predict_label);
            for(j=0;j<nr_class;j++)
                fprintf(output_predict," %g",prob_estimates[j]);
            fprintf(output_predict,"\n");
        }
        else
        {
            predict_label = svm_predict(model,x);
            fprintf(output_predict,"%.17g\n",predict_label);
        }

        if(predict_label == target_label)
            ++correct;
        error += (predict_label-target_label)*(predict_label-target_label);
        sump += predict_label;
        sumt += target_label;
        sumpp += predict_label*predict_label;
        sumtt += target_label*target_label;
        sumpt += predict_label*target_label;
        ++total;
    }
    if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
    {
        printf("Mean squared error = %g (regression)\n",error/total);
        printf("Squared correlation coefficient = %g (regression)\n",
            ((total*sumpt-sump*sumt)*(total*sumpt-sump*sumt))/
            ((total*sumpp-sump*sump)*(total*sumtt-sumt*sumt))
            );
    }
    else{
        printf("Accuracy = %g%% (%d/%d) (classification)\n",
            (double)correct/total*100,correct,total);
        fflush(stdout);
    }
    if(predict_probability)
        free(prob_estimates);
}

void SVMController::openModelFile(std::string input){
    model_file_path = input;
    if(model){
        svm_free_and_destroy_model(&model);
    }
    model=svm_load_model(model_file_path.c_str());
}

void SVMController::openPredictInputFile(std::string input){
    predict_input_file_path = input;
    input_predict = fopen(predict_input_file_path.c_str(),"r");
    if(input_predict == NULL)
    {
        fprintf(stderr,"can't open input file %s\n", predict_input_file_path.c_str());
        exit(1);
    }
}

void SVMController::openPredictOutputFile(std::string input){
    predict_out_file_path = input;
    output_predict = fopen(predict_out_file_path.c_str(),"w");
    if(output_predict == NULL)
    {
        fprintf(stderr,"can't open output file %s\n", predict_out_file_path.c_str());
        exit(1);
    }
}
