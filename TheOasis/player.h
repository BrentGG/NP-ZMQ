#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QList>

#include "item.h"


class Player
{
public:
    Player(QString name, int money = 1000, int health = 100, int happiness = 100, int satiation = 100, int hydration = 100);

private:
    QString name;
    int money;
    int health;
    int happiness;
    int satiation;
    int hydration;
    QList<Item*> items;
};

#endif // PLAYER_H
