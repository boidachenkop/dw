#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <string>


class FileManager
{
public:
    FileManager();
    std::string train_input_filepath{};
    std::string test_input_filepath{};
    std::string model_filepath{};
};

#endif // FILEMANAGER_H
