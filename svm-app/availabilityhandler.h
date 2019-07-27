#ifndef AVAILABILITYHANDLER_H
#define AVAILABILITYHANDLER_H
#include <QObject>
#include <iostream>

class AvailabilityHandler : public QObject
{
    Q_OBJECT
public:
    AvailabilityHandler();
    AvailabilityHandler& trainButtonEnabled(bool);
    AvailabilityHandler& testButtonEnabled(bool);
    AvailabilityHandler& cvTabEnabled(bool);
    AvailabilityHandler& scalingTabEnabled(bool);
    AvailabilityHandler& visualizationTabEnabled(bool);
    AvailabilityHandler& featureSelectionTabEnabled(bool);
    AvailabilityHandler& cvPercentLabelVisible(bool);
    AvailabilityHandler& cvSpinBoxEnabled(bool enabled);

public Q_SLOTS:
    void filterSVMTypeParams(int svm_type);
    void filterKernelParams(int kernel);

signals:
    void degreeEnabled(bool);
    void gammaEnabled(bool);
    void coef0Enabled(bool);
    void cEnabled(bool);
    void nuEnabled(bool);
    void pEnabled(bool);

    void trainEnabled(bool);
    void testEnabled(bool);
    void cvEnabled(bool);
    void scalingEnabled(bool);
    void visualizationEnabled(bool);
    void fsEnabled(bool);

    void cvPercentVisible(bool);
    void cvSBEnabled(bool);
};

#endif // AVAILABILITYHANDLER_H
