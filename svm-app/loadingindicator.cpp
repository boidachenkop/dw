#include "loadingindicator.h"

QLabel* LoadingIndicator::_indicator;
QMovie* LoadingIndicator::_movie;
bool LoadingIndicator::_is_running;

void LoadingIndicator::setIndicatorLabel(QLabel *label)
{
    _indicator = label;
    _movie = new QMovie(":/loader.gif");
    _indicator->setMovie(_movie);
    _is_running = false;
}

void LoadingIndicator::startLoadingAnimation()
{
    if(_indicator && !_is_running){
        _indicator->show();
        _movie->start();
        _is_running = true;
    }
}

void LoadingIndicator::stopLoadingAnimation()
{
    if(_indicator && _is_running){
        _movie->stop();
    }
}
