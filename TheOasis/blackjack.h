#ifndef BLACKJACK_H
#define BLACKJACK_H

#include "player.h"

#include <QList>
#include <QPair>

/**
 * @brief A blackjack game. There should only be one instance per player, this simulates the player sitting at a blackjack table.
 */
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

    static QString getInfo();
    QString handleRequest(QList<QString> request);
    Player* getPlayer();
    void leave();

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
    bool insured;

    void fillShoe();
    QPair<Suit, int> getCard();
    QString startRound();
    void hit();
    void stand();
    void split();
    void double_();
    void insurance();
    int calcScore(QList<QPair<Suit, int>> cards);
    QString endTurn();
    void endRound();
    QString cardToString(QPair<Suit, int> card);
    QString getResponseCards();
    bool checkCardUsed(QPair<Suit, int> card);
};

#endif // BLACKJACK_H
