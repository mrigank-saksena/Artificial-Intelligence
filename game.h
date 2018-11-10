#ifndef _GAME_H_
#define _GAME_H_

#include <time.h>
#include "board.h"
#include "const.h"

using namespace std;


class Game{
public:
    Game();
    void Setup(int gameType);
    void Play();

private:
    int heuristic(Board board);
    int alphabeta(Board board, int depth, int alpha, int beta, bool maxPlayer);
    bool computerMove();
    bool userMove();
    bool user[3];
    Board board;
    int player;
    int importedPlayer;
    int timeLimit;
    clock_t startTime;
    bool timeout;
};

#endif //_GAME_H_