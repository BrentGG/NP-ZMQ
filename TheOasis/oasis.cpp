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

        subscriber->subscribeTo("theoasis>help?>");
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
            if (parts[1].compare("help?") == 0)
                sendHelp();
            else if (parts[1].compare("register?") == 0)
                registerPlayer(parts);
        }
    }
}

/**
 * @brief Sends a message using the pusher
 * @param message: the message to be sent
 */
void Oasis::sendMessage(QString message)
{
    nzmqt::ZMQMessage responseZmq = nzmqt::ZMQMessage(message.toUtf8());
    pusher->sendMessage(responseZmq);
    std::cout << "Sent: " << message.toStdString() << std::endl;
}

/**
 * @brief Sends a help message for those unfamiliar with The Oasis
 */
void Oasis::sendHelp()
{
    QString response = QString("\n \
        Welcome To The Oasis!\n\n \
        Here are the requests you can make to The Oasis, as well as the responses you can expect.\n \
        The response might contain some variables, these will be indicated by square brackets '[]'.\n \
        Additionally, curly brackets '{}' are used to signify which part of the response is the topic you should subscribe to.\n\n \
        These requests are available to anyone:\n \
        - Receive an informational message from The Oasis.\n \
          REQ: theoasis>info?>\n \
          RES: {theoasis>info!>}[info:string]>\n \
        - Receive all the possible requests and responses (a.k.a. the message you're reading right now).\n \
          REQ: theoasis>help?>\n \
          RES: {theoasis>help!>}[help:string]>\n \
        - Register to The Oasis. A unique username is required, if this is not the case the 'success' variable will be false.\n \
          REQ: theoasis>register?>[username:string]>[password:string]>\n \
          RES: {theoasis>register!>[username:string]>}[success:bool]>\n \
        These requests are only available after registering: \n \
        W.I.P.\n \
        ");
    sendMessage(response);
}

/**
 * @brief Registers a player in The Oasis.
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
    sendMessage(response);
    return success;
}
