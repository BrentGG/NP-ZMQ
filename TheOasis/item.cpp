#include "item.h"

Item::Item(QString name, Rarity rarity, int ogValue, int limited) : name(name), rarity(rarity), ogValue(ogValue), limited(limited)
{

}

QString Item::getName() const
{
    return name;
}

void Item::setRarity(Rarity newRarity)
{
    rarity = newRarity;
}

int Item::getOgValue() const
{
    return ogValue;
}

void Item::setLimited(int newLimited)
{
    limited = newLimited;
}
