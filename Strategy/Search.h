#ifndef SEARCH_H_
#define SEARCH_H_

#include <cstring>
#include "Const.h"

class Board;

/** Nodes of Monte Carlo search tree
*/
class Node
{
private:
    static int M, N;
    static Board *board;
    static Node *root;

    int k; /// WE => 1, THEY => -1
    int childCnt;;
    double mu, sigma;
    Node *c[MAX_N]; /// Children;

public:
    Node(int color, double _mu, double _sigma);
    ~Node();

    /// Initialize static variables
    static void init(int _M, int _N, Board *_board);

    /// Extend search tree
    static void extend();

    /// Return best choice
    static int best();

    /// Move root forward and drop useless nodes
    static void moveRoot(int action);

private:
    /// Calculate mu, sigma of 2 normal distribution maximum
    void maxDist(double mu1, double sigma1, double mu2, double sigma2, double &mu, double &sigma);

    /// Get update from children
    void backtrack();

    /// Simulate recursively
    static double simulateImpl(int color, int depth);

    /// Simulate multiple times
    void simulate();

    /// Extend recursively
    static void extendImpl(Node *node);
};

inline Node::Node(int _k, double _mu, double _sigma)
    : k(_k), childCnt(-1), mu(_mu), sigma(_sigma)
{
    assert(k == 1 || k == -1);
    memset(c, 0, sizeof c);
}

inline Node::~Node()
{
    for (int i = 0; i < MAX_N; i++)
        if (c[i])
            delete c[i];
}

inline void Node::init(int _M, int _N, Board *_board)
{
    // `board` is deleted in Strategy.cpp
    M = _M, N = _N, board = _board;
    if (root)
        delete root;
    root = new Node(1, 0, 4);
}

inline void Node::extend()
{
    extendImpl(root);
}

#endif // SEARCH_H_
