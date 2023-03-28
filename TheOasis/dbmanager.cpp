#include "dbmanager.h"
#include "player.h"

#include <iostream>
#include <QSqlQuery>
#include <QSqlError>

DbManager::DbManager(QString path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();
}

bool DbManager::addPlayer(Player *player, QString password)
{
    bool success = false;
    QSqlQuery query;
    query.prepare("INSERT INTO Players (name, password, credits) VALUES (:name, :password, :credits)");
    query.bindValue(":name", player->getName());
    query.bindValue(":password", password);
    query.bindValue(":credits", player->getCredits());
    if(query.exec())
       success = true;
    else
        std::cout << "addPlayer error: " << query.lastError().text().toStdString() << std::endl;
    return success;
}

Player* DbManager::getPlayerByNameAndPassword(QString name, QString password)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM Players WHERE name=:name AND password=:password");
    query.bindValue(":name", name);
    query.bindValue(":password", password);
    if(query.exec()) {
        query.first();
        if (query.isValid())
            return new Player(query.value(0).toString(), query.value(2).toInt());
        return nullptr;
    }
    std::cout << "getPlayerByNameAndPassword error: " << query.lastError().text().toStdString() << std::endl;
    return nullptr;
}

bool DbManager::checkPassword(QString name, QString password)
{
    return getPlayerByNameAndPassword(name, password) != nullptr;
}

bool DbManager::setCredits(QString username, int credits)
{
    bool success = false;
    QSqlQuery query;
    query.prepare("UPDATE Players SET credits=:credits WHERE name=:name");
    query.bindValue(":name", username);
    query.bindValue(":credits", credits);
    if(query.exec())
       success = true;
    else
        std::cout << "setCredits error: " << query.lastError().text().toStdString() << std::endl;
    return success;
}

bool DbManager::isConnected()
{
    return db.isOpen();
}
