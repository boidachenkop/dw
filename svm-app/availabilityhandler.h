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
    AvailabilityHandler(QList<QHBoxLayout*> kernelParams);
private:
    QList<QHBoxLayout*> _kernelParams;
};

#endif // AVAILABILITYHANDLER_H
