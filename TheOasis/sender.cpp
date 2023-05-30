#include "sender.h"
#include "slotmachine.h"
#include "roulette.h"
#include "blackjack.h"
#include "chohan.h"
#include "failedrequest.h"

#include <iostream>

Sender::Sender(nzmqt::ZMQSocket *pusher): pusher(pusher)
{
}

/**
 * @brief Sends a message using the pusher.
 * @param message: the message to be sent
 */
void Sender::sendMessage(QString message)
{
    nzmqt::ZMQMessage messageZmq = nzmqt::ZMQMessage(message.toUtf8());
    pusher->sendMessage(messageZmq);
    std::cout << "Sent: " << message.toStdString() << std::endl;
    sendLog(message, false);
}

/**
 * @brief Sends an informational message about The Oasis or one of its activities.
 * @param request: the request to register, split into parts seperated by '>'
 * REQ: theoasis>info?>
 * RES: {theoasis>info!>}[info:string]>
 * or
 * REQ: theoasis>info?>[about:string]
 * RES: {theoasis>info!>[about:string]>}[info:string]>
 */
void Sender::sendInfo(QList<QString> request)
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
    else if (request[2].compare("slotmachine") == 0)
        sendMessage(SlotMachine::getInfo());
    else if (request[2].compare("roulette") == 0)
        sendMessage(Roulette::getInfo());
    else if (request[2].compare("blackjack") == 0)
        sendMessage(Blackjack::getInfo());
    else if (request[2].compare("cho-han") == 0)
        sendMessage(ChoHan::getInfo());
    else
        throw FailedRequest(QString("theoasis>info!>" + request[2] + ">false>No info on this subject.>"));
}

void Sender::sendLog(QString message, bool received)
{
    QString logMsg = QString("theoasis>log!>");
    logMsg.append(received ? "received>" : "sent>");
    logMsg.append("[" + message + "]>");
    nzmqt::ZMQMessage logMsgZmq = nzmqt::ZMQMessage(logMsg.toUtf8());
    pusher->sendMessage(logMsgZmq);
}

/**
 * @brief Sends a help message for those unfamiliar with The Oasis.
 */
void Sender::sendHelp()
{
    QString response = QString("theoasis>help!>\n\
>>> The Oasis: Help <<<\n\n\
Welcome To The Oasis!\n\n\
The Oasis is a virtual casino. Players have an account that stores the amount of credits they have. Everyone starts with 1000 credits. \
The available games are slotmachines, roulette, Blackjack and Cho-Han. For more information on a specific part of The Oasis, send the following: \
'theoasis>info?>'.\nHave fun!\n\n\
");
    sendMessage(response);
}

/**
 * @brief Send out an ad for The Oasis (unused)
 */
void Sender::sendAd()
{
    sendMessage(QString("Join The Oasis Now! Request 'theoasis>info?>' and subscribe to 'theoasis>info!>'."));
}
