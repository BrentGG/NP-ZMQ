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

    void sendMessage(QString message);

    void sendInfo();
    void sendHelp();
    bool registerPlayer(QList<QString> input);

private slots:
    void handleMessage(const QList<QByteArray>& messages);
    void sendAd();

private:
    DbManager *dbManager;

    nzmqt::ZMQContext *context;
    nzmqt::ZMQSocket *pusher;
    nzmqt::ZMQSocket *subscriber;
};

#endif // THEOASIS_H
