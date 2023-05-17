#ifndef BLACKJACK_H
#define BLACKJACK_H

#include "player.h"

#include <QList>
#include <QPair>

class Blackjack
{
public:
    Blackjack();

    enum Suit {
        SPADES,
        HEARTS,
        CLUBS,
        DIAMONDS
    };

private:
    Player* player;
    QList<QPair<Suit, int>> shoe;
    QList<QPair<Suit, int>> usedCards;
    QList<QPair<Suit, int>> currentCards;
    int currentBet;
};

#endif // BLACKJACK_H
