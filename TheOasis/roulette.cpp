#include "roulette.h"
#include "failedrequest.h"

#include <QRandomGenerator>
#include <algorithm>

Roulette::Roulette()
{
}

/**
 * @brief Return a string containing info about the roulette game.
 * @return The string
 */
QString Roulette::getInfo()
{
    return QString("theoasis>info!>roulette>\n\
>>> The Oasis: Roulette Info <<<\n\n\
The roulette wheels are American, double-zero roulette wheels. This means the numbers on the wheel are 0-36 and 00.\
Below are the possible bets, what they mean, and what their payouts are. The payout should be multiplied by the bet.\
If the description starts with 'any', you must specify in the request which numbers you choose.\n\
Bet Name                 Payout   Description\n\
zero                     36       0\n\
double zero              36       00\n\
straight up              36       any number (1-36)\n\
row                      18       0, 00\n\
split                    18       any two adjacent numbers (1-36), e.g.: 1,2 or 11,14\n\
street                   12       any three numbers horizontal (1-36), e.g.: 1,2,3 or 4,5,6 or 34,35,36\n\
corner                   9        any four adjoining numbers in a block, e.g.: 4,5,7,8\n\
basket                   7        0, 00, 1, 2, 3\n\
double street            6        any two streets, e.g.: 1,2,3,34,35,36\n\
first column             3        1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34\n\
second column            3        2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35\n\
third column             3        3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36\n\
first dozen              3        1 to 12\n\
second dozen             3        13 to 24\n\
third dozen              3        25 to 36\n\
odd                      2        odd numbers\n\
even                     2        even numbers\n\
red                      2        red numbers\n\
black                    2        black numbers\n\
one to eighteen          2        1 to 18\n\
nineteen to thirty-six   2        19 to 36\n\
    ");
}

/**
 * @brief Handle a request to the roulette game
 * @param player: the player making the request
 * @param request: the request
 * @return the response
 */
QString Roulette::handleRequest(Player* player, QList<QString> request)
{
    if (request.size() >= 5) {
        // Checks
        bool isNumber = false;
        int bet = request[3].toInt(&isNumber);
        if (!isNumber || bet <= 0 || bet > player->getCredits())
            throw FailedRequest(QString("theoasis>roulette!>" + request[2] + ">false>Invalid bet.>"));
        Roulette::BetName betName = Roulette::strToBetName(request[4]);
        if (betName == Roulette::UNKNOWN)
            throw FailedRequest(QString("theoasis>roulette!>" + request[2] + ">false>Invalid bet name.>"));
        QList<int> betNumbers;
        if (request.size() >= 6)
            betNumbers = Roulette::strToIntList(request[5]);
        if (!Roulette::areValidBetNumbers(betName, betNumbers))
            throw FailedRequest(QString("theoasis>roulette!>" + request[2] + ">false>Invalid bet numbers.>"));

        // Play roulette
        player->modifyCredits(-1 * bet);
        int rouletteNumber = Roulette::spin();
        int payout = Roulette::calcPayout(rouletteNumber, bet, betName, betNumbers);
        player->modifyCredits(payout);

        // Make result
        QString response = QString("theoasis>roulette!>" + request[2] + ">true>" + QString::number(rouletteNumber) + ">" + QString::number(payout) + ">" + (payout > 0 ? "Congratulations, you won!" : "You lost!"));
        return response;
    }
    else
        throw FailedRequest(QString("theoasis>roulette!>" + request[2] + ">false>Bad request.>"));
}

/**
 * @brief Spin the roulette wheel (a.k.a. return a random number on the roulette wheel)
 * @return The number the roulette landed on, 37 being 00.
 */
int Roulette::spin()
{
    return QRandomGenerator::global()->bounded(0, 38); // 0-36 + 00
}

/**
 * @brief Calculate the payout given the number the reoulette landed on, the bet, betname and betnumbers
 * @param rouletteNumber: the number the roulette landed on
 * @param bet: the bet amount
 * @param betName: the name of the bet
 * @param betNumbers: some bets require specifying numbers
 * @return The payout
 */
int Roulette::calcPayout(int rouletteNumber, int bet, BetName betName, QList<int> betNumbers)
{
    if (betName == ZERO && rouletteNumber == 0)
        return bet * 36;
    else if (betName == DOUBLE_ZERO && rouletteNumber == 37)
        return bet * 36;
    else if (betName == STRAIGHT_UP && rouletteNumber == betNumbers[0])
        return bet * 36;
    else if (betName == ROW && (rouletteNumber == 0 || rouletteNumber == 37))
        return bet * 18;
    else if (betName == SPLIT && betNumbers.contains(rouletteNumber))
        return bet * 18;
    else if (betName == STREET && betNumbers.contains(rouletteNumber))
        return bet * 12;
    else if (betName == CORNER && betNumbers.contains(rouletteNumber))
        return bet * 9;
    else if (betName == BASKET && QList({0, 1, 2, 3, 37}).contains(rouletteNumber))
        return bet * 7;
    else if (betName == DOUBLE_STREET && betNumbers.contains(rouletteNumber))
        return bet * 6;
    else if (betName == FIRST_COLUMN && QList({1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34}).contains(rouletteNumber))
        return bet * 3;
    else if (betName == SECOND_COLUMN && QList({2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35}).contains(rouletteNumber))
        return bet * 3;
    else if (betName == THIRD_COLUMN && QList({3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36}).contains(rouletteNumber))
        return bet * 3;
    else if (betName == FIRST_DOZEN && QList({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}).contains(rouletteNumber))
        return bet * 3;
    else if (betName == SECOND_DOZEN && QList({13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24}).contains(rouletteNumber))
        return bet * 3;
    else if (betName == THIRD_DOZEN && QList({25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36}).contains(rouletteNumber))
        return bet * 3;
    else if (betName == ODD && rouletteNumber % 2 != 0)
        return bet * 2;
    else if (betName == EVEN && rouletteNumber % 2 == 0)
        return bet * 2;
    else if (betName == RED && QList({32, 19, 21, 25, 34, 27, 36, 30, 23, 5, 16, 1, 14, 9, 18, 7, 12, 3}).contains(rouletteNumber))
        return bet * 2;
    else if (betName == BLACK && QList({15, 4, 2, 17, 6, 13, 11, 8, 10, 24, 33, 20, 31, 22, 29, 28, 35, 26}).contains(rouletteNumber))
        return bet * 2;
    else if (betName == ONE_TO_EIGHTEEN && QList({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18}).contains(rouletteNumber))
        return bet * 2;
    else if (betName == NINETEEN_TO_THIRTYSIX && QList({19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36}).contains(rouletteNumber))
        return bet * 2;
    return bet * 0;
}

/**
 * @brief Convert the string of a bet name to the enum of the bet name
 * @param betNameStr: the bet name as a string
 * @return The bet name as an enum
 */
Roulette::BetName Roulette::strToBetName(QString betNameStr)
{
    betNameStr = betNameStr.toLower();
    if (betNameStr.compare("zero") == 0)
        return ZERO;
    else if (betNameStr.compare("double zero") == 0)
        return DOUBLE_ZERO;
    else if (betNameStr.compare("straight up") == 0)
        return STRAIGHT_UP;
    else if (betNameStr.compare("row") == 0)
        return ROW;
    else if (betNameStr.compare("split") == 0)
        return SPLIT;
    else if (betNameStr.compare("street") == 0)
        return STREET;
    else if (betNameStr.compare("corner") == 0)
        return CORNER;
    else if (betNameStr.compare("basket") == 0)
        return BASKET;
    else if (betNameStr.compare("double street") == 0)
        return DOUBLE_STREET;
    else if (betNameStr.compare("first column") == 0)
        return FIRST_COLUMN;
    else if (betNameStr.compare("second column") == 0)
        return SECOND_COLUMN;
    else if (betNameStr.compare("third column") == 0)
        return THIRD_COLUMN;
    else if (betNameStr.compare("first dozen") == 0)
        return FIRST_DOZEN;
    else if (betNameStr.compare("second dozen") == 0)
        return SECOND_DOZEN;
    else if (betNameStr.compare("third dozen") == 0)
        return THIRD_DOZEN;
    else if (betNameStr.compare("odd") == 0)
        return ODD;
    else if (betNameStr.compare("even") == 0)
        return EVEN;
    else if (betNameStr.compare("red") == 0)
        return RED;
    else if (betNameStr.compare("black") == 0)
        return BLACK;
    else if (betNameStr.compare("one to eighteen") == 0)
        return ONE_TO_EIGHTEEN;
    else if (betNameStr.compare("nineteen to thirty-six") == 0)
        return NINETEEN_TO_THIRTYSIX;
    return UNKNOWN;
}

/**
 * @brief Check whether the given bet numbers are valid for a given bet name
 * @param betName: the name of a bet
 * @param betNumbers: the given numbers of the bet
 * @return True if the numbers are valid
 */
bool Roulette::areValidBetNumbers(BetName betName, QList<int> betNumbers)
{
    if (betNumbers.size() > 0) {
        std::sort(betNumbers.begin(), betNumbers.end());
        if (betNumbers[0] < 0 || betNumbers[betNumbers.size() - 1] > 37)
            return false;
    }
    else
        return true;
    if (betName == STRAIGHT_UP)
        return betNumbers.size() > 0;
    else if (betName == SPLIT)
        return betNumbers.size() >= 2 && (betNumbers[0] + 1 == betNumbers[1] || betNumbers[0] + 3 == betNumbers[1]);
    else if (betName == STREET)
        return betNumbers.size() >= 3 && (betNumbers[0] % 3 == 1 && betNumbers[1] == betNumbers[0] + 1 && betNumbers[2] == betNumbers[0] + 2);
    else if (betName == CORNER)
        return betNumbers.size() >= 4 && (betNumbers[0] + 1 == betNumbers[1] && betNumbers[0] + 3 == betNumbers[2] && betNumbers[0] + 4 == betNumbers[3]);
    else if (betName == DOUBLE_STREET)
        return betNumbers.size() >= 6 && (betNumbers[0] % 3 == 1 && betNumbers[1] == betNumbers[0] + 1 && betNumbers[2] == betNumbers[0] + 2 && betNumbers[3] % 3 == 1 && betNumbers[4] == betNumbers[3] + 1 && betNumbers[5] == betNumbers[3] + 2);
    return true;
}

/**
 * @brief Convert a string that represents a list to a list of integers
 * @param str: the string
 * @return the list of integers
 */
QList<int> Roulette::strToIntList(QString str)
{
    QList<int> result;
    for (QString element : str.split(','))
        result.append(element.toInt());
    return result;
}

