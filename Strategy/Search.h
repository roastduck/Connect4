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
    int totCnt, weWin, theyWin;
    Node *c[MAX_N]; /// Children;

public:
    Node(int _k);
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
    /// Simulate recursively
    static int simulateImpl(int color, int depth);

    /// Extend recursively
    static int extendImpl(Node *node);
};

inline Node::Node(int _k)
    : k(_k), totCnt(0), weWin(0), theyWin(0)
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
    root = new Node(1);
}

inline void Node::extend()
{
    extendImpl(root);
}

#endif // SEARCH_H_
