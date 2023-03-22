#include "item.h"

Item::Item(int id, QString name, Rarity rarity, int ogValue, int limited, bool consumable, QString effectParam, int effectValue) :
    id(id), name(name), rarity(rarity), ogValue(ogValue), limited(limited), consumable(consumable), effectParam(effectParam), effectValue(effectValue)
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

bool Item::getConsumable() const
{
    return consumable;
}

const QString &Item::getEffectParam() const
{
    return effectParam;
}

int Item::getEffectValue() const
{
    return effectValue;
}
