#ifndef CHOHAN_H
#define CHOHAN_H

#include "player.h"

/**
 * @brief A simple Japanese dice game, bet on the sum of two dice being even or odd.
 */
class ChoHan
{
public:
    ChoHan();

    static QString getInfo();
    static QString handleRequest(Player* player, QList<QString> request);
    static QList<int> rollDice();
    static int calcPayout(QList<int> dice, int bet, bool even);
};

#endif // CHOHAN_H
