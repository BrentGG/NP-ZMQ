#ifndef OASISSENDER_H
#define OASISSENDER_H

#include <QString>
#include <nzmqt/nzmqt.hpp>
#include <nzmqt/impl.hpp>
#include <QObject>


class OasisSender: public QObject
{
public:
    OasisSender(nzmqt::ZMQSocket *pusher);

    void sendMessage(QString message, bool log = true);
    void sendHelp();
    void sendInfo(QList<QString> request);
    void sendLog(QString message, bool received);

private:
    nzmqt::ZMQSocket *pusher;
};

#endif // OASISSENDER_H
