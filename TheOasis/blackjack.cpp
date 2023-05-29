#include "blackjack.h"
#include "failedrequest.h"

#include <QRandomGenerator>

Blackjack::Blackjack(Player* player, int decks) : player(player), decks(decks)
{
    fillShoe();
    currentHand = 0;
}

QString Blackjack::handleRequest(QList<QString> request)
{
    bool isNumber = false;
    int reqBet = request[3].toInt(&isNumber);
    if (isNumber) {
        if (reqBet <= 0 || reqBet > player->getCredits())
            throw FailedRequest(QString("theoasis>blackjack!>" + request[2] + ">false>Invalid bet.>"));
        bet = reqBet;
        return startRound();
    }
    else {
        if (request[3].compare("leave") == 0) {
            player = nullptr;
            return QString("theoasis>blackjack!>" + request[2] + ">true>Left the Blackjack game.>");
        }
        else if (request[3].compare("shoesize") == 0) {
            return QString("theoasis>blackjack!>" + request[2] + ">true>" + QString::number(shoe.size()) + ">");
        }
        if (playerCards.size() > 0) {
            if (request[3].compare("hit") == 0)
                hit();
            else if (request[3].compare("stand") == 0)
                stand();
            else if (request[3].compare("split") == 0)
                split();
            else if (request[3].compare("double") == 0)
                double_();
            else if (request[3].compare("insurance") == 0)
                insurance();
            else
                throw FailedRequest(QString("theoasis>blackjack!>" + request[2] + ">false>Unknown command.>"));
            return endTurn();
        }
        else
            throw FailedRequest(QString("theoasis>blackjack!>" + request[2] + ">false>Place a bet first.>"));
    }
    return "";
}

Player *Blackjack::getPlayer()
{
    return player;
}

void Blackjack::fillShoe()
{
    shoe.clear();
    for (int i = 0; i < decks; ++i) {
        for (int k = 1; k < 14; ++k) {
            QPair<Suit, int> card = QPair<Suit, int>(SPADES, k);
            if (!checkCardUsed(card))
                shoe.append(card);
            card = QPair<Suit, int>(HEARTS, k);
            if (!checkCardUsed(card))
                shoe.append(card);
            card = QPair<Suit, int>(CLUBS, k);
            if (!checkCardUsed(card))
                shoe.append(card);
            card = QPair<Suit, int>(DIAMONDS, k);
            if (!checkCardUsed(card))
                shoe.append(card);
        }
    }
}

QPair<Blackjack::Suit, int> Blackjack::getCard()
{
    int index = QRandomGenerator::global()->bounded(0, shoe.size());
    QPair<Suit, int> card = shoe[index];
    shoe.removeAt(index);
    return card;
}

QString Blackjack::startRound()
{
    if (playerCards.size() == 0) {
        playerCards.append(QList<QPair<Suit, int>>());
        playerCards[0].append(getCard());
        playerCards[0].append(getCard());
        dealerCards.append(getCard());
        holeCard = getCard();
        shoe.removeFirst();
        player->modifyCredits(-bet);

        int playerScore = calcScore(playerCards[0]);
        dealerCards.append(holeCard);
        int dealerScore = calcScore(dealerCards);
        QString response = getResponseCards();
        if (playerScore == 21 && dealerScore != 21) {
            response.append(QString::number(bet * 2.5) + ">Blackjack!>");
            player->modifyCredits(2.5 * bet);
            endRound();
            return response;
        }
        else if (playerScore == 21 && dealerScore == 21) {
            response.append(QString::number(bet) + ">Push!>");
            player->modifyCredits(bet);
            return response;
        }
        else if (playerScore != 21 && dealerScore == 21) {
            response.append(QString::number(bet) + ">Dealer Blackjack!>");
            return response;
        }
        dealerCards.removeLast();
        return getResponseCards();
    }
    else
        throw FailedRequest(QString("theoasis>blackjack!>" + player->getName() + ">false>Bet already placed.>"));
}

void Blackjack::hit()
{
    playerCards[currentHand].append(getCard());
    if (shoe.size() < 52)
        fillShoe();
    if (calcScore(playerCards[currentHand]) == 21)
        stand();
    else if (calcScore(playerCards[currentHand]) > 21)
        currentHand += 1;
}

void Blackjack::stand()
{
    currentHand += 1;
    if (shoe.size() < 52)
        fillShoe();
}

void Blackjack::split()
{
    if (playerCards[currentHand].size() == 2 && (
                (playerCards[currentHand][0].second == playerCards[currentHand][1].second) || // equal card number
                ((playerCards[currentHand][0].second >= 10 || playerCards[currentHand][0].second == 1) && // both cards value 10
                 (playerCards[currentHand][1].second >= 10 || playerCards[currentHand][1].second == 1)))) {
        int secondHand = playerCards.size();
        playerCards.append(QList<QPair<Suit, int>>());
        playerCards[secondHand].append(playerCards[currentHand][1]);
        playerCards[currentHand][1] = getCard();
        playerCards[secondHand].append(getCard());
    }
    else
        throw FailedRequest(QString("theoasis>blackjack!>" + player->getName() + ">false>Can't split right now.>"));
}

void Blackjack::double_()
{

}

void Blackjack::insurance()
{

}

QString Blackjack::cardToString(QPair<Suit, int> card)
{
    QString cardStr = "";
    if (card.first == SPADES)
        cardStr.append("spades,");
    else if (card.first == HEARTS)
        cardStr.append("hearts,");
    else if (card.first == CLUBS)
        cardStr.append("clubs,");
    else if (card.first == DIAMONDS)
        cardStr.append("diamonds,");
    if (card.second <= 10)
        cardStr.append(QString::number(card.second));
    else if (card.second == 11)
        cardStr.append("J");
    else if (card.second == 12)
        cardStr.append("Q");
    else if (card.second == 13)
        cardStr.append("Q");
    return cardStr;
}

QString Blackjack::getResponseCards()
{
    QString response = QString("theoasis>blackjack!>" + player->getName() + ">true>");
    for (int i = 0; i < dealerCards.size(); ++i) {
        response.append(cardToString(dealerCards[i]));
        if (i < dealerCards.size() - 1)
            response.append(",");
    }
    response.append(">" + QString::number(calcScore(dealerCards)) + ">");
    for (int i = 0; i < playerCards.size(); ++i) {
        for (int j = 0; j < playerCards[i].size(); ++j) {
            response.append(cardToString(playerCards[i][j]));
            if (j < playerCards[i].size() - 1)
                response.append(",");
        }
        if (i < playerCards.size() - 1)
            response.append(";");
    }
    response.append(">");
    for (int i = 0; i < playerCards.size(); ++i) {
        response.append(QString::number(calcScore(playerCards[i])));
        if (i < playerCards.size() - 1)
            response.append(",");
    }
    response.append(">");
    if (currentHand < playerCards.size())
        response.append(QString::number(currentHand) + ">");
    return response;
}

QString Blackjack::endTurn()
{
    QString response;
    if (currentHand >= playerCards.size()) {
        QString feedback = "";
        int totalPayout = 0;
        dealerCards.append(holeCard);
        int dealerScore = calcScore(dealerCards);
        while (dealerScore < 17) {
            dealerCards.append(getCard());
            dealerScore = calcScore(dealerCards);
        }
        response = getResponseCards();
        for (int i = 0; i < playerCards.size(); ++i) {
            int playerScore = calcScore(playerCards[i]);
            int payout = 0;
            if (playerScore <= 21) {
                if (dealerScore > playerScore && dealerScore <= 21)
                    payout = 0;
                else if (dealerScore == playerScore)
                    payout = bet;
                else
                    payout = 2 * bet;
            }
            else {
                if (dealerScore > 21)
                    payout = bet;
                else
                    payout = 0;
            }
            totalPayout += payout;
            response.append(QString::number(payout));
            if (payout == 0)
                feedback.append("Hand lost.");
            else if (payout == bet)
                feedback.append("Push.");
            else
                feedback.append("Hand won!");
            if (i < playerCards.size() - 1) {
                response.append(",");
                feedback.append(",");
            }
        }
        response.append(">" + feedback + ">");
        player->modifyCredits(totalPayout);
        endRound();
    }
    else
        response = getResponseCards();
    return response;
}

int Blackjack::calcScore(QList<QPair<Suit, int> > cards)
{
    int score = 0;
    int amountOfAces = 0;
    for (QPair<Suit, int> card : cards) {
        if (card.second == 1)
            ++amountOfAces;
        score += card.second < 10 ? card.second : 10;
    }
    for (int i = 0; i < amountOfAces; ++i) {
        if (score + 9 <= 21)
            score += 9;
    }
    return score;
}

void Blackjack::endRound()
{
    bet = 0;
    dealerCards.clear();
    playerCards.clear();
    currentHand = 0;
}

bool Blackjack::checkCardUsed(QPair<Suit, int> card)
{
    for (QList<QPair<Suit, int>> hand : playerCards) {
        if (hand.contains(card))
            return true;
    }
    if (dealerCards.contains(card))
        return true;
    if (holeCard == card)
        return true;
    return false;
}
