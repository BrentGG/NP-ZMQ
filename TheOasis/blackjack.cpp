#include "blackjack.h"
#include "failedrequest.h"

#include <QRandomGenerator>

Blackjack::Blackjack(Player* player, int decks) : player(player), decks(decks)
{
    fillShoe();
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
                return hit();
            else if (request[3].compare("stand") == 0)
                return stand();
            else if (request[3].compare("split") == 0)
                return split();
            else if (request[3].compare("double") == 0)
                return double_();
            else if (request[3].compare("insurance") == 0)
                return insurance();
            else
                throw FailedRequest(QString("theoasis>blackjack!>" + request[2] + ">false>Unknown command.>"));
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
            if (!playerCards.contains(card) && !dealerCards.contains(card) && holeCard != card)
                shoe.append(card);
            card = QPair<Suit, int>(HEARTS, k);
            if (!playerCards.contains(card) && !dealerCards.contains(card) && holeCard != card)
                shoe.append(card);
            card = QPair<Suit, int>(CLUBS, k);
            if (!playerCards.contains(card) && !dealerCards.contains(card) && holeCard != card)
                shoe.append(card);
            card = QPair<Suit, int>(DIAMONDS, k);
            if (!playerCards.contains(card) && !dealerCards.contains(card) && holeCard != card)
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
        playerCards.append(getCard());
        playerCards.append(getCard());
        dealerCards.append(getCard());
        holeCard = getCard();
        shoe.removeFirst();
        player->modifyCredits(-bet);

        int playerScore = calcScore(playerCards);
        dealerCards.append(holeCard);
        int dealerScore = calcScore(dealerCards);
        QString response = getResponseString();
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
        return getResponseString();
    }
    else
        throw FailedRequest(QString("theoasis>blackjack!>" + player->getName() + ">false>Bet already placed.>"));
}

QString Blackjack::hit()
{
    playerCards.append(getCard());
    QString response = getResponseString();
    if (calcScore(playerCards) > 21) {
        response.append("0>You bust!>");
        endRound();
    }
    if (shoe.size() < 52)
        fillShoe();
    if (calcScore(playerCards) == 21)
        stand();
    return response;
}

QString Blackjack::stand()
{
    dealerCards.append(holeCard);
    while (calcScore(dealerCards) < 17) {
        dealerCards.append(getCard());
        shoe.removeFirst();
    }
    int dealerScore = calcScore(dealerCards);
    int playerScore = calcScore(playerCards);
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
    QString response = getResponseString();
    response.append(QString::number(payout) + ">");
    if (payout == 0)
        response.append("You lost!>");
    else if (payout == bet)
        response.append("Push!>");
    else
        response.append("You won!>");
    endRound();
    if (shoe.size() < 52)
        fillShoe();
    return response;
}

QString Blackjack::split()
{

}

QString Blackjack::double_()
{

}

QString Blackjack::insurance()
{

}

QString Blackjack::suitEnumToString(Suit suit)
{
    if (suit == SPADES)
        return QString("spades");
    else if (suit == HEARTS)
        return QString("hearts");
    else if (suit == CLUBS)
        return QString("clubs");
    else if (suit == DIAMONDS)
        return QString("diamonds");
    return "";
}

QString Blackjack::cardNumberToString(int number)
{
    if (number <= 10)
        return QString::number(number);
    else if (number == 11)
        return "jack";
    else if (number == 12)
        return "queen";
    else if (number == 13)
        return "king";
    return "";
}

QString Blackjack::getResponseString()
{
    QString response = QString("theoasis>blackjack!>" + player->getName() + ">true>");
    for (int i = 0; i < dealerCards.size(); ++i) {
        response.append(suitEnumToString(dealerCards[i].first) + "," + QString::number(dealerCards[i].second));
        if (i < dealerCards.size() - 1)
            response.append(",");
    }
    response.append(">" + QString::number(calcScore(dealerCards)) + ">");
    for (int i = 0; i < playerCards.size(); ++i) {
        response.append(suitEnumToString(playerCards[i].first) + "," + QString::number(playerCards[i].second));
        if (i < playerCards.size() - 1)
            response.append(",");
    }
    response.append(">" + QString::number(calcScore(playerCards)) + ">");
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
}
