#ifndef AVAILABILITYHANDLER_H
#define AVAILABILITYHANDLER_H
#include <QList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QTabWidget>
#include <iostream>

enum {CV=1, SCALING=2, VISUALIZATION=4, FS=8};

class AvailabilityHandler
{
public:
    AvailabilityHandler(QHBoxLayout* params, QTabWidget* tabs, QPushButton* train_button, QPushButton* test_button);
    void filterTabs(unsigned int tab);
    void filterSVMTypeParams(int svm_type);
    void filterKernelParams(int kernel);
private:
    //parameters
    QHBoxLayout* degree;
    QHBoxLayout* gamma;
    QHBoxLayout* coef0;
    QHBoxLayout* C;
    QHBoxLayout* nu;
    QHBoxLayout* p;

    //buttons
    QPushButton* _train_button;
    QPushButton* _test_button;

    //tabs
    QWidget* cv_tab;
    QWidget* scaling_tab;
    QWidget* visualization_tab;
    QWidget* fs_tab;

};

#endif // AVAILABILITYHANDLER_H
