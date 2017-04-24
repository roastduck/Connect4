#ifndef EVALUATE_H_
#define EVALUATE_H_

class Board;

/** Evaluate a situation
 *  This is a static class
 */
class Evaluate
{
private:
    static const int WALK[4][2];

    static int lineValue(int x, int y, int dx, int dy, int color, const Board &board);

    static int pointValue(int x, int y, const Board &board);

public:
    static bool won(int lastX, int lastY, int lastColor, const Board &board);

    static int evaluate(int M, int N, int lastX, int lastY, int lastColor, const Board &board);
};

#endif // EVALUATE_H_
