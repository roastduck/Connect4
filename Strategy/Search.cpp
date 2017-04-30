#include <cmath>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include "Board.h"
#include "Search.h"

int Node::M = 0, Node::N = 0;
Board *Node::board = 0;
Node *Node::root = 0;

template <class T> inline static T sqr(T x) { return x * x; }

void Node::maxDist(double mu1, double sigma1, double mu2, double sigma2, double &mu, double &sigma)
{
    // A: (X + Y) / 2, B: (X - Y) /2
    double muA = (mu1 + mu2) * 0.5;
    double muB = (mu1 - mu2) * 0.5;
    double sigma2AB = (sqr(sigma1) + sqr(sigma2)) * 0.25;
    double sigmaAB = sqrt(sigma2AB);

    // C: |B|
    double muC, sigma2C;
    if (sigmaAB > 1e-5)
    {
        double reduced = fabs(muB / sigmaAB);
        muC = (reduced < 1.25204 ? 0.79788 + 0.28972 * sqr(muB) / sigma2AB : reduced) * sigmaAB;
        sigma2C = sqr(muB) + sigma2AB - sqr(muC);
        if (sigma2C < 0)
            sigma2C = 0, muC = sqrt(sqr(muB) + sigma2AB);
    } else
        muC = muB, sigma2C = sigma2AB;

    // return = A + C or A - C
    mu = muA + muC * k;
    sigma = sqrt(sigma2AB + sigma2C);
    assert(!isnan(mu));
    assert(!isnan(sigma));
}

void Node::backtrack()
{
    assert(hasChild);
    assert(!board->won());
    totCnt = weWin = theyWin = 0;
    for (int i = 0; i < N; i++)
        if (c[i])
        {
            weWin += c[i]->weWin;
            theyWin += c[i]->theyWin;
            totCnt += c[i]->totCnt;
        }
}

int Node::simulateImpl(int color, int depth)
{
    if (board->won())
        return board->won();
    if (depth > 10)
        return 0;
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

void Node::simulate()
{
    int res = simulateImpl(k == 1 ? WE : THEY, 0);
    weWin = theyWin = 0;
    totCnt = 1;
    if (res == WE)
        weWin = 1;
    if (res = THEY)
        theyWin = 1;
}

void Node::extendImpl(Node *node)
{
    if (board->won())
    {
        node->totCnt++;
        (board->won() == WE ? node->weWin : node->theyWin)++;
        return;
    } // !!!
    if (!node->hasChild)
    {
        node->hasChild = true;
        for (int i = 0; i < N; i++)
            if (board->getTop(i))
            {
                board->set(i, node->k == 1 ? WE : THEY);
                node->c[i] = new Node(-node->k);
                node->c[i]->simulate();
                board->reset(i);
            }
    } else {
        int toGo(-1);
        double bound(-INF);
        for (int i = 0; i < N; i++)
            if (node->c[i])
            {
                int tn = node->c[i]->totCnt;
                double _bound = (node->k == 1 ? node->c[i]->weWin : node->c[i]->theyWin) / tn + sqrt(2 * log(node->totCnt) / tn);
                if (_bound> bound)
                    bound = _bound, toGo = i;
            }
        if (!~toGo)
            return;
        board->set(toGo, node->k == 1 ? WE : THEY);
        extendImpl(node->c[toGo]);
        board->reset(toGo);
    }
    node->backtrack();
}

int Node::best()
{
    assert(root->hasChild);
    assert(root->k == 1);
    int ret(-1);
    double val(-INF * root->k);
    for (int i = 0; i < N; i++)
        if (root->c[i])
        {
            if (board->winning(board->getTop(i) - 1, i, WE))
                return i;
            if (double(root->c[i]->weWin) / root->c[i]->totCnt > val)
                val = double(root->c[i]->weWin) / root->c[i]->totCnt, ret = i;
        }
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
