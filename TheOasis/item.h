#ifndef ITEM_H
#define ITEM_H

#include <QString>


class Item
{
public:
    Item();

    enum class Rarity {
        COMMON, // grey
        UNCOMMON, // green
        RARE, // light blue
        VERY_RARE, // dark blue
        EPIC, // purple
        LEGENDARY, // yellow
        MYTHIC // red
    };

private:
    QString name;
    Rarity rarity;
    int ogValue;
    int limited; // -1 = unlimited, limited means there can only be a limited amount of this item in The Oasis
};

#endif // ITEM_H
