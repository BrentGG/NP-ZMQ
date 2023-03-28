#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "player.h"

#include <QString>
#include <QSqlDatabase>


class DbManager
{
public:
    DbManager(QString path);

    bool addPlayer(Player *player, QString password);
    Player* getPlayerByNameAndPassword(QString name, QString password);
    bool setCredits(QString username, int credits);

private:
    QSqlDatabase db;
};

#endif // DBMANAGER_H
