#include <cmath>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include "Board.h"
#include "Search.h"

int Node::M = 0, Node::N = 0;
Board *Node::board = 0;
Node *Node::root = 0;

int Node::simulateImpl(int color, int depth)
{
    if (board->won())
        return board->won();
    //if (depth > 15)
    //    return 0; // ???
    for (int i = 0; i < N; i++)
        if (board->getTop(i) && board->winning(board->getTop(i) - 1, i, color))
            return color;
    for (int i = 0; i < N; i++)
        if (board->getTop(i) && board->winning(board->getTop(i) - 1, i, 3 - color))
        {
            board->set(i, color);
            int ret = simulateImpl(3 - color, depth + 1);
            board->reset(i);
            return ret;
        }

    int cnt(0);
    for (int i = 0; i < N; i++)
        if (board->getTop(i))
            cnt++;
    if (!cnt)
        return 0;
    cnt = rand() % cnt;
    for (int i = 0; i < N; i++)
        if (board->getTop(i) && !(cnt--))
        {
            board->set(i, color);
            int ret = simulateImpl(3 - color, depth + 1);
            board->reset(i);
            return ret;
        }
    assert(false);
}

int Node::extendImpl(Node *node)
{
    node->totCnt++;
    if (board->won())
    {
        int got = board->won();
        if (got == WE) node->weWin++;
        if (got == THEY) node->theyWin++;
        return got;
    }
    int toGo(-1);
    double val(-INF);
    for (int i = 0; i < N; i++)
        if (board->getTop(i))
            if (!node->c[i])
            {
                node->c[i] = new Node(-node->k);
                node->c[i]->totCnt = 1;
                board->set(i, node->k == 1 ? WE : THEY);
                int got = simulateImpl(node->c[i]->k == 1 ? WE : THEY, 0);
                board->reset(i);
                if (got == WE) node->c[i]->weWin = 1;
                if (got == THEY) node->c[i]->theyWin = 1;
                return got;
            } else {
                int tn = node->c[i]->totCnt;
                double _val = double(node->k == 1 ? node->c[i]->weWin : node->c[i]->theyWin) / tn
                              + sqrt(2 * log(node->totCnt) / tn);
                if (_val > val)
                    val = _val, toGo = i;
            }
    if (!~toGo)
        return 0;
    board->set(toGo, node->k == 1 ? WE : THEY);
    int got = extendImpl(node->c[toGo]);
    board->reset(toGo);
    if (got == WE) node->weWin++;
    if (got == THEY) node->theyWin++;
    return got;
}

int Node::best()
{
    assert(root->k == 1);
    int ret(-1);
    double val(-INF);
    for (int i = 0; i < N; i++)
        if (root->c[i] && double(root->c[i]->weWin) / root->c[i]->totCnt > val)
            val = double(root->c[i]->weWin) / root->c[i]->totCnt, ret = i;
    assert(~ret);
    return ret;
}

void Node::moveRoot(int action)
{
    Node *old = root;
    board->set(action, root->k == 1 ? WE : THEY);
    if (root->c[action])
        root = old->c[action], old->c[action] = 0;
    else
        root = new Node(-old->k);
    delete old;
}
