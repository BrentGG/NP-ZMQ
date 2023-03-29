#ifndef SENDER_H
#define SENDER_H

#include <QString>
#include <nzmqt/nzmqt.hpp>
#include <nzmqt/impl.hpp>
#include <QObject>


class Sender: public QObject
{
public:
    Sender(nzmqt::ZMQSocket *pusher);

    void sendMessage(QString message);
    void sendHelp();
    void sendAd();
    void sendInfo(QList<QString> request);

private:
    nzmqt::ZMQSocket *pusher;
};

#endif // SENDER_H
