#include "loadingindicator.h"

QLabel* LoadingIndicator::_indicator;
bool LoadingIndicator::_is_running;

void LoadingIndicator::setIndicatorLabel(QLabel *label)
{
    _indicator = label;
    _indicator->setText("");
    _is_running = false;
}

void LoadingIndicator::startLoadingAnimation()
{
    if(_indicator && !_is_running){
        _indicator->show();
        _indicator->setText("Performing...");
        _is_running = true;
    }
}

void LoadingIndicator::stopLoadingAnimation()
{
    if(_indicator && _is_running){
        _indicator->setText("");
        _is_running = false;
    }
}
