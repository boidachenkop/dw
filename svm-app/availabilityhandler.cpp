#include "availabilityhandler.h"

AvailabilityHandler::AvailabilityHandler(QHBoxLayout* params, QTabWidget* tabs,
                                         QPushButton* train_button, QPushButton* test_button):
    _train_button(train_button), _test_button(test_button)
{
    degree = params->findChild<QHBoxLayout*>("degree_horizontalLayout");
    gamma = params->findChild<QHBoxLayout*>("gamma_horizontalLayout");
    coef0 = params->findChild<QHBoxLayout*>("coef0_horizontalLayout");
    C = params->findChild<QHBoxLayout*>("C_horizontalLayout");
    nu = params->findChild<QHBoxLayout*>("nu_horizontalLayout");
    p = params->findChild<QHBoxLayout*>("P_horizontalLayout");

    for(int i=0; i<tabs->count(); i++){
       if(tabs->tabText(i) == "CV"){
            cv_tab = tabs->widget(i);
       }else if(tabs->tabText(i) == "Scaling"){
            scaling_tab = tabs->widget(i);
       }else if(tabs->tabText(i) == "Visualization"){
            visualization_tab = tabs->widget(i);
       }else if(tabs->tabText(i) == "Feature Selection"){
            fs_tab = tabs->widget(i);
       }else{
           std::cerr<<"Not registered tab detected!"<<std::endl;
       }
    }
}

void AvailabilityHandler::filterTabs(unsigned int tab){
    for(int i=0; i<4; i++){
        if((tab>>i & 1) == 1){
            if(i==0){
                cv_tab->setEnabled(true);
            }else if(i==1){
                scaling_tab->setEnabled(true);
            }else if(i==2){
                visualization_tab->setEnabled(true);
            }else if(i==3){
                fs_tab->setEnabled(true);
            }
        }else{
            if(i==0){
                cv_tab->setEnabled(false);
            }else if(i==1){
                scaling_tab->setEnabled(false);
            }else if(i==2){
                visualization_tab->setEnabled(false);
            }else if(i==3){
                fs_tab->setEnabled(false);
            }
        }
    }
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
