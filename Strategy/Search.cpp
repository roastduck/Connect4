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
    assert(~childCnt);
    bool started(false);
    for (int i = 0; i < N; i++)
        if (c[i])
            if (!started)
                mu = c[i]->mu, sigma = c[i]->sigma, started = true;
            else {
                double _mu(mu), _sigma(sigma);
                maxDist(_mu, _sigma, c[i]->mu, c[i]->sigma, mu, sigma);
            }
    if (!started) // Tie
        mu = 0.5, sigma = 0;
}

double Node::simulateImpl(int color, int depth)
{
    if (board->won())
        return board->won() == WE ? 1 : 0;
    for (int i = 0; i < N; i++)
        if (board->getTop(i) && board->winning(board->getTop(i) - 1, i, color))
            return (color == WE ? 1 : -1) * 0.5 * exp(-0.022353 * depth) + 0.5;
    for (int i = 0; i < N; i++)
        if (board->getTop(i) && board->winning(board->getTop(i) - 1, i, 3 - color))
        {
            board->set(i, color);
            double ret = simulateImpl(3 - color, depth + 1);
            board->reset(i);
            return ret;
        }

    int cnt(0);
    for (int i = 0; i < N; i++)
        if (board->getTop(i))
            cnt++;
    if (!cnt)
        return 0.5;
    cnt = rand() % cnt;
    for (int i = 0; i < N; i++)
        if (board->getTop(i) && !(cnt--))
        {
            board->set(i, color);
            double ret = simulateImpl(3 - color, depth + 1);
            board->reset(i);
            return ret;
        }
    assert(false);
}

void Node::simulate()
{
    const int ROUND = 5;
    double *sample = new double[ROUND];
    mu = sigma = 0;
    for (int i = 0; i < ROUND; i++)
    {
        sample[i] = simulateImpl(k == 1 ? WE : THEY, 0);
        assert(sample[i] == 0 || sample[i] == 1 || sample[i] == 0.5);
        sample[i] = sample[i] * 0.98 + 0.01; // To avoid infinity
        sample[i] = log(sample[i] / (1 - sample[i])); // [0, 1] -> R
        mu += sample[i];
    }
    mu /= ROUND;
    for (int i = 0; i < ROUND; i++)
        sigma += sqr(sample[i] - mu);
    sigma = sqrt(sigma) / ROUND;
    delete[] sample;
}

void Node::extendImpl(Node *node)
{
    if (!~node->childCnt)
    {
        node->childCnt = 0;
        for (int i = 0; i < N; i++)
            if (board->getTop(i))
            {
                node->childCnt++;
                int x(board->getTop(i) - 1);
                board->set(i, node->k == 1 ? WE : THEY);
                node->c[i] = new Node(-node->k, 0, 4);
                node->c[i]->simulate();
                board->reset(i);
            }
    } else {
        int toGo(-1);
        double bound(-INF * node->k);
        for (int i = 0; i < N; i++)
            if (node->c[i] && (node->c[i]->mu + node->c[i]->sigma) * node->k > bound * node->k)
                bound = node->c[i]->mu + node->c[i]->sigma, toGo = i;
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
    assert(root->childCnt);
    int ret(-1);
    double val(-INF * root->k);
    for (int i = 0; i < N; i++)
        if (root->c[i])
        {
            if (board->winning(board->getTop(i) - 1, i, WE))
                return i;
            if (root->c[i]->mu * root->k > val * root->k)
                val = root->c[i]->mu, ret = i;
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
        root = new Node(-old->k, 0, 4);
    delete old;
}
