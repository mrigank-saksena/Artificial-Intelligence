#ifndef _BOARD_H_
#define _BOARD_H_

#include <vector>
#include "const.h"

using namespace std;

class Board{
public:

    int currentPlayer;
    int score[3];
    char board[8][8];
    bool previousPass;

    Board();
    Board(Board &b);
    Board(char startPosition[8][8], int currentPlayer);

    class Space{
    public:
        Space();
        Space(char y, char x);
        char y, x;
    };

    class Move{
    public:
        Move();
        Move(char y, char x);
        Board::Space space;
        bool valid;
        vector<Space> flips;
    };

    bool onFrontier(int y, int x);
    bool switchPlayer(bool currentPass);
    void applyMove(Board::Move move);
    vector<Board::Move> legalMoves(int player);
    void print(vector<Board::Move> moves = vector<Board::Move>(), bool computersMove = false);
    bool isGameOver(bool currentPass);
    void gameOver(bool whoWon[3]);

private:
    bool onBoard(const char y, const char x);
    bool traverse(char &y, char &x, const int mode, const int direction);
};

#endif //_BOARD_H_