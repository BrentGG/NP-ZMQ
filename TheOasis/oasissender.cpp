#include "oasissender.h"
#include "slotmachine.h"
#include "roulette.h"
#include "blackjack.h"
#include "chohan.h"
#include "failedrequest.h"

#include <iostream>

OasisSender::OasisSender(nzmqt::ZMQSocket *pusher): pusher(pusher)
{
}

/**
 * @brief Sends a message using the pusher.
 * @param message: the message to be sent
 */
void OasisSender::sendMessage(QString message, bool log)
{
    nzmqt::ZMQMessage messageZmq = nzmqt::ZMQMessage(message.toUtf8());
    pusher->sendMessage(messageZmq);
    std::cout << "Sent: " << message.toStdString() << std::endl;
    if (log)
        sendLog(message, false);
}

/**
 * @brief Sends an informational message about The Oasis or one of its activities.
 * @param request: the request to register, split into parts seperated by '>'
 */
void OasisSender::sendInfo(QList<QString> request)
{
    if (request.size() > 3) {
        if (request[2].compare("slotmachine") == 0)
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
    else
        throw FailedRequest(QString("theoasis>info!>" + request[2] + ">false>Specify a subject to get more info on.>"));
}

/**
 * @brief Send a message on the log channel
 * @param message: the message to send
 * @param received: true if this is a message that was received
 */
void OasisSender::sendLog(QString message, bool received)
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
void OasisSender::sendHelp()
{
    QString response = QString("theoasis>help!>\n\
>>> The Oasis: Help <<<\n\n\
Welcome To The Oasis!\n\n\
The Oasis is a virtual casino. Players have an account that stores the amount of credits they have. Everyone starts with 1000 credits. \
The available games are slotmachines, roulette, Blackjack and Cho-Han. For more information on a specific part of The Oasis, send the following: \
'theoasis>info?>'. To know which requests you can make, checkout the documentation on the Github repository: https://github.com/BrentGG/NP-ZMQ.\nHave fun!\n\n\
");
    sendMessage(response);
}
