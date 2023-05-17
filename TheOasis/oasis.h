#ifndef THEOASIS_H
#define THEOASIS_H

#include "dbmanager.h"
#include "sender.h"
#include "blackjack.h"

#include <QCoreApplication>
#include <nzmqt/nzmqt.hpp>
#include <nzmqt/impl.hpp>
#include <QObject>
#include <QList>
#include <QMap>

class Oasis : public QObject
{
public:
    Oasis(QCoreApplication *coreApp);

    void run();

    void sendMessage(QString message);

    void sendInfo(QList<QString> request);
    void sendHelp();
    bool registerPlayer(QList<QString> request);
    bool loginPlayer(QList<QString> request);
    bool logoutPlayer(QList<QString> request);
    bool getBalance(QList<QString> request);
    bool playSlotMachine(QList<QString> request);
    bool playRoulette(QList<QString> request);
    bool playBlackjack(QList<QString> request);
    QList<int> strToIntList(QString str);

private slots:
    void handleMessage(const QList<QByteArray>& messages);
    void sendAd();
    void checkStatus();

private:
    DbManager *dbManager;

    nzmqt::ZMQContext *context;
    nzmqt::ZMQSocket *pusher;
    nzmqt::ZMQSocket *subscriber;

    Sender *sender;
    QMap<QString, Player*> activePlayers;

    QList<Blackjack*> blackjackInstances;
};

#endif // THEOASIS_H
