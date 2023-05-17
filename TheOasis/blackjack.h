#ifndef BLACKJACK_H
#define BLACKJACK_H

#include "player.h"

#include <QList>
#include <QPair>

class Blackjack
{
public:
    Blackjack(Player* player, int decks);

    enum Suit {
        SPADES,
        HEARTS,
        CLUBS,
        DIAMONDS
    };

    QString handleRequest(QList<QString> request);
    Player* getPlayer();

private:
    Player* player;
    QList<QPair<Suit, int>> shoe;
    QList<QPair<Suit, int>> usedCards;
    QList<QPair<Suit, int>> playerCards;
    QList<QPair<Suit, int>> dealerCards;
    int bet;
    int decks;
    QPair<Suit, int> holeCard;

    void fillShoe();
    QPair<Suit, int> getCard();
    QString startRound();
    QString hit();
    QString stand();
    QString split();
    QString double_();
    QString insurance();
    QString suitEnumToString(Suit suit);
    QString cardNumberToString(int number);
    QString getResponseString();
    int calcScore(QList<QPair<Suit, int>> cards);
    void endRound();
};

#endif // BLACKJACK_H
