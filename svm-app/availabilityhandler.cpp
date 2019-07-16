#include "availabilityhandler.h"

AvailabilityHandler::AvailabilityHandler(QList<QHBoxLayout*> kernelParams):
    _kernelParams(kernelParams)
{
    QHBoxLayout* hl = _kernelParams.takeAt(0);
    hl->setEnabled(true);
}
