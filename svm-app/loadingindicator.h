#ifndef LOADINGINDICATOR_H
#define LOADINGINDICATOR_H

#include <QLabel>
#include <QMovie>

class LoadingIndicator
{
    static QLabel* _indicator;
    static bool _is_running;
    static QMovie* _movie;
public:
    static void setIndicatorLabel(QLabel* label);
    static void startLoadingAnimation();
    static void stopLoadingAnimation();
};

#endif // LOADINGINDICATOR_H
