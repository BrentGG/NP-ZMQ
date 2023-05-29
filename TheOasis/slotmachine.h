#ifndef SLOTMACHINE_H
#define SLOTMACHINE_H

#include "player.h"

#include <QList>
#include <QString>

/**
 * @brief A simple 3-reel, single-payline slot machine
 */
class SlotMachine
{
public:
    enum Symbols {
        BAR,
        CHERRY,
        PLUM,
        WATERMELON,
        ORANGE,
        LEMON,
        BLANK
    };

    static QString getInfo();
    static QString handleRequest(Player* player, QList<QString> request);
    static QList<Symbols> spin();
    static int calcPayout(QList<Symbols> payline, int bet);
    static QList<QString> paylineToStringList(QList<Symbols> payline);

private:
    static bool threeInARow(QList<Symbols> payline, Symbols symbol);
    static bool twoInARow(QList<Symbols> payline, Symbols symbol);
    static bool any(QList<Symbols> payline, Symbols symbol);
};

#endif // SLOTMACHINE_H
