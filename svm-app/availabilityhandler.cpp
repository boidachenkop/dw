#include "availabilityhandler.h"

AvailabilityHandler::AvailabilityHandler(QHBoxLayout* params, QTabWidget* tabs,
                                         QPushButton* train_button, QPushButton* test_button):
    _train_button(train_button), _test_button(test_button)
{
    //initialize parameters by name
    _degree = params->findChild<QHBoxLayout*>("degree_horizontalLayout");
    _gamma = params->findChild<QHBoxLayout*>("gamma_horizontalLayout");
    _coef0 = params->findChild<QHBoxLayout*>("coef0_horizontalLayout");
    _C = params->findChild<QHBoxLayout*>("C_horizontalLayout");
    _nu = params->findChild<QHBoxLayout*>("nu_horizontalLayout");
    _P = params->findChild<QHBoxLayout*>("P_horizontalLayout");

    //initialize tabs by name
    for(int i=0; i<tabs->count(); i++){
       if(tabs->tabText(i) == "CV"){
            _cv_tab = tabs->widget(i);
       }else if(tabs->tabText(i) == "Scaling"){
            _scaling_tab = tabs->widget(i);
       }else if(tabs->tabText(i) == "Visualization"){
            _visualization_tab = tabs->widget(i);
       }else if(tabs->tabText(i) == "Feature Selection"){
            _fs_tab = tabs->widget(i);
       }else{
           std::cerr<<"Not registered tab detected!"<<std::endl;
       }
    }

    //initialize buttons ?by name?
    _train_button = train_button;
    _test_button = test_button;
}

AvailabilityHandler& AvailabilityHandler::trainButtonEnabled(bool enabled) { _train_button->setEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::testButtonEnabled(bool enabled) { _test_button->setEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::cvTabEnabled(bool enabled){ _cv_tab->setEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::scalingTabEnabled(bool enabled) { _scaling_tab->setEnabled(enabled);  return *this; }

AvailabilityHandler& AvailabilityHandler::visualizationTabEnabled(bool enabled) { _visualization_tab->setEnabled(enabled); return *this; }

AvailabilityHandler& AvailabilityHandler::featureSelectionTabEnabled(bool enabled) { _fs_tab->setEnabled(enabled); return *this; }

void AvailabilityHandler::filterSVMTypeParams(int svm_type){
    switch(svm_type){
        case 0: // C-SVC
            _C->itemAt(1)->widget()->setEnabled(true);
            _nu->itemAt(1)->widget()->setEnabled(false);
            _P->itemAt(1)->widget()->setEnabled(false);
        break;
        case 1: // nu-SVC
            _C->itemAt(1)->widget()->setEnabled(false);
            _nu->itemAt(1)->widget()->setEnabled(true);
            _P->itemAt(1)->widget()->setEnabled(false);
        break;
        case 2: // one class SVM
            _C->itemAt(1)->widget()->setEnabled(false);
            _nu->itemAt(1)->widget()->setEnabled(true);
            _P->itemAt(1)->widget()->setEnabled(false);
        break;
        case 3: // epsilon-SVR
            _C->itemAt(1)->widget()->setEnabled(true);
            _nu->itemAt(1)->widget()->setEnabled(false);
            _P->itemAt(1)->widget()->setEnabled(true);
        break;
        case 4: // nu-SVR
            _C->itemAt(1)->widget()->setEnabled(true);
            _nu->itemAt(1)->widget()->setEnabled(true);
            _P->itemAt(1)->widget()->setEnabled(false);
        break;
    }
}

void AvailabilityHandler::filterKernelParams(int kernel){
    switch(kernel){
        case 0: // Linear
            _degree->itemAt(1)->widget()->setEnabled(true);
            _gamma->itemAt(1)->widget()->setEnabled(false);
            _coef0->itemAt(1)->widget()->setEnabled(false);
        break;
        case 1: // poly
            _degree->itemAt(1)->widget()->setEnabled(false);
            _gamma->itemAt(1)->widget()->setEnabled(true);
            _coef0->itemAt(1)->widget()->setEnabled(true);
        break;
        case 2: // RBF
            _degree->itemAt(1)->widget()->setEnabled(false);
            _gamma->itemAt(1)->widget()->setEnabled(true);
            _coef0->itemAt(1)->widget()->setEnabled(false);
        break;
        case 3: // Sigmoid
            _degree->itemAt(1)->widget()->setEnabled(false);
            _gamma->itemAt(1)->widget()->setEnabled(true);
            _coef0->itemAt(1)->widget()->setEnabled(true);
        break;
        case 4: // Precomputed
            _degree->itemAt(1)->widget()->setEnabled(false);
            _gamma->itemAt(1)->widget()->setEnabled(false);
            _coef0->itemAt(1)->widget()->setEnabled(false);
        break;
    }
}
