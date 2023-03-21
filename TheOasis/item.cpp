#include "item.h"

Item::Item(int id, QString name, Rarity rarity, int ogValue, int limited) : id(id), name(name), rarity(rarity), ogValue(ogValue), limited(limited)
{

}

int Item::getId() const
{
    return id;
}

QString Item::getName() const
{
    return name;
}

Item::Rarity Item::getRarity() const
{
    return rarity;
}

int Item::getOgValue() const
{
    return ogValue;
}

int Item::getLimited() const
{
    return limited;
}
