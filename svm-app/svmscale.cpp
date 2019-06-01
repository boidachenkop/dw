#include "svmscale.h"

svmscale::svmscale(std::string input_fp, std::string out_fp):
    input_filepath{input_fp}, output_filepath{out_fp}
{
    fp = fopen(input_filepath.c_str(), "r");
    if(fp==NULL)
    {
        printf("can't open file %s\n", input_filepath.c_str());
    }
    fp_output = fopen(output_filepath.c_str(), "w");
    if(fp_output==NULL)
    {
        printf("can't open file %s\n", output_filepath.c_str());
    }
    line = (char *) malloc(max_line_len*sizeof(char));
    feature_max = (double *)malloc((max_index+1)* sizeof(double));
    feature_min = (double *)malloc((max_index+1)* sizeof(double));
}

bool svmscale::check()
{
    if(!(upper > lower) || (y_scaling && !(y_upper > y_lower)))
    {
        printf("inconsistent lower/upper specification\n");
        return false;
    }

    if(restore_filename && save_filename)
    {
        printf("cannot use -r and -s simultaneously\n");
        return false;
    }
    return true;
}

void svmscale::setSaveFilename(char *range_filepath)
{
   save_filename = range_filepath;
}

void svmscale::setRestoreFilename(char *range_filepath)
{
    restore_filename = range_filepath;
}

void svmscale::scale()
{
    max_index = 0;
    min_index = 1;
    if(restore_filename)
    {
        int idx, c;

        fp_restore = fopen(restore_filename,"r");
        if(fp_restore==NULL)
        {
            printf("can't open file %s\n", restore_filename);
            return;
        }

        c = fgetc(fp_restore);
        if(c == 'y')
        {
            readline(fp_restore);
            readline(fp_restore);
            readline(fp_restore);
        }
        readline(fp_restore);
        readline(fp_restore);

        while(fscanf(fp_restore,"%d %*f %*f\n",&idx) == 1)
            max_index = std::max(idx,max_index);
        rewind(fp_restore);
    }

    while(readline(fp)!=NULL)
    {
        char *p=line;

        SKIP_TARGET

        while(sscanf(p,"%d:%*f",&index)==1)
        {
            max_index = std::max(max_index, index);
            min_index = std::min(min_index, index);
            SKIP_ELEMENT
            num_nonzeros++;
        }
    }

    if(min_index < 1)
        printf("WARNING: minimal feature index is %d, but indices should start from 1\n", min_index);

    rewind(fp);

    feature_max = (double *)malloc((max_index+1)* sizeof(double));
    feature_min = (double *)malloc((max_index+1)* sizeof(double));

    if(feature_max == NULL || feature_min == NULL)
    {
        printf("can't allocate enough memory\n");
        return;
    }

    for(int i=0;i<=max_index;i++)
    {
        feature_max[i]=-DBL_MAX;
        feature_min[i]=DBL_MAX;
    }

    /* pass 2: find out min/max value */
    while(readline(fp)!=NULL)
    {
        char *p=line;
        int next_index=1;
        double target;
        double value;

        if (sscanf(p,"%lf",&target) != 1){
            clean_up(fp_restore, fp, "ERROR: failed to read labels\n");
            return;
        }
        y_max = std::max(y_max,target);
        y_min = std::min(y_min,target);

        SKIP_TARGET

        while(sscanf(p,"%d:%lf",&index,&value)==2)
        {
            for(int i=next_index;i<index;i++)
            {
                feature_max[i]=std::max(feature_max[i],0.0);
                feature_min[i]=std::min(feature_min[i],0.0);
            }

            feature_max[index]=std::max(feature_max[index],value);
            feature_min[index]=std::min(feature_min[index],value);

            SKIP_ELEMENT
            next_index=index+1;
        }

        for(int i=next_index;i<=max_index;i++)
        {
            feature_max[i]=std::max(feature_max[i],0.0);
            feature_min[i]=std::min(feature_min[i],0.0);
        }
    }

    rewind(fp);

    /* pass 2.5: save/restore feature_min/feature_max */

    if(restore_filename)
    {
        /* fp_restore rewinded in finding max_index */
        int idx, c;
        double fmin, fmax;
        int next_index = 1;

        if((c = fgetc(fp_restore)) == 'y')
        {
            if(fscanf(fp_restore, "%lf %lf\n", &y_lower, &y_upper) != 2 ||
               fscanf(fp_restore, "%lf %lf\n", &y_min, &y_max) != 2){
                clean_up(fp_restore, fp, "ERROR: failed to read scaling parameters\n");
                return;
            }
            y_scaling = 1;
        }
        else
            ungetc(c, fp_restore);

        if (fgetc(fp_restore) == 'x')
        {
            if(fscanf(fp_restore, "%lf %lf\n", &lower, &upper) != 2){
                clean_up(fp_restore, fp, "ERROR: failed to read scaling parameters\n");
                return;
            }
            while(fscanf(fp_restore,"%d %lf %lf\n",&idx,&fmin,&fmax)==3)
            {
                for(int i = next_index;i<idx;i++)
                    if(feature_min[i] != feature_max[i])
                    {
                        printf("WARNING: feature index %d appeared in file %s was not seen in the scaling factor file %s. The feature is scaled to 0.\n",
                            i, input_filepath.c_str(), restore_filename);
                        feature_min[i] = 0;
                        feature_max[i] = 0;
                    }

                feature_min[idx] = fmin;
                feature_max[idx] = fmax;

                next_index = idx + 1;
            }

            for(int i=next_index;i<=max_index;i++)
                if(feature_min[i] != feature_max[i])
                {
                    printf("WARNING: feature index %d appeared in file %s was not seen in the scaling factor file %s. The feature is scaled to 0.\n",
                        i, input_filepath.c_str(), restore_filename);
                    feature_min[i] = 0;
                    feature_max[i] = 0;
                }
        }
        fclose(fp_restore);
    }

    if(save_filename)
    {
        FILE *fp_save = fopen(save_filename,"w");
        if(fp_save==NULL)
        {
            printf("can't open file %s\n", save_filename);
            return;
        }
        if(y_scaling)
        {
            fprintf(fp_save, "y\n");
            fprintf(fp_save, "%.17g %.17g\n", y_lower, y_upper);
            fprintf(fp_save, "%.17g %.17g\n", y_min, y_max);
        }
        fprintf(fp_save, "x\n");
        fprintf(fp_save, "%.17g %.17g\n", lower, upper);
        for(int i=1;i<=max_index;i++)
        {
            if(feature_min[i]!=feature_max[i])
                fprintf(fp_save,"%d %.17g %.17g\n",i,feature_min[i],feature_max[i]);
        }

        if(min_index < 1)
            printf("WARNING: scaling factors with indices smaller than 1 are not stored to the file %s.\n", save_filename);

        fclose(fp_save);
    }

    /* pass 3: scale */
    while(readline(fp)!=NULL)
    {
        char *p=line;
        int next_index=1;
        double target;
        double value;

        if (sscanf(p,"%lf",&target) != 1){
            clean_up(NULL, fp, "ERROR: failed to read labels\n");
            return;
        }
        output_target(target);

        SKIP_TARGET

        while(sscanf(p,"%d:%lf",&index,&value)==2)
        {
            for(int i=next_index;i<index;i++)
                output(i,0);

            output(index,value);

            SKIP_ELEMENT
            next_index=index+1;
        }

        for(int i=next_index;i<=max_index;i++)
            output(i,0);

        fprintf(fp_output, "\n");
    }

    if (new_num_nonzeros > num_nonzeros)
        printf("WARNING: original #nonzeros %ld\n"
            "       > new      #nonzeros %ld\n"
            "If feature values are non-negative and sparse, use -l 0 rather than the default -l -1\n",
            num_nonzeros, new_num_nonzeros);
    free(line);
    free(feature_max);
    free(feature_min);
    fclose(fp);
    fclose(fp_output);
}

void svmscale::setLowerUpper(double lower, double upper)
{
    this->lower = lower;
    this->upper = upper;
}

void svmscale::setYLowerUpper(double lower, double upper)
{
   y_scaling = true;
   y_lower = lower;
   y_upper = upper;
}

char* svmscale::readline(FILE *input)
{
    int len;

    if(fgets(line,max_line_len,input) == NULL)
        return NULL;

    while(strrchr(line,'\n') == NULL)
    {
        max_line_len *= 2;
        line = (char *) realloc(line, max_line_len);
        len = (int) strlen(line);
        if(fgets(line+len,max_line_len-len,input) == NULL)
            break;
    }
    return line;
}

void svmscale::output_target(double value)
{
    if(y_scaling)
    {
        if(value == y_min)
            value = y_lower;
        else if(value == y_max)
            value = y_upper;
        else value = y_lower + (y_upper-y_lower) *
                 (value - y_min)/(y_max-y_min);
    }
    fprintf(fp_output, "%.17g ",value);
}

void svmscale::output(int index, double value)
{
    /* skip single-valued attribute */
    if(feature_max[index] == feature_min[index])
        return;

    if(value == feature_min[index])
        value = lower;
    else if(value == feature_max[index])
        value = upper;
    else
        value = lower + (upper-lower) *
            (value-feature_min[index])/
            (feature_max[index]-feature_min[index]);

    if(value != 0)
    {
        fprintf(fp_output,"%d:%g ",index, value);
        new_num_nonzeros++;
    }
}

int svmscale::clean_up(FILE *fp_restore, FILE *fp, const char* msg)
{
    printf("%s", msg);
    free(line);
    free(feature_max);
    free(feature_min);
    fclose(fp);
    if (fp_restore)
        fclose(fp_restore);
    return -1;
}
