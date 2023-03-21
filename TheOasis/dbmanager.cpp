#include "dbmanager.h"

#include <iostream>
#include <QSqlQuery>
#include <QSqlError>

DbManager::DbManager(QString path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open())
      std::cout << "Failed to connect to database\n" << std::endl;
    else
      std::cout << "Connected to database" << std::endl;
}

bool DbManager::addPlayer(Player *player, QString password)
{
    bool success = false;
    QSqlQuery query;
    query.prepare("INSERT INTO players (name, password, money, health, happiness, satiation, hydration, items) VALUES (:name, :password, :money, :health, :happiness, :satiation, :hydration, :items)");
    query.bindValue(":name", player->getName());
    query.bindValue(":password", password);
    query.bindValue(":money", player->getMoney());
    query.bindValue(":health", player->getHealth());
    query.bindValue(":happiness", player->getHappiness());
    query.bindValue(":satiation", player->getSatiation());
    query.bindValue(":hydration", player->getHydration());
    query.bindValue(":items", ""); // TODO: convert item list to string
    if(query.exec())
       success = true;
    else
        std::cout << "addPlayer error:" << query.lastError().text().toStdString() << std::endl;
    return success;
}
