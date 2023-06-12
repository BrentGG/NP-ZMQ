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

    bool addPlayer(Player *player, QString password);
    Player* getPlayerByNameAndPassword(QString name, QString password);
    bool checkPassword(QString name, QString password);
    bool setCredits(QString username, int credits);
    QList<QString> getAllPlayerNames();

    bool isConnected();

private:
    QSqlDatabase db;
};

#endif // DBMANAGER_H
