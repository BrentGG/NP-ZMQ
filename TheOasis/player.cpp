#include "player.h"

Player::Player(QString name, int money, int health, int happiness, int satiation, int hydration) : name(name), money(money), health(health), happiness(happiness), satiation(satiation), hydration(hydration)
{

}

QString Player::getName() const
{
    return name;
}

int Player::getMoney() const
{
    return money;
}

int Player::getHealth() const
{
    return health;
}

int Player::getHappiness() const
{
    return happiness;
}

int Player::getSatiation() const
{
    return satiation;
}

int Player::getHydration() const
{
    return hydration;
}

QList<Item *> Player::getItems() const
{
    return items;
}
