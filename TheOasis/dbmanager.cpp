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

bool DbManager::addPlayer(Player *player, QString password, QString email)
{
    bool success = false;
    QSqlQuery query;
    query.prepare("INSERT INTO Players (name, password, credits, email) VALUES (:name, :password, :credits, :email)");
    query.bindValue(":name", player->getName());
    query.bindValue(":password", password);
    query.bindValue(":credits", player->getCredits());
    query.bindValue(":email", email);
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

QList<QString> DbManager::getAllPlayerNames()
{
    QList<QString> names = QList<QString>();
    QSqlQuery query;
    query.prepare("SELECT name FROM Players");
    if(query.exec()) {
        while(query.next())
            names.append(query.value(0).toString());
        return names;
    }
    std::cout << "getAllPlayerNames error: " << query.lastError().text().toStdString() << std::endl;
}

QString DbManager::getPlayerEmail(QString username)
{
    QSqlQuery query;
    query.prepare("SELECT email FROM Players WHERE name=:name");
    query.bindValue(":name", username);
    if(query.exec()) {
        query.first();
        if (query.isValid())
            return query.value(0).toString();
        return "";
    }
    std::cout << "getPlayerEmail error: " << query.lastError().text().toStdString() << std::endl;
    return "";
}

QString DbManager::getPlayerPassword(QString username)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM Players WHERE name=:name");
    query.bindValue(":name", username);
    if(query.exec()) {
        query.first();
        if (query.isValid())
            return query.value(0).toString();
        return "";
    }
    std::cout << "getPlayerPassword error: " << query.lastError().text().toStdString() << std::endl;
    return "";
}

bool DbManager::isConnected()
{
    return db.isOpen();
}
