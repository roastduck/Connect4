#include <cassert>
#include <cstdlib>
#include <algorithm>
#include "Board.h"
#include "Search.h"
#include "Evaluate.h"

void Search::addChildren(Node *node, int color)
{
    assert(node->valFrom == -1);
    for (int i = 0; i < N; i++)
        if (board.getTop(i))
        {
            int x(board.getTop(i) - 1);
            board.set(i, color);
            node->children[i] = new Node(Evaluate::evaluate(M, N, x, i, color, board));
            board.reset(i);
        }
    backtrack(node, color);
}

void Search::extendImpl(Node *node, int color)
{
    if (node->minConf == MAX_VALUE || node->maxConf == MIN_VALUE)
        return;
    if (node->valFrom == -1)
    {
        addChildren(node, color);
        return;
    }

    double* prob = new double[N]; // Probability of changing node->value
    memset(prob, 0, N * sizeof(double));
    for (int i = 0; i < N; i++)
        if (node->children[i])
        {
            int len = node->children[i]->maxConf - node->children[i]->minConf;
            if (!len) continue;
            if (i == node->valFrom)
                prob[i] = 1;
            else if (color == WE)
                prob[i] = double(std::max(0, node->children[i]->maxConf - node->value)) / len;
            else
                prob[i] = double(std::max(0, node->value - node->children[i]->minConf)) / len;
        }
    for (int i = 1; i < N; i++)
        prob[i] += prob[i - 1];
    assert(prob[N - 1] >= 0);
    if (!prob[N - 1]) return;
    double pos(double(rand()) / RAND_MAX * prob[N - 1]);
    int toGo(0);
    while (prob[toGo] <= pos && toGo < N - 1) toGo++;
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
            if (node->value == ch->value)
            {
                if (color == WE && node->value == MIN_VALUE || color == THEY && node->value == MAX_VALUE)
                {
                    assert(ch->timeToDie >= 0);
                    if (ch->timeToDie + 1 > node->timeToDie)
                        node->timeToDie = ch->timeToDie + 1, node->valFrom = i;
                }
                if (color == WE && node->value == MAX_VALUE || color == THEY && node->value == MIN_VALUE)
                {
                    assert(ch->timeToDie >= 0);
                    if (ch->timeToDie + 1 < node->timeToDie)
                        node->timeToDie = ch->timeToDie + 1, node->valFrom = i;
                }
            }
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
    Node *old = root;
    if (root->children[action])
        root = old->children[action], old->children[action] = 0;
    else
        root = new Node(0);
    delete old;
    board.set(action, color); // Should be after addChildren
}
