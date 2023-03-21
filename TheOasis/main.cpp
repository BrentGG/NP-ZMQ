#include <QCoreApplication>

#include "oasis.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Oasis *oasis = new Oasis(&a);
    oasis->run();

    return a.exec();
}
