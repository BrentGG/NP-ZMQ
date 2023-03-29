#include "roulette.h"

#include <QRandomGenerator>
#include <algorithm>

Roulette::Roulette()
{
}

QString Roulette::getInfo()
{
    return QString("");
}

int Roulette::play(int bet, BetName betName, QList<int> betNumbers)
{
    return calcPayout(spin(), bet, betName, betNumbers);
}

int Roulette::spin()
{
    return QRandomGenerator::global()->bounded(0, 38); // 0-36 + 00
}

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

bool Roulette::areValidBetNumbers(BetName betName, QList<int> betNumbers)
{
    std::sort(betNumbers.begin(), betNumbers.end());
    if (betNumbers[0] < 0 || betNumbers[betNumbers.size() - 1] > 37)
        return false;
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
