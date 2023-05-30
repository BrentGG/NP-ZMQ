#ifndef FAILEDREQUEST_H
#define FAILEDREQUEST_H


#include <iostream>
#include <QString>

class FailedRequest : std::exception
{
public:
    FailedRequest(QString reason);

    QString what();

private:
    QString reason;
};

#endif // FAILEDREQUEST_H
