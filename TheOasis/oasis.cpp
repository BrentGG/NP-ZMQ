#include "oasis.h"
#include "failedrequest.h"
#include "slotmachine.h"

#include <iostream>
#include <QDir>
#include <QTimer>
#include <QCryptographicHash>
#include <QDateTime>

Oasis::Oasis(QCoreApplication *coreApp)
{
    try {
        dbManager = new DbManager(QDir("../TheOasis/oasis_db.db").absolutePath());
        if (dbManager->isConnected())
            std::cout << "Connected to database." << std::endl;
        else {
            std::cout << "Could not connect to database." << std::endl;
            exit(-1);
        }

        context = nzmqt::createDefaultContext(coreApp);
        pusher = context->createSocket(nzmqt::ZMQSocket::TYP_PUSH, context);
        subscriber = context->createSocket(nzmqt::ZMQSocket::TYP_SUB, context);

        QObject::connect(subscriber, &nzmqt::ZMQSocket::messageReceived, this, &Oasis::handleMessage);

        // Send ad every X minutes
        QTimer *adTimer = new QTimer(this);
        QObject::connect(adTimer, &QTimer::timeout, this, &Oasis::sendAd);
        //adTimer->start(60000 * 3);

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
        subscriber->subscribeTo("theoasis>logout?>");
        subscriber->subscribeTo("theoasis>balance?>");
        subscriber->subscribeTo("theoasis>slotmachine?>");

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
        std::cout << "Received: " << message.toStdString() << std::endl;
        QString msg = QString::fromStdString(message.toStdString());
        QList<QString> request = msg.split(">");
        if (request.size() >= 2) {
            try {
                // Request that can be made without logging in
                if (request[1].compare("info?") == 0)
                    sendInfo(request);
                else if (request[1].compare("help?") == 0)
                    sendHelp();
                else if (request[1].compare("register?") == 0)
                    registerPlayer(request);
                else if (request[1].compare("login?") == 0)
                    loginPlayer(request);
                else {
                    // Request that can only be made when logged in
                    if (request.size() >= 3) {
                        if (!activePlayers.contains(request[2]))
                            sendMessage(QString("theoasis>" + request[1].remove('?') + "!>" + request[2] + ">false>Not logged in.>"));
                        else {
                            if (request[1].compare("logout?") == 0)
                                logoutPlayer(request);
                            else if (request[1].compare("balance?") == 0)
                                getBalance(request);
                            else if (request[1].compare("slotmachine?") == 0)
                                playSlotMachine(request);
                        }
                    }
                }
            } catch(FailedRequest e) {
                sendMessage(e.what());
            }
        }
    }
}

/**
 * @brief Send out an ad for The Oasis
 */
void Oasis::sendAd()
{
    sendMessage(QString("Join The Oasis Now! Request 'theoasis>info?>' and subscribe to 'theoasis>info!>'."));
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
 * @brief Sends an informational message about The Oasis or one of its activities.
 * @param request: the request to register, split into parts seperated by '>'
 * REQ: theoasis>info?>\n\
 * RES: {theoasis>info!>}[info:string]>\n\
 * or
 * REQ: theoasis>info?>[about:string]\n\
 * RES: {theoasis>info!>[about:string]>}[info:string]>\n\
 */
void Oasis::sendInfo(QList<QString> request)
{
    if (request.size() < 3 || request[2].compare("") == 0) {
        QString response = QString("theoasis>info!>\n\
>>> The Oasis: Info <<<\n\n\
Welcome to The Oasis, where your wildest fantasies become reality. Step into our virtual casino and leave behind the drudgery of \
the real world. Our advanced technology allows you to experience the thrill of high-stakes gambling without ever leaving your home.\
Don't let the mundane nature of your everyday life hold you back. Join us now and indulge in every desire you've ever had. \
The Oasis – where reality is just a distant memory.\n\n\
To join The Oasis, send the following request 'theoasis>register?>[username:string]>[password:string]>' and subscribe to 'theoasis>register!>[username:string]>'. \
Make sure to hash your password for your own security.\n\n\
To get an overview of all the possible request, send the following request 'theoasis>help?>' and subscribe to 'theoasis>help!>'.\n\
");
        sendMessage(response);
    }
    else if (request[2].compare("slotmachine") == 0) {
        sendMessage(SlotMachine::getInfo());
    }
}

/**
 * @brief Sends a help message for those unfamiliar with The Oasis.
 */
void Oasis::sendHelp()
{
    QString response = QString("theoasis>help!>\n\
>>> The Oasis: Help <<<\n\n\
Welcome To The Oasis!\n\n\
The Oasis is a virtual casino. Here are the requests you can make to The Oasis, as well as the responses you can expect.\n\
The response might contain some variables, these will be indicated by square brackets '[]'.\n\
Additionally, curly brackets '{}' are used to signify which part of the response is the topic you should subscribe to.\n\
Variables that are lists have the elements separated by a comma (,)\n\n\
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
- Get more info on the slot machines.\n\
  REQ: theoasis>info?>slotmachine>\n\
  RES: {theoasis>info!>slotmachine>}\n\
\nThese requests are only available after registering and logging in: \n\
- Logout of The Oasis. Must be logged in.\n\
  REQ: theoasis>logout?>[username:string]>[password:string]>\n\
  RES: {theoasis>logout!>[username:string]>}[success:bool]>[message:string]>\n\
- Get your credit balance.\n\
  REQ: theoasis>balance?>[username:string]>\n\
  RES: {theoasis>balance!>[username:string]>}[success:bool]>[balance:int]>[message:string]\n\
- Play a three-reel, single-payline slot machine. The bet is the amount of money you put into the slot machine, which must be \
lower than player's balance and larger than 0. The payline is the three symbols you got. The payout is the amount of money you \
get back (if this is 0, you lost your bet).\n\
  REQ: theoasis>slotmachine?>[username:string]>[bet:integer]>\n\
  RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[payline:string]>[payout:integer]>[message:string]>\n\
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
    if (request.size() >= 4) {
        QString hashedPassword(QCryptographicHash::hash(QByteArrayView(request[3].toUtf8().constData()), QCryptographicHash::Md5).toHex().constData());
        bool success = dbManager->addPlayer(new Player(request[2]), hashedPassword);
        if (success) {
            std::cout << "Registered new player: " << request[2].toStdString() << std::endl;
            sendMessage(QString("theoasis>register!>" + request[2] + ">true>Successfully registered! Welcome to The Oasis!>"));
            return true;
        }
        else
            throw FailedRequest(QString("theoasis>register!>" + request[2] + ">false>Registration failed, try a different username.>"));
    }
    throw FailedRequest(QString("theoasis>register!>" + request[2] + ">false>Bad request.>"));
    return false;
}

/**
 * @brief Login to The Oasis
 * @param request: the request to login, split into parts separated by '>'
 * @return true if succesfully logged in, false if not
 * REQ: theoasis>login?>[username:string]>[password:string]>
 * RES: {theoasis>login!>[username:string]>}[success:bool]>[message:string]>
 */
bool Oasis::loginPlayer(QList<QString> request)
{
    Player *player = nullptr;
    if (request.size() >= 4) {
        if (activePlayers.contains(request[2])) {
            sendMessage(QString("theoasis>login!>" + request[2] + ">true>Already logged in.>"));
            return true;
        }
        QString hashedPassword(QCryptographicHash::hash(QByteArrayView(request[3].toUtf8().constData()), QCryptographicHash::Md5).toHex().constData());
        player = dbManager->getPlayerByNameAndPassword(request[2], hashedPassword);
        if (player != nullptr) {
            std::cout << "Logged in player: " << request[2].toStdString() << std::endl;
            activePlayers[player->getName()] = player;
            player->setOnlineSince(QDateTime::currentSecsSinceEpoch());
            sendMessage(QString("theoasis>login!>" + request[2] + ">true>Successfully logged in! Welcome back to The Oasis!>"));
            return true;
        }
        else
            throw FailedRequest(QString("theoasis>login!>" + request[2] + ">false>Login failed, wrong username and/or password.>"));
    }
    throw FailedRequest(QString("theoasis>login!>" + request[2] + ">false>Bad request.>"));
    return false;
}

/**
 * @brief Logout of The Oasis
 * @param request: the request to logout, split into parts separated by '>'
 * @return true if succesfully logged out, false if not
 * REQ: theoasis>logout?>[username:string]>[password:string]>
 * RES: {theoasis>logout!>[username:string]>}[success:bool]>[message:string]>
 */
bool Oasis::logoutPlayer(QList<QString> request)
{
    if (request.size() >= 4 && activePlayers.contains(request[2])) {
        QString hashedPassword(QCryptographicHash::hash(QByteArrayView(request[3].toUtf8().constData()), QCryptographicHash::Md5).toHex().constData());
        if (dbManager->checkPassword(request[2], hashedPassword)) {
            Player *player = activePlayers[request[2]];
            std::cout << "Logged out player: " << request[2].toStdString() << std::endl;
            std::cout << player->getCredits() << std::endl;
            dbManager->setCredits(player->getName(), player->getCredits());
            activePlayers.remove(player->getName());
            sendMessage(QString("theoasis>logout!>" + request[2] + ">true>Successfully logged out, come back soon!"));
            return true;
        }
        else
            throw FailedRequest(QString("theoasis>logout!>" + request[2] + ">false>Wrong password.>"));
    }
    throw FailedRequest(QString("theoasis>logout!>" + request[2] + ">false>Bad request.>"));
    return false;
}

/**
 * @brief Get a player's credit balance.
 * @param request: the request to get balance, split into parts separated by '>'
 * @return true if success, false if not
 * REQ: theoasis>balance?>[username:string]>
 * RES: {theoasis>balance!>[username:string]>}[success:bool]>[balance:int]>[message:string]
 */
bool Oasis::getBalance(QList<QString> request)
{
    if (request.size() >= 3 && activePlayers.contains(request[2])) {
        sendMessage(QString("theoasis>balance!>" + request[2] + ">true>" + QString::number(activePlayers[request[2]]->getCredits()) + ">"));
        return true;
    }
    throw FailedRequest(QString("theoasis>getBalance!>" + request[2] + ">false>Bad request.>"));
    return false;
}

/**
 * @brief Play a three-reel, single-payline slot machine. The bet is the amount of money you put into the slot machine, which must be lower than player's balance. The payline is the
 * three symbols you got. The payout is the amount of money you get back (if this is 0, you lost your bet).
 * @param request: the request to play the slot machine, split into parts separated by '>'
 * @param return: true if successful, false if not
 * REQ: theoasis>slotmachine?>[username:string]>[bet:integer]>
 * RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[payline:string]>[payout:integer]>[message:string]>
 */
bool Oasis::playSlotMachine(QList<QString> request)
{
    if (request.size() >= 4 && activePlayers.contains(request[2])) {
        Player* player = activePlayers[request[2]];
        bool isNumber = false;
        int bet = request[3].toInt(&isNumber);
        if (isNumber && bet > 0 && bet <= player->getCredits()) {
            player->modifyCredits(-1 * bet);
            QList<SlotMachine::Symbols> payline = SlotMachine::spin();
            int payout = SlotMachine::calcPayout(payline, bet);
            QString response = QString("theoasis>slotmachine!>");
            response.append(request[2] + ">true>");
            QList<QString> paylineStr = SlotMachine::paylineToStringList(payline);
            for (int i = 0; i < paylineStr.size(); ++i)
                response.append(paylineStr[i] + (i < paylineStr.size() - 1 ? "," : ">"));
            response.append(QString::number(payout) + ">" + (payout != 0 ? "Congratulations, you won!>" : "You lost!>"));
            sendMessage(response);
            player->modifyCredits(payout);
            return true;
        }
        else
            throw FailedRequest(QString("theoasis>slotmachine!>" + request[2] + ">false>Invalid bet.>"));
    }
    throw FailedRequest(QString("theoasis>slotmachine!>" + request[2] + ">false>Bad request.>"));
    return false;
}

/**
 * @brief Checks if any of the active players have gone inactive and removes those from the active list.
 */
void Oasis::checkStatus()
{
    long long currentTime = QDateTime::currentSecsSinceEpoch();
    QMap<QString, Player*>::iterator i = activePlayers.begin();
    while (i != activePlayers.end()) {
        if (currentTime - i.value()->getOnlineSince() > 15 * 60) {
            std::cout << "Timed out player: " << i.value()->getName().toStdString() << std::endl;
            dbManager->setCredits(i.value()->getName(), i.value()->getCredits());
            i = activePlayers.erase(i);
        }
        else
            ++i;
    }
}
