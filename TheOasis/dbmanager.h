#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "player.h"

#include <QString>
#include <QSqlDatabase>
#include <QList>

class DbManager
{
public:
    DbManager(QString path);

    bool addPlayer(Player *player, QString password, QString email);
    Player* getPlayerByNameAndPassword(QString name, QString password);
    bool checkPassword(QString name, QString password);
    bool setCredits(QString username, int credits);
    QList<QString> getAllPlayerNames();
    QString getPlayerEmail(QString username);
    QString getPlayerPassword(QString username);

    bool isConnected();

private:
    QSqlDatabase db;
};

#endif // DBMANAGER_H
