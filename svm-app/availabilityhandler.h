#ifndef AVAILABILITYHANDLER_H
#define AVAILABILITYHANDLER_H
#include <QList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <iostream>


class AvailabilityHandler
{
public:
    AvailabilityHandler(QHBoxLayout* params);
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
};

#endif // AVAILABILITYHANDLER_H
