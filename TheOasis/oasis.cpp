#include "oasis.h"

#include <iostream>
#include <QDir>

Oasis::Oasis(QCoreApplication *coreApp)
{
    try {
        dbManager = new DbManager(QDir("../TheOasis/oasis_db.db").absolutePath());

        context = nzmqt::createDefaultContext(coreApp);
        pusher = context->createSocket(nzmqt::ZMQSocket::TYP_PUSH, context);
        subscriber = context->createSocket(nzmqt::ZMQSocket::TYP_SUB, context);
        QObject::connect(subscriber, &nzmqt::ZMQSocket::messageReceived, this, &Oasis::handleMessage);
    }
    catch(nzmqt::ZMQException & ex) {
        std::cerr << "Caught an exception : " << ex.what();
    }
}

/**
 * @brief Runs the Oasis by connecting the pusher and subscriber, sub
 */
void Oasis::run()
{
    try {
        pusher->connectTo("tcp://benternet.pxl-ea-ict.be:24041");
        subscriber->connectTo("tcp://benternet.pxl-ea-ict.be:24042");
        if(!pusher->isConnected() || !subscriber->isConnected()) {
            std::cerr << "Pusher and/or subscriber not connected." << std::endl;
        }

        subscriber->subscribeTo("theoasis>register?>");

        context->start();
    }
    catch(nzmqt::ZMQException & ex) {
        std::cerr << "Caught an exception : " << ex.what();
    }
}

/**
 * @brief Handles incoming messages and calls the appropriate method to deal with each message.
 * @param messages: messages to process
 */
void Oasis::handleMessage(const QList<QByteArray> &messages)
{
    for(const QByteArray &message : messages) {
        std::cout << "Recieved: " << message.toStdString() << std::endl;
        QString msg = QString::fromStdString(message.toStdString());
        QList<QString> parts = msg.split(">");
        if (parts.size() > 1) {
            if (parts[1].compare("register?") == 0) {
                registerPlayer(parts);
            }
        }
    }
}

/**
 * @brief Registers a player in the Oasis.
 * @param request: the request to register, split into parts seperated by '>'
 * @returns true if registering was successful, false if not
 * REQ: theoasis>register?>[username:string]>[password:string]>
 * RES: theoasis>register!>[username:string]>[success:bool]> (client should subscribe to topic including username they sent)
 * TODO: Make password secure
 */
bool Oasis::registerPlayer(QList<QString> request) {
    bool success = false;
    if (request.size() >= 4) {
        success = dbManager->addPlayer(new Player(request[2]), request[3]);
        if (success) {
            std::cout << "Registered new player: " << request[2].toStdString() << std::endl;
        }
    }
    QString response = QString("theoasis>register!>");
    response.append(request[2] + ">" + (success ? "true" : "false") + ">");
    std::cout << "Sent: " << response.toStdString() << std::endl;
    nzmqt::ZMQMessage responseZmq = nzmqt::ZMQMessage(response.toUtf8());
    pusher->sendMessage(responseZmq);
    return success;
}
