#ifndef ROULETTE_H
#define ROULETTE_H

#include "player.h"

#include <QString>
#include <QList>

/**
 * @brief An American, double-zero roulette game.
 */
class Roulette
{
public:
    Roulette();

    enum BetName {
        ZERO,                 // 0
        DOUBLE_ZERO,          // 00
        STRAIGHT_UP,          // any number
        ROW,                  // 0, 00
        SPLIT,                // any two adjoining numbers vertical or horizontal
        STREET,               // any three numbers horizontal
        CORNER,               // any four adjoining numbers in a block
        BASKET,               // 0, 00, 1, 2, 3
        DOUBLE_STREET,        // any six numbers from two horizontal rows
        FIRST_COLUMN,         // 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34
        SECOND_COLUMN,        // 2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35
        THIRD_COLUMN,         // 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36
        FIRST_DOZEN,          // 1 through 12
        SECOND_DOZEN,         // 13 through 24
        THIRD_DOZEN,          // 25 through 36
        ODD,                  // 1, 3, 5, ..., 35
        EVEN,                 // 2, 4, 6, ..., 36
        RED,                  // 32, 19, 21, 25, 34, 27, 36, 30, 23, 5, 16, 1, 14, 9, 18, 7, 12, 3
        BLACK,                // 15, 4, 2, 17, 6, 13, 11, 8, 10, 24, 33, 20, 31, 22, 29, 28, 35, 26
        ONE_TO_EIGHTEEN,      // 1, 2, 3, ..., 18
        NINETEEN_TO_THIRTYSIX,// 19, 20, 21, ..., 36
        UNKNOWN
    };

    static QString getInfo();
    static QString handleRequest(Player* player, QList<QString> request);
    static int spin();
    static int calcPayout(int rouletteNumber, int bet, BetName betName, QList<int> betNumbers);
    static BetName strToBetName(QString betNameStr);
    static bool areValidBetNumbers(BetName betName, QList<int> betNumbers);
    static QList<int> strToIntList(QString str);
};

#endif // ROULETTE_H
