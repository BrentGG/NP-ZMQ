#include "oasis.h"

#include <iostream>

Oasis::Oasis(QCoreApplication *coreApp)
{
    try {
        dbManager = new DbManager("./theoasis.db");

        context = nzmqt::createDefaultContext(coreApp);
        pusher = context->createSocket(nzmqt::ZMQSocket::TYP_PUSH, context);
        subscriber = context->createSocket(nzmqt::ZMQSocket::TYP_SUB, context);
        QObject::connect(subscriber, &nzmqt::ZMQSocket::messageReceived, this, &Oasis::handleMessage);
    }
    catch(nzmqt::ZMQException & ex) {
        std::cerr << "Caught an exception : " << ex.what();
    }
}

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

void Oasis::handleMessage(const QList<QByteArray> &messages)
{
    for( const QByteArray & message : messages ) {
        std::cout << "Recieved: " << message.toStdString() << std::endl;
        /*QString msg = QString(message);
        QList<QString> parts = msg.split(">");
        if (parts.size() > 1) {
            if (parts[1].compare("register") == 1) {

            }
        }*/
    }
}

/*
 * REQ: theoasis>register?>[username:string]>[password:string]>
 * RES: theoasis>register!>[username:string]>[success:bool]> (client should subscribe to topic including username they sent)
 */
bool registerPlayer(QList<QString> input) {
    if (input.size() >= 4) {

    }
    return false;
}
