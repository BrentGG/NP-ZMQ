#include "oasis.h"
#include "failedrequest.h"
#include "slotmachine.h"
#include "roulette.h"
#include "chohan.h"

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
        sender = new Sender(pusher);

        QObject::connect(subscriber, &nzmqt::ZMQSocket::messageReceived, this, &Oasis::handleMessage);

        // Send ad every X minutes
        QTimer *adTimer = new QTimer(this);
        QObject::connect(adTimer, &QTimer::timeout, sender, &Sender::sendAd);
        //adTimer->start(60000 * 3);

        // Check online status
        QTimer *statusTimer = new QTimer(this);
        QObject::connect(statusTimer, &QTimer::timeout, this, &Oasis::checkStatus);
        statusTimer->start(15 * 60 * 1000);
    }
    catch(nzmqt::ZMQException & ex) {
        std::cerr << "Caught an exception : " << ex.what();
        exit(-1);
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
            exit(-1);
        }

        subscriber->subscribeTo("theoasis>info?>");
        subscriber->subscribeTo("theoasis>help?>");
        subscriber->subscribeTo("theoasis>register?>");
        subscriber->subscribeTo("theoasis>login?>");
        subscriber->subscribeTo("theoasis>logout?>");
        subscriber->subscribeTo("theoasis>balance?>");
        subscriber->subscribeTo("theoasis>slotmachine?>");
        subscriber->subscribeTo("theoasis>roulette?>");
        subscriber->subscribeTo("theoasis>blackjack?>");
        subscriber->subscribeTo("theoasis>cho-han?>");

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
                    sender->sendInfo(request);
                else if (request[1].compare("help?") == 0)
                    sender->sendHelp();
                else if (request[1].compare("register?") == 0)
                    registerPlayer(request);
                else if (request[1].compare("login?") == 0)
                    loginPlayer(request);
                else {
                    // Request that can only be made when logged in
                    if (request.size() >= 3) {
                        if (!activePlayers.contains(request[2]))
                            throw FailedRequest(QString("theoasis>" + request[1].remove('?') + "!>" + request[2] + ">false>Not logged in.>"));
                        else {
                            activePlayers[request[2]]->setOnlineSince(QDateTime::currentSecsSinceEpoch());
                            if (request[1].compare("logout?") == 0)
                                logoutPlayer(request);
                            else if (request[1].compare("balance?") == 0)
                                getBalance(request);
                            else if (request[1].compare("slotmachine?") == 0)
                                playSlotMachine(request);
                            else if (request[1].compare("roulette?") == 0)
                                playRoulette(request);
                            else if (request[1].compare("blackjack?") == 0)
                                playBlackjack(request);
                            else if (request[1].compare("cho-han?") == 0)
                                playChoHan(request);
                        }
                    }
                }
            } catch(FailedRequest e) {
                sender->sendMessage(e.what());
            }
        }
    }
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
            sender->sendMessage(QString("theoasis>register!>" + request[2] + ">true>Successfully registered! Welcome to The Oasis!>"));
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
            sender->sendMessage(QString("theoasis>login!>" + request[2] + ">true>Already logged in.>"));
            return true;
        }
        QString hashedPassword(QCryptographicHash::hash(QByteArrayView(request[3].toUtf8().constData()), QCryptographicHash::Md5).toHex().constData());
        player = dbManager->getPlayerByNameAndPassword(request[2], hashedPassword);
        if (player != nullptr) {
            std::cout << "Logged in player: " << request[2].toStdString() << std::endl;
            activePlayers[player->getName()] = player;
            player->setOnlineSince(QDateTime::currentSecsSinceEpoch());
            sender->sendMessage(QString("theoasis>login!>" + request[2] + ">true>Successfully logged in! Welcome back to The Oasis!>"));
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
    if (request.size() >= 4) {
        QString hashedPassword(QCryptographicHash::hash(QByteArrayView(request[3].toUtf8().constData()), QCryptographicHash::Md5).toHex().constData());
        if (dbManager->checkPassword(request[2], hashedPassword)) {
            Player *player = activePlayers[request[2]];
            std::cout << "Logged out player: " << request[2].toStdString() << std::endl;
            std::cout << player->getCredits() << std::endl;
            dbManager->setCredits(player->getName(), player->getCredits());
            activePlayers.remove(player->getName());
            sender->sendMessage(QString("theoasis>logout!>" + request[2] + ">true>Successfully logged out, come back soon!"));
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
 * REQ: theoasis>balance?>[username:string]>
 * RES: {theoasis>balance!>[username:string]>}[success:bool]>[balance:int]>[message:string]
 */
bool Oasis::getBalance(QList<QString> request)
{
    if (request.size() >= 3) {
        sender->sendMessage(QString("theoasis>balance!>" + request[2] + ">true>" + QString::number(activePlayers[request[2]]->getCredits()) + ">"));
        return true;
    }
    throw FailedRequest(QString("theoasis>getBalance!>" + request[2] + ">false>Bad request.>"));
    return false;
}

/**
 * @brief Play a three-reel, single-payline slot machine. The bet is the amount of money you put into the slot machine, which must be lower than player's
 * balance and higher than 0. The payline is the three symbols you got. The payout is the amount of money you get back (if this is 0, you lost
 * your bet).
 * @param request: the request to play the slot machine, split into parts separated by '>'
 * REQ: theoasis>slotmachine?>[username:string]>[bet:integer]>
 * RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[payline:string]>[payout:integer]>[message:string]>
 */
void Oasis::playSlotMachine(QList<QString> request)
{
    QString response = SlotMachine::handleRequest(activePlayers[request[2]], request);
    sender->sendMessage(response);
}

/**
 * @brief Play American, double-zero roulette. Bet must be lower than player's balance and higher than 0. The betName is the name of the
 * bet you're placing (list can be found in 'theoasis>info?>roulette>'). Some bets require you to specify some numbers, these should
 * be provided in betNumbers.
 * @param request: the request to play roulette, split into parts separated by '>'
 * REQ: theoasis>slotmachine?>[username:string]>[bet:integer]>[betName:string]>[betNumbers:list<int>]>
 * RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[rouletteNumber:string]>[payout:integer]>[message:string]>
 */
void Oasis::playRoulette(QList<QString> request)
{
    QString response = Roulette::handleRequest(activePlayers[request[2]], request);
    sender->sendMessage(response);
}

/**
 * @brief Play a blackjack game
 * @param request: the request to play Blackjack, split into parts separated by '>'
 */
void Oasis::playBlackjack(QList<QString> request)
{
    if (request.size() >= 4) {
        Player* player = activePlayers.find(request[2]).value();
        for (Blackjack* blackjack : blackjackInstances) {
            if (blackjack->getPlayer() == player) {
                QString response = blackjack->handleRequest(request);
                sender->sendMessage(response);
            }
        }
        blackjackInstances.append(new Blackjack(player, 2));
        QString response = blackjackInstances.last()->handleRequest(request);
        sender->sendMessage(response);
    }
    else
        throw FailedRequest(QString("theoasis>blackjack!>" + request[2] + ">false>Bad request.>"));
}

/**
 * @brief Play a game of Cho-Han
 * @param request: the request to play Cho-Han, split into parts separated by '>'
 */
void Oasis::playChoHan(QList<QString> request)
{
    QString response = ChoHan::handleRequest(activePlayers[request[2]], request);
    sender->sendMessage(response);
}

/**
 * @brief Checks if any of the active players have gone inactive and removes those from the active list. Also removes any inactive blackjack instances.
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
    QList<Blackjack*>::iterator j = blackjackInstances.begin();
    while (j != blackjackInstances.end()) {
        if ((*j)->getPlayer() == nullptr || !activePlayers.contains((*j)->getPlayer()->getName()))
            j = blackjackInstances.erase(j);
        else
            ++j;
    }
}
