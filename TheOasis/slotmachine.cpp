#include "slotmachine.h"

#include <QRandomGenerator>

/**
 * @brief Play the slot machine
 * @param bet: the bet to place
 * @return the credits won, calculated as if the bet was actually paid (so a return of 0 means the bet was lost)
 */
int SlotMachine::play(int bet)
{
    return calcPayout(spin()) * bet;
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
 * @return the payout
 */
int SlotMachine::calcPayout(QList<Symbols> payline)
{
    if (threeInARow(payline, Symbols::BAR))
        return 5000;
    else if (threeInARow(payline, Symbols::CHERRY))
        return 1000;
    else if (threeInARow(payline, Symbols::PLUM))
        return 200;
    else if (threeInARow(payline, Symbols::WATERMELON))
        return 100;
    else if (threeInARow(payline, Symbols::ORANGE))
        return 50;
    else if (threeInARow(payline, Symbols::LEMON))
        return 25;
    else if (twoInARow(payline, Symbols::CHERRY))
        return 10;
    else if (any(payline, Symbols::CHERRY))
        return 2;
    else
        return 0;
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
