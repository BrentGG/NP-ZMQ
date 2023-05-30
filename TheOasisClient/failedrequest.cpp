#include "failedrequest.h"

FailedRequest::FailedRequest(QString reason): reason(reason)
{
}

QString FailedRequest::what()
{
    return reason;
}
