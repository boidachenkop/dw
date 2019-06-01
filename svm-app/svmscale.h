#ifndef SVMSCALE_H
#define SVMSCALE_H
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <algorithm>

#define SKIP_TARGET\
    while(isspace(*p)) ++p;\
    while(!isspace(*p)) ++p;

#define SKIP_ELEMENT\
    while(*p!=':') ++p;\
    ++p;\
    while(isspace(*p)) ++p;\
    while(*p && !isspace(*p)) ++p;

class svmscale
{
public:
    svmscale(std::string in, std::string out);
    bool check();
    void scale();

    //setters
    void setLowerUpper(double lower, double upper);
    void setYLowerUpper(double lower, double upper);
    void setSaveFilename(char* range_filepath);
    void setRestoreFilename(char* range_filepath);
    void setOutputFilePath(std::string ofpath);

private:
    double lower{-1.0};
    double upper{1.0};
    double *feature_max, *feature_min;

    bool y_scaling{false};
    double y_lower, y_upper;
    double y_max{-DBL_MAX};
    double y_min{DBL_MAX};

    int index;
    int max_index, min_index;

    long int num_nonzeros{0};
    long int new_num_nonzeros{0};

    FILE *fp, *fp_restore{nullptr};
    FILE *fp_output;
    char *save_filename{nullptr};
    char *restore_filename{nullptr};
    std::string input_filepath;
    std::string output_filepath;

    char *line{nullptr};
    int max_line_len{1024};
private:
    char* readline(FILE *input);
    void output_target(double value);
    void output(int index, double value);
    int clean_up(FILE *fp_restore, FILE *fp, const char *msg);
};

#endif // SVMSCALE_H
