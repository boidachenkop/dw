#ifndef AVAILABILITYHANDLER_H
#define AVAILABILITYHANDLER_H
#include <QList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QTabWidget>
#include <iostream>

enum {CV=1, SCALING=2, VISUALIZATION=4, FS=8};
enum {TEST=1, TRAIN=2};

class AvailabilityHandler
{
public:
    AvailabilityHandler(QHBoxLayout* params, QTabWidget* tabs, QPushButton* train_button, QPushButton* test_button);
    AvailabilityHandler& trainButtonEnabled(bool);
    AvailabilityHandler& testButtonEnabled(bool);
    AvailabilityHandler& cvTabEnabled(bool);
    AvailabilityHandler& scalingTabEnabled(bool);
    AvailabilityHandler& visualizationTabEnabled(bool);
    AvailabilityHandler& featureSelectionTabEnabled(bool);
    void filterSVMTypeParams(int svm_type);
    void filterKernelParams(int kernel);
private:
    //parameters
    QHBoxLayout* _degree;
    QHBoxLayout* _gamma;
    QHBoxLayout* _coef0;
    QHBoxLayout* _C;
    QHBoxLayout* _nu;
    QHBoxLayout* _P;

    //buttons
    QPushButton* _train_button;
    QPushButton* _test_button;

    //tabs
    QWidget* _cv_tab;
    QWidget* _scaling_tab;
    QWidget* _visualization_tab;
    QWidget* _fs_tab;
};

#endif // AVAILABILITYHANDLER_H
