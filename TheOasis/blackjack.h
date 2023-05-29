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
    QList<QList<QPair<Suit, int>>> playerCards;
    QList<QPair<Suit, int>> dealerCards;
    int bet;
    int decks;
    QPair<Suit, int> holeCard;
    int currentHand;

    void fillShoe();
    QPair<Suit, int> getCard();
    QString startRound();
    void hit();
    void stand();
    void split();
    void double_();
    void insurance();
    QString cardToString(QPair<Suit, int> card);
    QString getResponseCards();
    QString endTurn();
    int calcScore(QList<QPair<Suit, int>> cards);
    void endRound();
    bool checkCardUsed(QPair<Suit, int> card);
};

#endif // BLACKJACK_H
