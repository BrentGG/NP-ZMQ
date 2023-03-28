#include "slotmachine.h"

#include <QRandomGenerator>

/**
 * @brief Play the slot machine
 * @param bet: the bet to place
 * @return the credits won, calculated as if the bet was actually paid (so a return of 0 means the bet was lost)
 */
QString SlotMachine::getInfo()
{
    return QString("\n\
>>> The Oasis: Slotmachines Info <<<\n\
The slot machines are three-reel, single-payline slot machines. This means that there are three symbols that appear when the \
slot machine is spun, and these three symbols determine the payout. The payouts are as follows (multiplied by the bet):\n\
Combination              Payout\n\
Three Bar                5000\n\
Three Cherry             1000\n\
Three Plum               200\n\
Three Watermelon         100\n\
Three Orange             50\n\
Thee Lemon               25\n\
Two Cherry (in a row)    10\n\
One Cherry               2\n\
Any other                0\n\
    ");
}

int SlotMachine::play(int bet)
{
    return calcPayout(spin(), bet);
}

/**
 * @brief Spin the slot machine
 * @return a list of three symbols, one for each reel
 */
QList<SlotMachine::Symbols> SlotMachine::spin()
{
    QList<Symbols> result;
    // Reel 1
    int rng = QRandomGenerator::global()->bounded(0, 64); // min is inclusive, max is exclusive
    if (rng < 28)
        result.append(Symbols::BLANK);
    else if (rng < 28 + 8)
        result.append(Symbols::LEMON);
    else if (rng < 28 + 8 + 7)
        result.append(Symbols::ORANGE);
    else if (rng < 28 + 8 + 7 + 6)
        result.append(Symbols::WATERMELON);
    else if (rng < 28 + 8 + 7 + 6 + 6)
        result.append(Symbols::PLUM);
    else if (rng < 28 + 8 + 7 + 6 + 6 + 5)
        result.append(Symbols::CHERRY);
    else if (rng < 28 + 8 + 7 + 6 + 6 + 5 + 4)
        result.append(Symbols::BAR);
    else // shouldn't happen, just for safety
        result.append(Symbols::BLANK);
    // Reel 2
    rng = QRandomGenerator::global()->bounded(0, 64); // min is inclusive, max is exclusive
    if (rng < 37)
        result.append(Symbols::BLANK);
    else if (rng < 37 + 6)
        result.append(Symbols::LEMON);
    else if (rng < 37 + 6 + 5)
        result.append(Symbols::ORANGE);
    else if (rng < 37 + 6 + 5 + 5)
        result.append(Symbols::WATERMELON);
    else if (rng < 37 + 6 + 5 + 5 + 4)
        result.append(Symbols::PLUM);
    else if (rng < 37 + 6 + 5 + 5 + 4 + 4)
        result.append(Symbols::CHERRY);
    else if (rng < 37 + 6 + 5 + 5 + 4 + 4 + 3)
        result.append(Symbols::BAR);
    else // shouldn't happen, just for safety
        result.append(Symbols::BLANK);
    // Reel 3
    rng = QRandomGenerator::global()->bounded(0, 64); // min is inclusive, max is exclusive
    if (rng < 42)
        result.append(Symbols::BLANK);
    else if (rng < 42 + 6)
        result.append(Symbols::LEMON);
    else if (rng < 42 + 6 + 6)
        result.append(Symbols::ORANGE);
    else if (rng < 42 + 6 + 6 + 4)
        result.append(Symbols::WATERMELON);
    else if (rng < 42 + 6 + 6 + 4 + 3)
        result.append(Symbols::PLUM);
    else if (rng < 42 + 6 + 6 + 4 + 3 + 2)
        result.append(Symbols::CHERRY);
    else if (rng < 42 + 6 + 6 + 4 + 3 + 2 + 1)
        result.append(Symbols::BAR);
    else // shouldn't happen, just for safety
        result.append(Symbols::BLANK);
    return result;
}

/**
 * @brief Calculate the payout of a certain payline
 * @param payline: a list of three symbols, one for each reel
 * @param bet: the bet that was placed
 * @return the payout
 */
int SlotMachine::calcPayout(QList<Symbols> payline, int bet)
{
    if (threeInARow(payline, Symbols::BAR))
        return 5000 * bet;
    else if (threeInARow(payline, Symbols::CHERRY))
        return 1000 * bet;
    else if (threeInARow(payline, Symbols::PLUM))
        return 200 * bet;
    else if (threeInARow(payline, Symbols::WATERMELON))
        return 100 * bet;
    else if (threeInARow(payline, Symbols::ORANGE))
        return 50 * bet;
    else if (threeInARow(payline, Symbols::LEMON))
        return 25 * bet;
    else if (twoInARow(payline, Symbols::CHERRY))
        return 10 * bet;
    else if (any(payline, Symbols::CHERRY))
        return 2 * bet;
    else
        return 0 * bet;
}

QList<QString> SlotMachine::paylineToStringList(QList<Symbols> payline)
{
    QList<QString> strList;
    for (Symbols symbol : payline) {
        if (symbol == Symbols::BAR)
            strList.append("bar");
        else if (symbol == Symbols::CHERRY)
            strList.append("cherry");
        else if (symbol == Symbols::PLUM)
            strList.append("plum");
        else if (symbol == Symbols::WATERMELON)
            strList.append("watermelon");
        else if (symbol == Symbols::ORANGE)
            strList.append("orange");
        else
            strList.append("blank");
    }
    return strList;
}

bool SlotMachine::threeInARow(QList<Symbols> payline, Symbols symbol)
{
    return payline[0] == symbol && payline[1] == symbol && payline[2] == symbol;
}

bool SlotMachine::twoInARow(QList<Symbols> payline, Symbols symbol)
{
    return (payline[0] == symbol && payline[1] == symbol) || (payline[1] == symbol && payline[2] == symbol);
}

bool SlotMachine::any(QList<Symbols> payline, Symbols symbol)
{
    return payline[0] == symbol || payline[1] == symbol || payline[2] == symbol;
}
