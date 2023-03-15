#ifndef ITEM_H
#define ITEM_H

#include <QString>


class Item
{
public:
    enum Rarity {
        COMMON, // grey
        UNCOMMON, // green
        RARE, // light blue
        VERY_RARE, // dark blue
        EPIC, // purple
        LEGENDARY, // yellow
        MYTHIC // red
    };

    Item(int id, QString name, Rarity rarity, int ogValue, int limited = -1);

    int getId() const;
    QString getName() const;
    int getOgValue() const;
    Rarity getRarity() const;
    int getLimited() const;

private:
    int id;
    QString name;
    Rarity rarity;
    int ogValue;
    int limited; // -1 = unlimited, limited means there can only be a limited amount of this item in The Oasis
};

#endif // ITEM_H
