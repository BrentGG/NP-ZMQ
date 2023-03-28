#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QList>


class Player
{
public:
    Player(QString name, int credits = 1000);

    QString getName() const;
    int getCredits() const;
    long long getOnlineSince() const;

    void setOnlineSince(long long newOnlineSince);

    void modifyCredits(int amount);

private:
    QString name;
    int credits;

    long long onlineSince;
};

#endif // PLAYER_H
