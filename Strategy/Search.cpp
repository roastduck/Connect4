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

void Search::extendImpl(Node *node, int color)
{
    if (node->minConf == MAX_VALUE || node->maxConf == MIN_VALUE)
        return;
    if (node->valFrom == -1)
    {
        for (int i = 0; i < N; i++)
            if (board.getTop(i))
            {
                int x(board.getTop(i) - 1);
                board.set(i, color);
                node->children[i] = new Node(Evaluate::evaluate(M, N, x, i, color, board));
                board.reset(i);
            }
    } else {
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
        int pos(myRand() % prob[N - 1]);
        int toGo(0);
        while (prob[toGo] <= pos) toGo++;
        delete[] prob;
        board.set(toGo, color);
        extendImpl(node->children[toGo], 3 - color);
        board.reset(toGo);
    }

    node->value = node->minConf = node->maxConf = color == WE ? MIN_VALUE : MAX_VALUE;
    bool tie(true);
    for (int i = 0; i < N; i++) // may speed up
        if (node->children[i])
        {
            tie = false;
            int k(color == WE ? 1 : -1);
            if (node->children[i]->value * k > node->value * k)
                node->value = node->children[i]->value, node->valFrom = i;
            if (node->children[i]->maxConf * k > node->maxConf * k)
                node->maxConf = node->children[i]->maxConf;
            if (node->children[i]->minConf * k > node->minConf * k)
                node->minConf = node->children[i]->minConf;
            assert(node->maxConf >= node->value);
            assert(node->value >= node->minConf);
        }
    if (tie)
        node->value = node->maxConf = node->minConf = 0;
    if (color == WE)
        node->maxConf -= (node->maxConf - node->value) / 4;
    else
        node->minConf += (node->value - node->minConf) / 4;
}

void Search::extend()
{
    extendImpl(root, WE);
}

int Search::best() const
{
    return root->valFrom;
}

void Search::moveRoot(int action)
{
    assert(root->children[action]);
    Node *old = root;
    root = old->children[action];
    old->children[action] = 0;
    delete old;
}
