#include <cstdlib>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "Const.h"
#include "Board.h"
#include "Evaluate.h"

const int Evaluate::WALK[4][2] = { { -1, 0 },{ -1, 1 },{ 0, 1 },{ 1, 1 } };

bool Evaluate::won(int lastX, int lastY, int lastColor, const Board &board)
{
    assert(lastColor == 1 || lastColor == 2);
    assert(board(lastX, lastY) == lastColor);
    for (int k = 0; k < 4; k++)
    {
        const int dx(WALK[k][0]), dy(WALK[k][1]);
        int step(1), l(1), r(1);
        while (step < 4 && board.inBoard(lastX + l * dx, lastY + l * dy) && board(lastX + l * dx, lastY + l * dy) == lastColor)
            step++, l++;
        while (step < 4 && board.inBoard(lastX - r * dx, lastY - r * dy) && board(lastX - r * dx, lastY - r * dy) == lastColor)
            step++, r++;
        if (step >= 4)
            return true;
    }
    return false;
}

int Evaluate::lineValue(int x, int y, int dx, int dy, int color, const Board &board)
{
    assert(color == 1 || color == 2);
    int xl(x), yl(y), xr(x), yr(y), cnt(0);
    while (cnt < 3 && board.inBoard(xl - dx, yl - dy) && board(xl - dx, yl - dy) == color)
        xl -= dx, yl -= dy, cnt++;
    while (cnt < 3 && board.inBoard(xr + dx, yr + dy) && board(xr + dx, yr + dy) == color)
        xr += dx, yr += dy, cnt++;
    if (!cnt) return 0;
    int cntFree(cnt + 1);
    while (cntFree < 4 && board.inBoard(xl - dx, yl - dy) && board(xl - dx, yl - dy) != 3 - color && !board.forbid(xl - dx, yl - dy))
        xl -= dx, yl -= dy, cntFree++;
    while (cntFree < 4 && board.inBoard(xr + dx, yr + dy) && board(xr + dx, yr + dy) != 3 - color && !board.forbid(xr + dx, yr + dy))
        xr += dx, yr += dy, cntFree++;
    if (cntFree < 4) return 0;
    assert(cnt <= 3);
    return cnt * cnt * cnt;
}

int Evaluate::pointValue(int x, int y, const Board &board)
{
    int maxGain(1), maxLoss(1);
    for (int k = 0; k < 4; k++)
    {
        const int dx(WALK[k][0]), dy(WALK[k][1]);
        maxGain *= lineValue(x, y, dx, dy, WE, board) + 1;
        maxLoss *= maxLoss, lineValue(x, y, dx, dy, THEY, board) + 1;
    }
    return maxGain - maxLoss;
}

int Evaluate::evaluate(int M, int N, int lastX, int lastY, int lastColor, const Board &board)
{
    if (won(lastX, lastY, lastColor, board))
        return lastColor == WE ? MAX_VALUE : MIN_VALUE;

    int ret(0);
    for (int i = 0; i < N; i++)
    {
        int mul(1);
        for (int x = board.getTop(i) - 1; x >= 0; x--)
        {
            int pointV = pointValue(x, i, board);
            if (!pointV) break;
            ret += pointV * mul;
            mul = abs(pointV);
        }
    }
    assert(ret > MIN_VALUE && ret < MAX_VALUE);
    return ret;
}
