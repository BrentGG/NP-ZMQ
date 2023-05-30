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

        QThread *thread = QThread::create([this]{
            QTextStream s(stdin);
            while(1)
            {
                std::cout << "What would you like to do?\n(1) Help\n(2) Info\n(3) Login\n(4) Register\n(5) Logout\n(6) Play slotmachine\n(7) Play Roulette\n(8) Play Blackjack\n(9) Play Cho-Han\n(10) Exit\n";
                int choice = getMenuInput(1, 10);
                if (choice == 1)
                    this->help();
                else if (choice == 2)
                    this->info();
                while (waiting) {};
            }
        });
        thread->start();
    }
    catch(nzmqt::ZMQException & ex) {
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
                else if (response[1].compare("login!") == 0)
                    completeLogin(response);
                else
                    continue;
                waiting = false;
            } catch(FailedRequest &e) {
                std::cout << e.what().toStdString() << std::endl;
            } catch(...) {
                printf("An unexpected error occured.\n");
            }
        }
    }
}

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

void OasisClient::sendMessage(QString message)
{
    nzmqt::ZMQMessage messageZmq = nzmqt::ZMQMessage(message.toUtf8());
    pusher->sendMessage(messageZmq);
}

void OasisClient::help()
{
    waiting = true;
    helpRequested = true;
    sendMessage("theoasis>help?>");
}

void OasisClient::completeHelp(QString response)
{
    helpRequested = false;
    std::cout << response.last(response.size() - QString("theoasis>help!>").size()).toStdString() << std::endl;
}

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

void OasisClient::completeInfo(QString response)
{
    infoRequested = false;
    infoTopic = "";
    std::cout << response.last(response.size() - (response.indexOf(">", QString("theoasis>info!>").size()) + 1)).toStdString() << std::endl;
}

void OasisClient::login()
{
    QTextStream s(stdin);
    std::cout << "Username: ";
    username = s.readLine();
    std::cout << "Password: ";
    password = s.readLine();
    subscriber->subscribeTo("theoasis>login!>" + username + ">");
    sendMessage("theoasis>login?>" + username + ">" + password + ">");
}

void OasisClient::completeLogin(QList<QString> response)
{
    if (response.size() > 3 && response[3].compare("true") == 0) {
        loggedIn = true;
        std::cout << "Successfully logged in.\n";
    }
    else
        throw FailedRequest("Login attempt failed.");
}
