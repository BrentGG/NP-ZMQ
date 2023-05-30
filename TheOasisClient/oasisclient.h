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

    void sendMessage(QString message);
    void help();
    void completeHelp(QString response);
    void login();
    void completeLogin(QList<QString> response);
};

#endif // OASISCLIENT_H
