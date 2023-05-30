#include "oasisclient.h"
#include "failedrequest.h"

#include <iostream>
#include <QThread>

OasisClient::OasisClient(QCoreApplication *coreApp)
{
    try {
        loggedIn = false;
        waiting = false;
        helpRequested = false;
        infoRequested = false;
        infoTopic = "";
        balance = -1;
        rouletteBets = QList<QString>({"zero", "double zero", "straight up", "row", "split", "street", "corner", "basket", "double street", "first column", "second column", "third column", "first dozen", "second dozen", "third dozen", "odd", "even", "red", "black", "one to eighteen", "nineteen to thirty-six"});
        blackjackActions = QList<QString>({"hit", "stand", "split", "double", "insurance"});
        inProgress = false;

        context = nzmqt::createDefaultContext(coreApp);
        pusher = context->createSocket(nzmqt::ZMQSocket::TYP_PUSH, context);
        subscriber = context->createSocket(nzmqt::ZMQSocket::TYP_SUB, context);

        QObject::connect(subscriber, &nzmqt::ZMQSocket::messageReceived, this, &OasisClient::handleMessage);
    }
    catch(nzmqt::ZMQException & ex) {
        std::cerr << "Caught an exception : " << ex.what();
        exit(-1);
    }
}

/**
 * @brief Runs the Oasis Client by connecting the pusher and subscriber
 */
void OasisClient::run()
{
    try {
        connect();

        std::cout << "Welcome to \n\
 _______ _             ____            _     \n\
|__   __| |           / __ \\          (_)    \n\
   | |  | |__   ___  | |  | | __ _ ___ _ ___ \n\
   | |  | '_ \\ / _ \\ | |  | |/ _` / __| / __| \n\
   | |  | | | |  __/ | |__| | (_| \\__ \\ \\__ \\ \n\
   |_|  |_| |_|\\___|  \\____/ \\__,_|___/_|___/ \n\
\n";
        QThread *thread = QThread::create([this]{
            QTextStream s(stdin);
            while(1)
            {
                if (!inProgress) {
                    std::cout << "\nWhat would you like to do?\n(1) Help\n(2) Info\n(3) Register\n(4) Login\n(5) Logout\n(6) Play slotmachine\n(7) Play Roulette\n(8) Play Blackjack\n(9) Play Cho-Han\n(10) Exit\n";
                    int choice = getMenuInput(1, 10);
                    if (choice == 1)
                        this->help();
                    else if (choice == 2)
                        this->info();
                    else if (choice == 3)
                        this->register_();
                    else if (choice == 4)
                        this->login();
                    else if (choice == 10)
                        this->exitClient();
                    else if (loggedIn) {
                        if (choice == 5)
                            this->logout();
                        else if (choice == 6)
                            this->slotmachine();
                        else if (choice == 7)
                            this->roulette();
                        else if (choice == 8)
                            this->blackjack();
                        else if (choice == 9)
                            this->choHan();
                    }
                    else
                        std::cout << "Log in first before doing this.\n";
                    while (waiting) {};
                }
            }
        });
        thread->start();
    }
    catch(nzmqt::ZMQException &ex) {
        std::cerr << "Caught an exception : " << ex.what();
        exit(-1);
    }
}

/**
 * @brief Connects the pusher and subscriber
 */
void OasisClient::connect()
{
    try {
        pusher->connectTo("tcp://benternet.pxl-ea-ict.be:24041");
        subscriber->connectTo("tcp://benternet.pxl-ea-ict.be:24042");
        if(!pusher->isConnected() || !subscriber->isConnected()) {
            std::cerr << "Pusher and/or subscriber not connected." << std::endl;
            exit(-1);
        }

        subscriber->subscribeTo("theoasis>info!>");
        subscriber->subscribeTo("theoasis>help!>");
        subscriber->subscribeTo("theoasis>register!>");

        context->start();
    }
    catch(nzmqt::ZMQException & ex) {
        std::cerr << "Caught an exception : " << ex.what();
        exit(-1);
    }
}

/**
 * @brief Handles incoming messages and calls the appropriate method to deal with each message.
 * @param messages: messages to process
 */
void OasisClient::handleMessage(const QList<QByteArray> &messages)
{
    for(const QByteArray &message : messages) {
        QString msg = QString::fromStdString(message.toStdString());
        QList<QString> response = msg.split(">");
        if (response.size() >= 2) {
            try {
                if (response[1].compare("help!") == 0 && helpRequested)
                    completeHelp(msg);
                else if (response[1].compare("info!") == 0 && infoRequested && response[2].compare(infoTopic) == 0)
                    completeInfo(msg);
                else if (response[1].compare("register!") == 0)
                    completeRegister(response);
                else if (response[1].compare("login!") == 0)
                    completeLogin(response);
                else if (response[1].compare("logout!") == 0)
                    completeLogout(response);
                else if (response[1].compare("balance!") == 0)
                    completeBalance(response);
                else if (response[1].compare("slotmachine!") == 0)
                    completeSlotmachine(response);
                else if (response[1].compare("roulette!") == 0)
                    completeRoulette(response);
                else if (response[1].compare("blackjack!") == 0)
                    completeBlackjack(response);
                else if (response[1].compare("cho-han!") == 0)
                    completeChoHan(response);
                else
                    continue;
                waiting = false;
            } catch(FailedRequest &e) {
                std::cout << e.what().toStdString() << std::endl;
                waiting = false;
            } catch(...) {
                printf("An unexpected error occured.\n");
                waiting = false;
            }
        }
    }
}

/**
 * @brief Ask the user to input a number to choose a menu item
 * @param min: the lowest menu item
 * @param max: the highest menu item
 * @return the chosen item
 */
int OasisClient::getMenuInput(int min, int max)
{
    QTextStream s(stdin);
    bool isNumber = false;
    while (1) {
        QString input = s.readLine();
        int choice = input.toInt(&isNumber);
        if (!isNumber || choice < min || choice > max)
            std::cout << "Please type in one of the numbers in the menu to choose an option.\n";
        else
            return choice;
    }
}

/**
 * @brief Ask the user to input a valid bet
 * @return the bet
 */
int OasisClient::getBet()
{
    std::cout << "Bet: ";
    QTextStream s(stdin);
    bool isNumber = false;
    int bet;
    while (1) {
        QString input = s.readLine();
        bet = input.toInt(&isNumber);
        if (!isNumber || bet <= 0 || bet > balance)
            std::cout << "Please type in a bet that is higher than 0 and doesn't exceed your balance.\n";
        else
            return bet;
    }
}

/**
 * @brief Send a message
 * @param message: the message
 */
void OasisClient::sendMessage(QString message)
{
    nzmqt::ZMQMessage messageZmq = nzmqt::ZMQMessage(message.toUtf8());
    pusher->sendMessage(messageZmq);
}

/**
 * @brief Ask The Oasis for help
 */
void OasisClient::help()
{
    waiting = true;
    helpRequested = true;
    sendMessage("theoasis>help?>");
}

/**
 * @brief Process the help response
 * @param response: the response
 */
void OasisClient::completeHelp(QString response)
{
    helpRequested = false;
    std::cout << response.last(response.size() - QString("theoasis>help!>").size()).toStdString() << std::endl;
}

/**
 * @brief Ask the oasis for info
 */
void OasisClient::info()
{
    std::cout << "What part of The Oasis would you like to know more about?\n(1) Slotmachines\n(2) Roulette\n(3) Blackjack\n(4) Cho-Han\n";
    int choice = getMenuInput(1, 4);
    QString request = QString("theoasis>info?>");
    if (choice == 1)
        infoTopic = "slotmachine";
    else if (choice == 2)
        infoTopic = "roulette";
    else if (choice == 3)
        infoTopic = "blackjack";
    else if (choice == 4)
        infoTopic = "cho-han";
    request.append(infoTopic + ">");
    waiting = true;
    infoRequested = true;
    sendMessage(request);
}

/**
 * @brief Process the info response
 * @param response: the response
 */
void OasisClient::completeInfo(QString response)
{
    infoRequested = false;
    infoTopic = "";
    std::cout << response.last(response.size() - (response.indexOf(">", QString("theoasis>info!>").size()) + 1)).toStdString() << std::endl;
}

/**
 * @brief Register to the oasis
 */
void OasisClient::register_()
{
    if (!loggedIn) {
        QTextStream s(stdin);
        std::cout << "Username: ";
        username = s.readLine();
        std::cout << "Password: ";
        password = s.readLine();
        subscriber->subscribeTo("theoasis>register!>" + username + ">");
        sendMessage("theoasis>register?>" + username + ">" + password + ">");
        waiting = true;
    }
    else
        throw FailedRequest("You are already logged in. Logout first if you want to register a new account.");
}

/**
 * @brief Process the register response
 * @param response: the response
 */
void OasisClient::completeRegister(QList<QString> response)
{
    if (response.size() > 3 && response[3].compare("true") == 0)
        std::cout << "Successfully registered. You can now log in.\n";
    else
        throw FailedRequest("Register attempt failed. Try using a different username.");
}

/**
 * @brief Login to the oasis
 */
void OasisClient::login()
{
    if (!loggedIn) {
        QTextStream s(stdin);
        std::cout << "Username: ";
        username = s.readLine();
        std::cout << "Password: ";
        password = s.readLine();
        subscriber->subscribeTo("theoasis>login!>" + username + ">");
        sendMessage("theoasis>login?>" + username + ">" + password + ">");
        waiting = true;
    }
    else
        throw FailedRequest("You are already logged in.");
}

/**
 * @brief Process the login response
 * @param response: the response
 */
void OasisClient::completeLogin(QList<QString> response)
{
    if (response.size() > 3 && response[3].compare("true") == 0) {
        loggedIn = true;
        subscriber->subscribeTo("theoasis>logout!>" + username + ">");
        subscriber->subscribeTo("theoasis>balance!>" + username + ">");
        subscriber->subscribeTo("theoasis>slotmachine!>" + username + ">");
        subscriber->subscribeTo("theoasis>roulette!>" + username + ">");
        subscriber->subscribeTo("theoasis>blackjack!>" + username + ">");
        subscriber->subscribeTo("theoasis>cho-han!>" + username + ">");
        std::cout << "Successfully logged in.\n";

        getBalance();
    }
    else
        throw FailedRequest("Login attempt failed.");
}

/**
 * @brief Logout of the oasis
 */
void OasisClient::logout()
{
    std::cout << "Are you sure you want to log out of The Oasis?\n(1) Yes, I hate having fun and I want to leave.\n(2) No, I am smart and I love having fun, keep me logged in.\n";
    int choice = getMenuInput(1, 2);
    if (choice == 1) {
        sendMessage("theoasis>logout?>" + username + ">" + password + ">");
        waiting = true;
    }
}

/**
 * @brief Process the logout response
 * @param response: the response
 */
void OasisClient::completeLogout(QList<QString> response)
{
    if (response.size() > 3 && response[3].compare("true") == 0) {
        loggedIn = false;
        std::cout << "Successfully logged out.\n";
    }
    else
        throw FailedRequest("Logout attempt failed.");
}

/**
 * @brief Ask the oasis for your balance
 */
void OasisClient::getBalance()
{
    this->sendMessage("theoasis>balance?>" + username + ">");
    waiting = true;
}

/**
 * @brief Process the balance response
 * @param response: the response
 */
void OasisClient::completeBalance(QList<QString> response)
{
    if (response.size() > 3 && response[3].compare("true") == 0)
        balance = response[4].toInt();
}

/**
 * @brief Play the slot machine game
 */
void OasisClient::slotmachine()
{
    std::cout << "\n>>> Slot Machine <<<\n";
    std::cout << "Balance: " << balance << " credits\n";
    int bet = getBet();
    sendMessage("theoasis>slotmachine?>" + username + ">" + QString::number(bet) + ">");
    waiting = true;
}

/**
 * @brief Process the slot machine response
 * @param response: the response
 */
void OasisClient::completeSlotmachine(QList<QString> response)
{
    if (response.size() > 3 && response[3].compare("true") == 0) {
        std::cout << "Payline: " << response[4].toStdString() << "\nPayout: " << response[5].toStdString() << "\n" << response[response.size() - 2].toStdString() << "\n";
        getBalance();
    }
    else
        throw FailedRequest(response[response.size() - 2]);
}

/**
 * @brief Play the roulette game
 */
void OasisClient::roulette()
{
    std::cout << "\n>>> Roulette <<<\n";
    std::cout << "Balance: " << balance << " credits\n";
    QString request = QString("theoasis>roulette?>" + username + ">");
    int bet = getBet();
    request.append(QString::number(bet) + ">");
    std::cout << "Choose a bet type:\n(1) Zero\n(2) Double zero\n(3) Straight up\n(4) Row\n(5) Split\n(6) Street\n(7) Corner\n(8) Basket \n(9) Double street\n(10) First column\n(11) Second column\n(12) Third column\n(13) First dozen\n(14) Second dozen\n(15) Third dozen\n(16) Odd\n(17) Even\n(18) Red\n(19) Black\n(20) 1-18\n(21) 19-36\n";
    int choice = getMenuInput(1, 21);
    request.append(rouletteBets[choice - 1] + ">");
    if (choice == 3)
        request.append(getRouletteNumbers(1));
    else if (choice == 5)
        request.append(getRouletteNumbers(2));
    else if (choice == 6)
        request.append(getRouletteNumbers(3));
    else if (choice == 7)
        request.append(getRouletteNumbers(4));
    else if (choice == 9)
        request.append(getRouletteNumbers(6));
    request.append(">");
    sendMessage(request);
    waiting = true;
}

/**
 * @brief Process the roulette response
 * @param response: the response
 */
void OasisClient::completeRoulette(QList<QString> response)
{
    if (response.size() > 3 && response[3].compare("true") == 0) {
        std::cout << "Roulette number: " << response[4].toStdString() << "\nPayout: " << response[5].toStdString() << "\n" << response[response.size() - 2].toStdString() << "\n";
        getBalance();
    }
    else
        throw FailedRequest(response[response.size() - 2]);
}

/**
 * @brief Ask the user to input the roulette numbers that correspond with their bet type
 * @param amount: the amount of numbers to ask
 * @return a string representing a list of numbers
 */
QString OasisClient::getRouletteNumbers(int amount)
{
    std::cout << "Please provide the numbers for this bet type.\n";
    QString numbers = "";
    QTextStream s(stdin);
    for (int i = 0; i < amount; ++i) {
        int number;
        while (1) {
            std::cout << "Number " << i + 1 << ": ";
            QString input = s.readLine();
            bool isNumber = false;
            number = input.toInt(&isNumber);
            if (!isNumber || number < 1 || number > 36)
                std::cout << "Please provide a number from 1 to 36.\n";
            else
                break;
        }
        numbers += QString::number(number);
        if (i < amount - 1)
            numbers += ",";
    }
    return numbers;
}

/**
 * @brief Play a blackjack game
 */
void OasisClient::blackjack()
{
    std::cout << "\n>>> Blackjack <<<\n";
    std::cout << "Balance: " << balance << " credits\n";
    int bet = getBet();
    sendMessage("theoasis>blackjack?>" + username + ">" + QString::number(bet) + ">");
    waiting = true;
    inProgress = true;
}

/**
 * @brief Process the blackjack response
 * @param response: the response
 */
void OasisClient::completeBlackjack(QList<QString> response)
{
    if (response.size() > 3) {
        if (response[3].compare("true") != 0)
            std::cout << response[response.size() - 2].toStdString() << "\n";
        if (response.size() > 9) {
            std::cout << "Dealer hand: " << response[4].toStdString() << "\n" << "Dealer score: " << response[5].toStdString() << "\n";
            std::cout << "Player hand(s):\n";
            QList<QString> playerHands = response[6].split(";");
            for (int i = 0; i < playerHands.size(); ++i)
                std::cout << "  " << i + 1 << ": " << playerHands[i].toStdString() << "\n";
            std::cout << "Player score(s):\n";
            QList<QString> playerScores = response[7].split(",");
            for (int i = 0; i < playerScores.size(); ++i)
                std::cout << "  " << i + 1 << ": " << playerScores[i].toStdString() << "\n";
        }
        if (response.size() > 11) {
            std::cout << "Round result(s):\n";
            QList<QString> results = response[10].split(",");
            for (int i = 0; i < results.size(); ++i)
                std::cout << "  " << i + 1 << ": " << results[i].toStdString() << "\n";
            std::cout << "Payout(s):\n";
            QList<QString> payouts = response[9].split(",");
            int totalPayout = 0;
            for (int i = 0; i < results.size(); ++i) {
                std::cout << "  " << i + 1 << ": " << payouts[i].toStdString() << "\n";
                totalPayout += payouts[i].toInt();
            }
            std::cout << "Total payout: " << totalPayout << "\n";
            getBalance();
            inProgress = false;
        }
        else {
            if (response.size() > 8)
                std::cout << "Current hand: " << response[8].toInt() + 1 << "\n";
            QString request = QString("theoasis>blackjack?>Brent>");
            std::cout << "\nWhat would you like to do?\n(1) Hit\n(2) Stand\n(3) Split\n(4) Double\n(5) Insurance\n";
            int choice = getMenuInput(1, 5);
            request.append(blackjackActions[choice - 1] + ">");
            sendMessage(request);
            waiting = true;
        }
    }
    else
        throw FailedRequest(response[response.size() - 2]);
}

/**
 * @brief Play a cho-han game
 */
void OasisClient::choHan()
{
    std::cout << "\n>>> Cho-Han <<<\n";
    std::cout << "Balance: " << balance << " credits\n";
    int bet = getBet();
    std::cout << "Even or odd:\n(1) Even\n(2) Odd\n";
    int choice = getMenuInput(1, 2);
    sendMessage("theoasis>cho-han?>" + username + ">" + QString::number(bet) + ">" + (choice == 1 ? "even" : "odd") + ">");
    waiting = true;
}

/**
 * @brief Process the cho-han response
 * @param response: the response
 */
void OasisClient::completeChoHan(QList<QString> response)
{
    if (response.size() > 3 && response[3].compare("true") == 0) {
        std::cout << "Dice: " << response[4].toStdString() << "\nPayout: " << response[5].toStdString() << "\n" << response[response.size() - 2].toStdString() << "\n";
        getBalance();
    }
    else
        throw FailedRequest(response[response.size() - 2]);
}

/**
 * @brief Exit the client
 */
void OasisClient::exitClient()
{
    sendMessage("theoasis>logout?>" + username + ">" + password + ">");
    std::cout << "Bye!\n";
    exit(0);
}


