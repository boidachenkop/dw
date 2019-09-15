#ifndef LOADINGINDICATOR_H
#define LOADINGINDICATOR_H

#include <QLabel>

class LoadingIndicator
{
    static QLabel* _indicator;
    static bool _is_running;
public:
    static void setIndicatorLabel(QLabel* label);
    static void startLoadingAnimation();
    static void stopLoadingAnimation();
};

#endif // LOADINGINDICATOR_H
