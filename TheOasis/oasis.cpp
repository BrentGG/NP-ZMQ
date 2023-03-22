#include "oasis.h"

#include <iostream>
#include <QDir>
#include <QTimer>
#include <QCryptographicHash>
#include <QDateTime>

Oasis::Oasis(QCoreApplication *coreApp)
{
    try {
        dbManager = new DbManager(QDir("../TheOasis/oasis_db.db").absolutePath());

        context = nzmqt::createDefaultContext(coreApp);
        pusher = context->createSocket(nzmqt::ZMQSocket::TYP_PUSH, context);
        subscriber = context->createSocket(nzmqt::ZMQSocket::TYP_SUB, context);

        QObject::connect(subscriber, &nzmqt::ZMQSocket::messageReceived, this, &Oasis::handleMessage);

        // Send ad every X minutes
        QTimer *adTimer = new QTimer(this);
        QObject::connect(adTimer, &QTimer::timeout, this, &Oasis::sendAd);
        adTimer->start(60000 * 3);

        // Check online status
        QTimer *statusTimer = new QTimer(this);
        QObject::connect(statusTimer, &QTimer::timeout, this, &Oasis::checkStatus);
        statusTimer->start(15 * 60 * 1000);
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

        subscriber->subscribeTo("theoasis>info?>");
        subscriber->subscribeTo("theoasis>help?>");
        subscriber->subscribeTo("theoasis>register?>");
        subscriber->subscribeTo("theoasis>login?>");

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
            if (parts[1].compare("info?") == 0)
                sendInfo();
            else if (parts[1].compare("help?") == 0)
                sendHelp();
            else if (parts[1].compare("register?") == 0)
                registerPlayer(parts);
            else if (parts[1].compare("login?") == 0)
                loginPlayer(parts);
        }
    }
}

/**
 * @brief Send out an ad for The Oasis
 */
void Oasis::sendAd()
{
    QString response = QString("Join The Oasis Now! Request 'theoasis>info?>' and subscribe to 'theoasis>info!>'.");
    sendMessage(response);
}

/**
 * @brief Sends a message using the pusher.
 * @param message: the message to be sent
 */
void Oasis::sendMessage(QString message)
{
    nzmqt::ZMQMessage responseZmq = nzmqt::ZMQMessage(message.toUtf8());
    pusher->sendMessage(responseZmq);
    std::cout << "Sent: " << message.toStdString() << std::endl;
}

/**
 * @brief Sends an informational message about The Oasis.
 */
void Oasis::sendInfo()
{
    QString response = QString("\n\
>>> The Oasis: Info <<<\n\n\
Welcome to The Oasis, the ultimate virtual world experience where you can escape the drudgery \
of reality and live out your wildest fantasies. With our cutting-edge technology, you'll even forget you're in a simulation.\n\
Need a break from your mundane life? The Oasis is your ticket to an endless party where the fun never stops. No need to worry \
about responsibilities or consequences, just immerse yourself in our virtual wonderland and forget about the real world.\n\
Tired of being stuck in the same place all day? In The Oasis, you can explore entire galaxies and visit exotic planets, all \
from the comfort of your own couch. Whether you're looking for adventure or relaxation, The Oasis has it all.\n\
With our state-of-the-art customization options, you can be whoever you want to be in The Oasis. Shed your boring old self \
and become a powerful superhero, a world-renowned assassin, or anything in between. The only limit is your imagination.\n\
Join us in The Oasis today and start living your best life. Don't let the monotony of reality hold you back any longer. \
The Oasis is waiting for you. \n\n\
To get an overview of all the possible request, send the following request 'theoasis>help?>' and subscribe to 'theoasis>help!>'.\n\
To register to The Oasis, send the following request 'theoasis>register?>[username:string]>[password:string]>' and subscribe to 'theoasis>register!>[username:string]>'. \
Make sure to hash your password for your own security.\n\n\
");
    sendMessage(response);
}

/**
 * @brief Sends a help message for those unfamiliar with The Oasis.
 */
void Oasis::sendHelp()
{
    QString response = QString("\n\
>>> The Oasis: Help <<<\n\n\
Welcome To The Oasis!\n\n\
For some general information about what The Oasis is, send the following request 'theoasis>info?>' and subscribe to 'theoasis>info!>'\n\n\
Here are the requests you can make to The Oasis, as well as the responses you can expect.\n\
The response might contain some variables, these will be indicated by square brackets '[]'.\n\
Additionally, curly brackets '{}' are used to signify which part of the response is the topic you should subscribe to.\n\n\
All passwords that are sent to the service should be hashed for your own security(see https://doc.qt.io/qt-5/qcryptographichash.html). \
However, passwords will also be hashed again in the service.\n\n\
These requests are available to anyone:\n\
- Receive an informational message from The Oasis.\n\
  REQ: theoasis>info?>\n\
  RES: {theoasis>info!>}[info:string]>\n\
- Receive all the possible requests and responses (a.k.a. the message you're reading right now).\n\
  REQ: theoasis>help?>\n\
  RES: {theoasis>help!>}[help:string]>\n\
- Register to The Oasis. A unique username is required, if this is not the case the 'success' variable will be false.\n\
  REQ: theoasis>register?>[username:string]>[password:string]>\n\
  RES: {theoasis>register!>[username:string]>}[success:bool]>[message:string]>\n\
- Login to The Oasis. Must be registered first.\n\
  REQ: theoasis>login?>[username:string]>[password:string]>\n\
  RES: {theoasis>login!>[username:string]>}[success:bool]>[message:string]>\n\
\nThese requests are only available after registering and logging in: \n\
W.I.P.\n\
");
    sendMessage(response);
}

/**
 * @brief Registers a player in The Oasis.
 * @param request: the request to register, split into parts seperated by '>'
 * @returns true if registering was successful, false if not
 * REQ: theoasis>register?>[username:string]>[password:string]>
 * RES: {theoasis>register!>[username:string]>}[success:bool]>[message:string]>
 * Passwords that are sent to the server should already be hashed but I hash them again just in case they werent hashed.
 */
bool Oasis::registerPlayer(QList<QString> request) {
    bool success = false;
    if (request.size() >= 4) {
        QString hashedPassword(QCryptographicHash::hash(QByteArrayView(request[3].toUtf8().constData()), QCryptographicHash::Md5).toHex().constData());
        success = dbManager->addPlayer(new Player(request[2]), hashedPassword);
        if (success) {
            std::cout << "Registered new player: " << request[2].toStdString() << std::endl;
        }
    }
    QString response = QString("theoasis>register!>");
    response.append(request[2] + ">" + (success ? "true" : "false") + ">" + (success ? "Successfully registered! Welcome to The Oasis!>" : "Registration failed, try a different username.>"));
    sendMessage(response);
    return success;
}

/**
 * @brief Login to The Oasis
 * @param request: the request to loging, split into parts separated by '>'
 * @return true if succesfully logged in, false if not
 * REQ: theoasis>login?>[username:string]>[password:string]>
 * RES: {theoasis>login!>[username:string]>}[success:bool]>[message:string]>
 */
bool Oasis::loginPlayer(QList<QString> request)
{
    Player *player = nullptr;
    if (request.size() >= 4) {
        QString hashedPassword(QCryptographicHash::hash(QByteArrayView(request[3].toUtf8().constData()), QCryptographicHash::Md5).toHex().constData());
        player = dbManager->getPlayerByNameAndPassword(request[2], hashedPassword);
        if (player != nullptr) {
            std::cout << "Logged in player: " << request[2].toStdString() << std::endl;
            activePlayers.append(player);
            player->setOnlineSince(QDateTime::currentSecsSinceEpoch());
        }
    }
    QString response = QString("theoasis>login!>");
    response.append(request[2] + ">" + (player != nullptr ? "true" : "false") + ">" + (player != nullptr ? "Successfully logged in! Welcome back to The Oasis!>" : "Login failed, wrong username and/or password.>"));
    sendMessage(response);
    return player != nullptr;
}

/**
 * @brief Checks if any of the active players have gone inactive and removes those from the active list.
 */
void Oasis::checkStatus()
{
    int i = 0;
    long long currentTime = QDateTime::currentSecsSinceEpoch();
    while (i < activePlayers.size()) {
        if (currentTime - activePlayers[i]->getOnlineSince() > 15 * 60)
            activePlayers.remove(i);
        else
            ++i;
    }
}
