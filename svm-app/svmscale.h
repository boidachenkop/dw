#ifndef SVMSCALE_H
#define SVMSCALE_H
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <algorithm>

class svmscale
{
public:
    svmscale();

private:
    double lower{-1.0};
    double upper{1.0};

    int y_scaling{0};
    double y_lower, y_upper;
    double y_max = -DBL_MAX;
    double y_min = DBL_MAX;
    int max_index, min_index;

    long int num_nonzeros = 0;
    long int new_num_nonzeros = 0;

    FILE *fp, *fp_restore = NULL;
    char *save_filename = NULL;
    char *restore_filename = NULL;
private:
    void output_target(double value);
    void output(int index, double value);
    char* readline(FILE *input);
    int clean_up(FILE *fp_restore, FILE *fp, const char *msg);
};

#endif // SVMSCALE_H
