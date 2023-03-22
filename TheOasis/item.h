#ifndef ITEM_H
#define ITEM_H

#include <QString>


class Item
{
public:    
    enum Rarity {
        COMMON=0, // grey, 30% (+ the rest)
        UNCOMMON=1, // green, 20%
        RARE=2, // light blue, 10%
        VERY_RARE=3, // dark blue, 5%
        EPIC=4, // purple, 1%
        LEGENDARY=5, // yellow, 0.1%
        MYTHIC=5 // red, 0.001%
    };

    Item(int id, QString name, Rarity rarity, int ogValue, int limited = -1, bool consumable = false, QString effectParam = "", int effectValue = 0);

    int getId() const;
    QString getName() const;
    int getOgValue() const;
    Rarity getRarity() const;
    int getLimited() const;
    bool getConsumable() const;
    const QString &getEffectParam() const;
    int getEffectValue() const;

private:
    int id;
    QString name;
    Rarity rarity;
    int ogValue;
    int limited; // -1 = unlimited, limited means there can only be a limited amount of this item in The Oasis
    bool consumable;
    QString effectParam;
    int effectValue;
};

#endif // ITEM_H
