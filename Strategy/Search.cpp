#include <cassert>
#include <algorithm>
#include "Board.h"
#include "Search.h"
#include "Evaluate.h"

static int myRand()
{
    static int x = 0;
    x = x * 1103515245 + 3611553;
    x &= 0x7fffffff;
    assert(x >= 0);
    return x;
}

void Search::addChildren(Node *node, int depth, int color, int alpha, int beta)
{
    assert(node->valFrom == -1);
    bool drop(false);
    for (int i = 0; i < N; i++)
        if (board.getTop(i))
        {
            int x(board.getTop(i) - 1);
            board.set(i, color);
            if (!depth || drop)
                node->children[i] = new Node(Evaluate::evaluate(M, N, x, i, color, board));
            else if (Evaluate::won(M, N, x, i, color, board))
                node->children[i] = new Node(color == WE ? MAX_VALUE : MIN_VALUE);
            else {
                node->children[i] = new Node(0); // Value doesn't matter
                addChildren(node->children[i], depth - 1, 3 - color, alpha, beta);
            }
            board.reset(i);
            assert(node->children[i]);
            if (color == WE)
            {
                if (node->children[i]->value >= beta)
                    drop = true;
                alpha = std::max(alpha, node->children[i]->value);
            } else {
                if (node->children[i]->value <= alpha)
                    drop = true;
                beta = std::min(beta, node->children[i]->value);
            }
        }
    backtrack(node, color);
}

void Search::extendImpl(Node *node, int color)
{
    if (node->minConf == MAX_VALUE || node->maxConf == MIN_VALUE)
        return;
    if (node->valFrom == -1)
    {
        addChildren(node, 2, color);
        return;
    }

    int* prob = new int[N]; // Probability of changing node->value
    memset(prob, 0, N * sizeof(int));
    for (int i = 0; i < N; i++)
        if (node->children[i])
            if (i == node->valFrom)
                prob[i] = node->children[i]->maxConf - node->children[i]->minConf;
            else if (color == WE)
                prob[i] = std::max(0, node->children[i]->maxConf - node->value);
            else
                prob[i] = std::max(0, node->value - node->children[i]->minConf);
    for (int i = 1; i < N; i++)
        prob[i] += prob[i - 1];
    assert(prob[N - 1] > 0);
    int pos(myRand() % prob[N - 1]);
    int toGo(0);
    while (prob[toGo] <= pos) toGo++;
    delete[] prob;
    board.set(toGo, color);
    extendImpl(node->children[toGo], 3 - color);
    board.reset(toGo);

    backtrack(node, color);
}

void Search::backtrack(Search::Node *node, int color)
{
    node->value = color == WE ? MIN_VALUE : MAX_VALUE;
    node->valFrom = 0, node->timeToDie = -1;
    bool tie(true);
    int maxMax = MIN_VALUE, minMax = MAX_VALUE, maxMin = MIN_VALUE, minMin = MAX_VALUE;
    for (int i = 0; i < N; i++)
        if (node->children[i])
        {
            const Node *ch = node->children[i];
            tie = false;
            int k(color == WE ? 1 : -1);
            if (ch->value * k > node->value * k)
            {
                node->value = ch->value, node->valFrom = i;
                node->timeToDie = !~ch->timeToDie ? -1 : ch->timeToDie + 1;
            }
            if (node->value == ch->value && ch->timeToDie >= 0 && ch->timeToDie + 1 > node->timeToDie)
                node->timeToDie = ch->timeToDie + 1, node->valFrom = i;
            // If ch->value tend to win, there is usually only 1 choice
            maxMax = std::max(maxMax, ch->maxConf);
            minMax = std::min(minMax, ch->maxConf);
            maxMin = std::max(maxMin, ch->minConf);
            minMin = std::min(minMin, ch->minConf);
        }
    if (tie)
    {
        node->value = node->maxConf = node->minConf = 0;
        return;
    }
    if (color == WE)
    {
        node->maxConf = (std::max(node->value, minMax) + maxMax) / 2; // Use average to approach median
        node->minConf = maxMin;
    } else {
        node->minConf = (std::min(node->value, maxMin) + minMin) / 2;
        node->maxConf = minMax;
    }
    assert(node->maxConf >= node->value);
    assert(node->value >= node->minConf);
    assert((node->value < MAX_VALUE && node->value > MIN_VALUE) ^ (node->timeToDie >= 0));
}

void Search::extend()
{
    extendImpl(root, WE);
}

int Search::best() const
{
    return root->valFrom;
}

void Search::moveRoot(int action, int color)
{
    if (!root->children[action])
    {
        assert(color == THEY);
        addChildren(root, 0, color);
    }
    Node *old = root;
    root = old->children[action];
    old->children[action] = 0;
    delete old;
}
