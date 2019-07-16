#include "availabilityhandler.h"

AvailabilityHandler::AvailabilityHandler(QHBoxLayout* params)
{
    degree = params->findChild<QHBoxLayout*>("degree_horizontalLayout");
    gamma = params->findChild<QHBoxLayout*>("gamma_horizontalLayout");
    coef0 = params->findChild<QHBoxLayout*>("coef0_horizontalLayout");
    C = params->findChild<QHBoxLayout*>("C_horizontalLayout");
    nu = params->findChild<QHBoxLayout*>("nu_horizontalLayout");
    p = params->findChild<QHBoxLayout*>("P_horizontalLayout");
}


void AvailabilityHandler::filterSVMTypeParams(int svm_type){
    switch(svm_type){
        case 0: // C-SVC
            C->itemAt(1)->widget()->setEnabled(true);
            nu->itemAt(1)->widget()->setEnabled(false);
            p->itemAt(1)->widget()->setEnabled(false);
        break;
        case 1: // nu-SVC
            C->itemAt(1)->widget()->setEnabled(false);
            nu->itemAt(1)->widget()->setEnabled(true);
            p->itemAt(1)->widget()->setEnabled(false);
        break;
        case 2: // one class SVM
            C->itemAt(1)->widget()->setEnabled(false);
            nu->itemAt(1)->widget()->setEnabled(true);
            p->itemAt(1)->widget()->setEnabled(false);
        break;
        case 3: // epsilon-SVR
            C->itemAt(1)->widget()->setEnabled(true);
            nu->itemAt(1)->widget()->setEnabled(false);
            p->itemAt(1)->widget()->setEnabled(true);
        break;
        case 4: // nu-SVR
            C->itemAt(1)->widget()->setEnabled(true);
            nu->itemAt(1)->widget()->setEnabled(true);
            p->itemAt(1)->widget()->setEnabled(false);
        break;
    }
}

void AvailabilityHandler::filterKernelParams(int kernel){
    switch(kernel){
        case 0: // Linear
            degree->itemAt(1)->widget()->setEnabled(true);
            gamma->itemAt(1)->widget()->setEnabled(false);
            coef0->itemAt(1)->widget()->setEnabled(false);
        break;
        case 1: // poly
            degree->itemAt(1)->widget()->setEnabled(false);
            gamma->itemAt(1)->widget()->setEnabled(true);
            coef0->itemAt(1)->widget()->setEnabled(true);
        break;
        case 2: // RBF
            degree->itemAt(1)->widget()->setEnabled(false);
            gamma->itemAt(1)->widget()->setEnabled(true);
            coef0->itemAt(1)->widget()->setEnabled(false);
        break;
        case 3: // Sigmoid
            degree->itemAt(1)->widget()->setEnabled(false);
            gamma->itemAt(1)->widget()->setEnabled(true);
            coef0->itemAt(1)->widget()->setEnabled(true);
        break;
        case 4: // Precomputed
            degree->itemAt(1)->widget()->setEnabled(false);
            gamma->itemAt(1)->widget()->setEnabled(false);
            coef0->itemAt(1)->widget()->setEnabled(false);
        break;
    }

}
