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

public:
    static bool won(int M, int N, int lastX, int lastY, int lastColor, const Board &board);

    static int evaluate(int M, int N, int lastX, int lastY, int lastColor, const Board &board);
};

#endif // EVALUATE_H_
