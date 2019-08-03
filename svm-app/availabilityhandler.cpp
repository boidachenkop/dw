#include "availabilityhandler.h"

AvailabilityHandler::AvailabilityHandler()
{}

AvailabilityHandler& AvailabilityHandler::trainButtonEnabled(bool enabled) { emit trainEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::testButtonEnabled(bool enabled) { emit testEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::cvTabEnabled(bool enabled){ emit cvEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::scalingTabEnabled(bool enabled) { emit scalingEnabled(enabled);  return *this; }

AvailabilityHandler& AvailabilityHandler::visualizationTabEnabled(bool enabled) { emit visualizationEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::featureSelectionTabEnabled(bool enabled) { emit fsEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::cvPercentLabelVisible(bool visible) { emit cvPercentVisible(visible); return *this; }

AvailabilityHandler& AvailabilityHandler::cvSpinBoxEnabled(bool enabled) { emit cvSBEnabled(enabled); return *this; }

AvailabilityHandler &AvailabilityHandler::trainInfoLabelsVisible(bool visible) { emit trainInfoVisible(visible); return *this; }

AvailabilityHandler &AvailabilityHandler::testInfoLabelsVisible(bool visible) { emit testInfoVisible(visible); return *this; }

void AvailabilityHandler::filterSVMTypeParams(int svm_type){
    switch(svm_type){
        case 0: // C-SVC
            emit cEnabled(true);
            emit nuEnabled(false);
            emit pEnabled(false);
        break;
        case 1: // nu-SVC
            emit cEnabled(false);
            emit nuEnabled(true);
            emit pEnabled(false);
        break;
        case 2: // one class SVM
            emit cEnabled(false);
            emit nuEnabled(true);
            emit pEnabled(false);
        break;
        case 3: // epsilon-SVR
            emit cEnabled(true);
            emit nuEnabled(false);
            emit pEnabled(true);
        break;
        case 4: // nu-SVR
            emit cEnabled(true);
            emit nuEnabled(true);
            emit pEnabled(false);
        break;
    }
}

void AvailabilityHandler::filterKernelParams(int kernel){
    switch(kernel){
        case 0: // Linear
            emit degreeEnabled(true);
            emit gammaEnabled(false);
            emit coef0Enabled(false);
        break;
        case 1: // poly
            emit degreeEnabled(false);
            emit gammaEnabled(true);
            emit coef0Enabled(true);
        break;
        case 2: // RBF
            emit degreeEnabled(false);
            emit gammaEnabled(true);
            emit coef0Enabled(false);
        break;
        case 3: // Sigmoid
            emit degreeEnabled(false);
            emit gammaEnabled(true);
            emit coef0Enabled(true);
        break;
        case 4: // Precomputed
            emit degreeEnabled(false);
            emit gammaEnabled(false);
            emit coef0Enabled(false);
        break;
    }
}
