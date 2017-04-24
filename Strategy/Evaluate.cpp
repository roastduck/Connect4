#include <cassert>
#include <cstring>
#include "Const.h"
#include "Board.h"
#include "Evaluate.h"

const int Evaluate::WALK[4][2] = { { -1, 0 },{ -1, 1 },{ 0, 1 },{ 1, 1 } };

int Evaluate::evaluate(int M, int N, int lastX, int lastY, int lastColor, const Board &board)
{
    assert(board(lastX, lastY) == lastColor);
    for (int k = 0; k < 4; k++)
    {
        const int dx(WALK[k][0]), dy(WALK[k][1]);
        int step(1), l(1), r(1);
        while (board.inBoard(lastX + l * dx, lastY + l * dy) && board(lastX + l * dx, lastY + l * dy) == lastColor)
            step++, l++;
        while (board.inBoard(lastX - r * dx, lastY - r * dy) && board(lastX - r * dx, lastY - r * dy) == lastColor)
            step++, r++;
        if (step >= 4)
            return lastColor == WE ? MAX_VALUE : MIN_VALUE;
    }

    int ret(0);
    for (int i = 0; i < N; i++)
    {
        const int y(i);
        for (int x = board.getTop(i) - 1, scale = 1; scale && x >= 0; x--)
        {
            int accu(0);
            for (int k = 0; k < 4; k++)
            {
                const int dx(WALK[k][0]), dy(WALK[k][1]);
                int stepL(1), stepR(1), l(2), r(2), colorL(0), colorR(0);
                if (board.inBoard(x + dx, y + dy) && board(x + dx, y + dy))
                {
                    colorL = board(x + dx, y + dy);
                    while (board.inBoard(x + l * dx, y + l * dy) && board(x + l * dx, y + l * dy) == colorL)
                        stepL++, l++;
                }
                if (board.inBoard(x - dx, y - dy) && board(x - dx, y - dy))
                {
                    colorR = board(x - dx, y - dy);
                    while (board.inBoard(x - r * dx, y - r * dy) && board(x - r * dx, y - r * dy) == colorR)
                        stepR++, r++;
                }
                if (!colorL && !colorR)
                    continue;
                if (colorR == colorL)
                    accu += (colorL == WE ? 1 : -1) * (stepL + stepR) * (stepL + stepR) * (stepL + stepR);
                else {
                    if (colorL)
                        accu += (colorL == WE ? 1 : -1) * stepL * stepL * stepL;
                    if (colorR)
                        accu += (colorR == WE ? 1 : -1) * stepR * stepR * stepL;
                }
            }
            ret += accu * scale;
            scale *= accu / 25;
        }
    }
    assert(ret > MIN_VALUE && ret < MAX_VALUE);
    return ret;
}
