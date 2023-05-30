#ifndef OASISCLIENT_H
#define OASISCLIENT_H

#include <QCoreApplication>
#include <nzmqt/nzmqt.hpp>
#include <nzmqt/impl.hpp>

/**
 * @brief A CLI client for The Oasis.
 */
class OasisClient : public QObject
{
public:
    OasisClient(QCoreApplication *coreApp);

    void run();
    void connect();

private slots:
    void handleMessage(const QList<QByteArray>& messages);

private:
    nzmqt::ZMQContext *context;
    nzmqt::ZMQSocket *pusher;
    nzmqt::ZMQSocket *subscriber;

    QString username;
    QString password;
    bool loggedIn;
    bool waiting;
    bool helpRequested;
    bool infoRequested;
    QString infoTopic;
    int balance;
    bool inProgress;

    int getMenuInput(int min, int max);
    int getBet();
    void sendMessage(QString message);
    void help();
    void completeHelp(QString response);
    void info();
    void completeInfo(QString response);
    void register_();
    void completeRegister(QList<QString> response);
    void login();
    void completeLogin(QList<QString> response);
    void logout();
    void completeLogout(QList<QString> response);
    void getBalance();
    void completeBalance(QList<QString> response);
    void slotmachine();
    void completeSlotmachine(QList<QString> response);
    void roulette();
    void completeRoulette(QList<QString> response);
    QList<QString> rouletteBets;
    QString getRouletteNumbers(int amount);
    void blackjack();
    void completeBlackjack(QList<QString> response);
    QList<QString> blackjackActions;
    void choHan();
    void completeChoHan(QList<QString> response);
    void exitClient();
};

#endif // OASISCLIENT_H
