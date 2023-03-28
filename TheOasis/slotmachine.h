#ifndef SLOTMACHINE_H
#define SLOTMACHINE_H

#include <QList>

/**
 * @brief A simple 3-reel, single-payline slot machine
 */
class SlotMachine
{
public:
    SlotMachine();

    enum Symbols {
        BAR,
        CHERRY,
        PLUM,
        WATERMELON,
        ORANGE,
        LEMON,
        BLANK
    };

    QList<Symbols> spin();

private:

};

#endif // SLOTMACHINE_H
