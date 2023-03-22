#ifndef THEOASIS_H
#define THEOASIS_H

#include "dbmanager.h"

#include <QCoreApplication>
#include <nzmqt/nzmqt.hpp>
#include <nzmqt/impl.hpp>
#include <QObject>
#include <QList>


class Oasis : public QObject
{
public:
    Oasis(QCoreApplication *coreApp);

    void run();

    void handleMessage(const QList<QByteArray>& messages);
    void sendMessage(QString message);

    void sendInfo();
    void sendHelp();
    bool registerPlayer(QList<QString> input);

private:
    DbManager *dbManager;

    nzmqt::ZMQContext *context;
    nzmqt::ZMQSocket *pusher;
    nzmqt::ZMQSocket *subscriber;
};

#endif // THEOASIS_H
