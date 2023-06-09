#include "slotmachine.h"
#include "failedrequest.h"

#include <QRandomGenerator>

/**
 * @brief Play the slot machine
 * @param bet: the bet to place
 */
QString SlotMachine::getInfo()
{
    return QString("theoasis>info!>slotmachine>\n\
>>> The Oasis: Slotmachines Info <<<\n\n\
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

/**
 * @brief Handle a slot machine request
 * @param player: the player making the request
 * @param request: the request
 * @return the response
 */
QString SlotMachine::handleRequest(Player* player, QList<QString> request)
{
    if (request.size() >= 4) {
        // Checks
        bool isNumber = false;
        int bet = request[3].toInt(&isNumber);
        if (isNumber && bet > 0 && bet <= player->getCredits()) {
            // Play roulette
            player->modifyCredits(-1 * bet);
            QList<SlotMachine::Symbols> payline = SlotMachine::spin();
            int payout = SlotMachine::calcPayout(payline, bet);
            player->modifyCredits(payout);

            // Make response
            QString response = QString("theoasis>slotmachine!>" + request[2] + ">true>");
            QList<QString> paylineStr = SlotMachine::paylineToStringList(payline);
            for (int i = 0; i < paylineStr.size(); ++i)
                response.append(paylineStr[i] + (i < paylineStr.size() - 1 ? "," : ">"));
            response.append(QString::number(payout) + ">" + (payout != 0 ? "Congratulations, you won!>" : "You lost!>"));
            return response;
        }
        else
            throw FailedRequest(QString("theoasis>slotmachine!>" + request[2] + ">false>Invalid bet.>"));
    }
    throw FailedRequest(QString("theoasis>slotmachine!>" + request[2] + ">false>Bad request.>"));
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

/**
 * @brief Convert the payline, which is a list of enums, to a list of strings, so it can be sent
 * @param payline: the payline list
 * @return The string list
 */
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

/**
 * @brief Check whether the payline has three in a row of a given symbol
 * @param payline: the payline
 * @param symbol: the symbol
 * @return True if the payline has three of the symbol in a row
 */
bool SlotMachine::threeInARow(QList<Symbols> payline, Symbols symbol)
{
    return payline[0] == symbol && payline[1] == symbol && payline[2] == symbol;
}

/**
 * @brief Check whether the payline has two in a row of a given symbol
 * @param payline: the payline
 * @param symbol: the symbol
 * @return True if the payline has two of the symbol in a row
 */
bool SlotMachine::twoInARow(QList<Symbols> payline, Symbols symbol)
{
    return (payline[0] == symbol && payline[1] == symbol) || (payline[1] == symbol && payline[2] == symbol);
}

/**
 * @brief Check if the payline contains the symbol
 * @param payline: the payline
 * @param symbol: the symbol
 * @return True if the payline contains the symbol
 */
bool SlotMachine::any(QList<Symbols> payline, Symbols symbol)
{
    return payline[0] == symbol || payline[1] == symbol || payline[2] == symbol;
}
