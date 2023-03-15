#include <QCoreApplication>
#include <iostream>
#include <nzmqt/nzmqt.hpp>
#include <nzmqt/impl.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    try
    {
        nzmqt::ZMQContext *context = nzmqt::createDefaultContext(&a);
        nzmqt::ZMQSocket *pusher = context->createSocket(nzmqt::ZMQSocket::TYP_PUSH, context);
        nzmqt::ZMQSocket *subscriber = context->createSocket(nzmqt::ZMQSocket::TYP_SUB, context);
        //QObject::connect(subscriber, &nzmqt::ZMQSocket::messageReceived, printMessages);
        QObject::connect(subscriber, &nzmqt::ZMQSocket::messageReceived, []( const QList<QByteArray>& messages) {
            if(messages.size() < 1) {
                std::cout << "Received empty message." << std::endl;
            }
            else if(messages.size() == 1) {
                std::cout << "Received (" << messages.constFirst().size() << ") : " << messages.constFirst().toStdString() << std::endl;
            }
            else {
                std::cout << "Received " << messages.size() << " parts" << std::endl;
                int part = 1;
                for( const QByteArray & message : messages ) {
                    std::cout << "Part " << part << " (" << message.size() << ") : " << message.toStdString() << std::endl;
                    ++part;
                }
            }
        });

        pusher->connectTo("tcp://benternet.pxl-ea-ict.be:24041");
        subscriber->connectTo("tcp://benternet.pxl-ea-ict.be:24042");

        subscriber->subscribeTo("test>6969>");

        if(!pusher->isConnected() || !subscriber->isConnected()) {
            std::cerr << "Pusher and/or subscriber not connected." << std::endl;
        }

        context->start();
    }
    catch(nzmqt::ZMQException & ex) {
        std::cerr << "Caught an exception : " << ex.what();
    }
    return a.exec();
}
