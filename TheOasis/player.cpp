#include "player.h"

Player::Player(QString name, int credits) : name(name), credits(credits)
{

}

QString Player::getName() const
{
    return name;
}

int Player::getCredits() const
{
    return credits;
}

long long Player::getOnlineSince() const
{
    return onlineSince;
}

void Player::setOnlineSince(long long newOnlineSince)
{
    onlineSince = newOnlineSince;
}
