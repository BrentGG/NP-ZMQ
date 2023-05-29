#include "sender.h"
#include "slotmachine.h"
#include "roulette.h"
#include "blackjack.h"
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
    else
        throw FailedRequest(QString("theoasis>info!>" + request[2] + ">false>No info on this subject.>"));
}

/**
 * @brief Sends a help message for those unfamiliar with The Oasis.
 */
void Sender::sendHelp()
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
  RES: {theoasis>balance!>[username:string]>}[success:bool]>[balance:int]>[message:string]>\n\
- Play a three-reel, single-payline slot machine. The bet is the amount of money you put into the slot machine, which must be \
lower than player's balance and larger than 0. The payline is the three symbols you got. The payout is the amount of money you \
get back (if this is 0, you lost your bet).\n\
  REQ: theoasis>slotmachine?>[username:string]>[bet:integer]>\n\
  RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[payline:list<string>]>[payout:integer]>[message:string]>\n\
- Play American, double-zero roulette. Bet must be lower than your balance and higher than 0. The betName is the name of the \
bet you're placing (list can be found at 'theoasis>info?>roulette>'). Some bets require you to specify some numbers, these should \
be provided in betNumbers.\n\
  REQ: theoasis>roulette?>[username:string]>[bet:integer]>[betName:string]>[betNumbers:list<int>]>\n\
  RES: {theoasis>roulette!>[username:string]>}[success:bool]>[rouletteNumber:string]>[payout:integer]>[message:string]>\n\
");
    sendMessage(response);
}

/**
 * @brief Send out an ad for The Oasis
 */
void Sender::sendAd()
{
    sendMessage(QString("Join The Oasis Now! Request 'theoasis>info?>' and subscribe to 'theoasis>info!>'."));
}
