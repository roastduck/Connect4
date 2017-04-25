#ifndef SEARCH_H_
#define SEARCH_H_

#include <cstring>
#include "Const.h"

class Board;

/** Monte Carlo search tree
 */
class Search
{
private:
    int M, N;
    Board &board;

    struct Node
    {
        int value, minConf, maxConf, valFrom, timeToDie;
        Node *children[MAX_N];

        Node(int _value) :
            value(_value), minConf(_value - CONF_FLOAT), maxConf(_value + CONF_FLOAT), valFrom(-1), timeToDie(-1)
        {
            if (_value == MIN_VALUE || _value == MAX_VALUE)
                minConf = maxConf = _value, timeToDie = 0;
            memset(children, 0, sizeof(children));
        }

        ~Node()
        {
            for (int i = 0; i < MAX_N; i++)
                if (children[i])
                    delete children[i];
        }
    } *root;

    /// Add children to a leaf node
    /// @param depth : Totally add `depth` levels of nodes
    void addChildren(Node *node, int color);

    /// Extend recursively
    void extendImpl(Node *node, int color);

    /// Get update from children
    void backtrack(Node *node, int color);

public:
    Search(int _M, int _N, Board &_board) : 
        M(_M), N(_N), board(_board), root(new Node(0))
    {} // Value of root doesn't matter

    ~Search()
    {
        if (root)
            delete root;
    }

    /// Extend search tree
    void extend();

    /// Return best choice
    int best() const;

    /// Move root forward and drop useless nodes
    void moveRoot(int action, int color);
};

#endif // SEARCH_H_
