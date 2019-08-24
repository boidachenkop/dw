#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QObject>
#include <QString>

#include <set>
#include <vector>


class FileManager : public QObject
{
    Q_OBJECT
public:
    FileManager();
    virtual ~FileManager(){}

    enum DATASET_TYPE{TRAIN, TEST};

    int setDatasetFilepath(QString, DATASET_TYPE);
    int setModelFilepath(QString);

    QString getTrainFilepath();
    QString getTestFilepath();
    QString getModelFilepath();
    std::vector<std::string> getLabels();
    int getNClasses();
    int getNFeatures();
    int getNLines();
    bool isTrainOK(){ return _trainFileOK; }
    bool isTestOK(){ return _testFileOK; }
signals:
    void updateTrainNFeatures(QString);
    void updateTrainNClasses(QString);
    void updateTrainNLines(QString);
    void updateTestNFeatures(QString);
    void updateTestNClasses(QString);
    void updateTestNLines(QString);
    void updateFeatureSelection(QString);
    void updateTrainInputFilepath(QString, bool);
    void updateTestInputFilepath(QString, bool);
    void updateModelFilepath(QString);

private:
    int parseFile(QString filepath);
    bool insertLabel(std::string line);

    bool _trainFileOK{true};
    bool _testFileOK{true};

    QString _train_input_filepath{};
    QString _test_input_filepath{};
    QString _model_filepath{};

    int _train_n_features{-1};
    int _test_n_features{-1};
    int _n_lines{-1};

    std::set<std::string> _labels;
};

#endif // FILEMANAGER_H
