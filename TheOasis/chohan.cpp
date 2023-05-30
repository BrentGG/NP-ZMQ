#include "chohan.h"
#include "failedrequest.h"

#include <QRandomGenerator>

ChoHan::ChoHan()
{

}

/**
 * @brief Return a string containing info about the Cho-Han game
 * @return the string
 */
QString ChoHan::getInfo()
{
    return QString("theoasis>info!>cho-han>\n\
>>> The Oasis: Cho-Han Info <<<\n\n\
Cho-Han is a simple Japanese gambling game played with two standard six-sided dice. Before rolling the dice, call out 'even' or 'odd'. \
If the sum of the dice is even and you called even or the sum is odd and you called odd, you win. Otherwise, you lose. \
Below are the payouts (multiplied by the bet).\n\
Situation     Payout\n\
Win           2\n\
Loss          0\n\
    ");
}

/**
 * @brief Handle a request to play Cho-Han
 * @param player: the player making the request
 * @param request: the reques, separated by '>'
 * @return the response
 */
QString ChoHan::handleRequest(Player *player, QList<QString> request)
{
    if (request.size() >= 5) {
        // Checks
        bool isNumber = false;
        int bet = request[3].toInt(&isNumber);
        if (!isNumber || bet <= 0 || bet > player->getCredits())
            throw FailedRequest(QString("theoasis>cho-han!>" + request[2] + ">false>Invalid bet.>"));
        bool even = true;
        if (request[4].toLower().compare("even") == 0)
            even = true;
        else if (request[4].toLower().compare("odd") == 0)
            even = false;
        else
            throw FailedRequest(QString("theoasis>cho-han!>" + request[2] + ">false>Invalid choice, choose 'even' or 'odd'.>"));

        // Play Cho-Han
        player->modifyCredits(-bet);
        QList<int> dice = rollDice();
        int payout = calcPayout(dice, bet, even);
        player->modifyCredits(payout);

        // Make response
        QString response = QString("theoasis>cho-han!>" + request[2] + ">true>");
        for (int i = 0; i < dice.size(); ++i) {
            response.append(QString::number(dice[i]));
            if (i < dice.size())
                response.append(",");
        }
        response.append(">" + QString::number(payout) + ">" + (payout > 0 ? "Congratulations, you won!>" : "You lost!>"));
        return response;
    }
    else
        throw FailedRequest(QString("theoasis>cho-han!>" + request[2] + ">false>Bad request.>"));
}

/**
 * @brief Roll the Cho-Han dice
 * @return a list containing the result of the dice
 */
QList<int> ChoHan::rollDice()
{
    QList<int> dice;
    for (int i = 0; i < 2; ++i)
        dice.append(QRandomGenerator::global()->bounded(1, 7));
    return dice;
}

/**
 * @brief Calculate the payout given the dice, the bet and whether the player bet on even or odd
 * @param dice: the dice that were rolled
 * @param bet: the bet amount
 * @param even: true if the player bet on even
 * @return the payout
 */
int ChoHan::calcPayout(QList<int> dice, int bet, bool even)
{
    int sum = 0;
    for (int i = 0; i < dice.size(); ++i)
        sum += dice[i];
    if (sum % 2 == 0 && even)
        return bet * 2;
    return 0;
}


