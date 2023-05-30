#include <QCoreApplication>

#include "oasisclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    OasisClient *oasisClient = new OasisClient(&a);
    oasisClient->run();
    return a.exec();
}
