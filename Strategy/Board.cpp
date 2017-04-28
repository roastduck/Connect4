#include "Board.h"

const int Board::WALK[4][2] = { { -1, 0 },{ -1, 1 },{ 0, 1 },{ 1, 1 } };

bool Board::winning(int lastX, int lastY, int lastColor) const
{
    assert(lastColor == 1 || lastColor == 2);
    assert((*this)(lastX, lastY) == lastColor || !(*this)(lastX, lastY));
    for (int k = 0; k < 4; k++)
    {
        const int dx(WALK[k][0]), dy(WALK[k][1]);
        int step(1), l(1), r(1);
        while (step < 4 && inBoard(lastX + l * dx, lastY + l * dy) && (*this)(lastX + l * dx, lastY + l * dy) == lastColor)
            step++, l++;
        while (step < 4 && inBoard(lastX - r * dx, lastY - r * dy) && (*this)(lastX - r * dx, lastY - r * dy) == lastColor)
            step++, r++;
        if (step >= 4)
            return true;
    }
    return false;
}
