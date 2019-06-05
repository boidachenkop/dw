#ifndef VISUALIZER_H
#define VISUALIZER_H
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

class svmvisualizer
{
public:
    svmvisualizer(std::string data_filepath);

    int getNDim();
    bool parse();
    void visualize();
private:
    int parseNDim();
private:
    std::ifstream data;
    //features
    std::vector<double> f1;
    std::vector<double> f2;
    std::vector<double> f3;
    std::vector<double> y;
    int n_dim;
};

#endif // VISUALIZER_H
