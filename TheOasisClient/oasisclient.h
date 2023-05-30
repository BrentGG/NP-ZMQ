#ifndef OASISCLIENT_H
#define OASISCLIENT_H

#include <QCoreApplication>
#include <nzmqt/nzmqt.hpp>
#include <nzmqt/impl.hpp>

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

    int getMenuInput(int min, int max);
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
    void slotMachine();
    void completeSlotmachine(QList<QString> response);
};

#endif // OASISCLIENT_H
