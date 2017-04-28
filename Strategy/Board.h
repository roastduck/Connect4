#ifndef BOARD_H_
#define BOARD_H_

#include <cstring>
#include <cassert>

/** Chess Board
 *  It requires to set/reset chess in order to maintain top array
 */
class Board
{
private:
    int M, N, noX, noY, *data, *top, winX, winY;

    static const int WALK[4][2];

public:
    Board(int _M, int _N, int _noX, int _noY, const int *_data, const int *_top)
        : M(_M), N(_N), noX(_noX), noY(_noY), data(new int[M * N]), top(new int[N]), winX(-1), winY(-1)
    {
        memcpy(data, _data, M * N * sizeof(int));
        memcpy(top, _top, N * sizeof(int));
    }
    ~Board() { delete[] data; delete[] top; }

    Board(const Board &) = delete;
    Board &operator=(const Board &) = delete;

    bool inBoard(int x, int y) const { return x >= 0 && x < M && y >= 0 && y < N;  }
    bool forbid(int x, int y) const { return x == noX && y == noY; }

    int operator()(int x, int y) const;
    int getTop(int y) const;
    void set(int y, int color);
    void reset(int y);

    /// Detect vitory when `lastColor` being putting to (lastX, lastY)
    bool winning(int lastX, int lastY, int lastColor) const;

    /// Already won?
    /// @return : winning side or 0
    int won() const;
};

inline int Board::operator()(int x, int y) const
{
    assert(inBoard(x, y));
    return data[x * N + y];
}

inline int Board::getTop(int y) const
{
    assert(y >= 0 && y < N);
    return top[y];
}

inline void Board::set(int y, int color)
{
    top[y] --;
    assert(!data[top[y] * N + y] && (color == 1 || color == 2));
    data[top[y] * N + y] = color;
    if (!won() && winning(top[y], y, color))
        winX = top[y], winY = y;
    top[y] -= forbid(top[y] - 1, y);
}

inline void Board::reset(int y)
{
    top[y] += forbid(top[y], y);
    assert(data[top[y] * N + y]);
    data[top[y] * N + y] = 0;
    if (top[y] == winX && y == winY)
        winX = winY = -1;
    top[y] ++;
}

inline int Board::won() const
{
    return ~winX ? data[winX * N + winY] : 0;
}

#endif // BOARD_H_
