#ifndef _GAME_CPP_
#define _GAME_CPP_

#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include "game.h"
#include "board.h"
#include "const.h"

using namespace std;

// Default Game Constructor
Game::Game(){}

// Sets up the game
// Case 1: Player is black (moves first)
// Case 2. Player is white (moves second)
// Case 3: Player is neither (CPU vs. CPU)
void Game::Setup(int gameType){
    char temp;

    if(gameType == 1){
        user[BLACK] = true;
        user[WHITE] = false;
    }
    else if(gameType == 2){
        user[BLACK] = false;
        user[WHITE] = true;
    }
    else if(gameType == 3){
        user[BLACK] = false;
        user[WHITE] = false;
    }
    else{
        return;
    }

        cout << "Would you like to import an unfinished game? (Y if yes, otherwise type anything) ";
        cin >> temp;

    if(toupper(temp) == 'Y'){
        char c, startPosition[8][8];
        int player, i = 0, j = 0;
        string fileName;
        fstream stream;

        cout << "Enter a file name: ";
        cin >> fileName;
        stream.open(fileName.c_str(), fstream::in);

        // Make sure the path is valid
        while(stream.fail()){
            cout << "Invalid file name" << endl;
            cout << "Enter a file name: ";
            cin >> fileName;
            stream.open(fileName.c_str(), fstream::in);
        }

        // Scans file to get starting position
        while((c = stream.get()) != EOF){
            if((c == '0') || (c == '1') || (c == '2')){
                startPosition[i][j++] = c-'0';
                if(j == 8){
                    j = 0;
                    i++;
                    if(i == 8)
                        break;
                }
            }
        }

        stream >> importedPlayer;
        stream >> timeLimit;
        board = Board(startPosition, player);
        stream.close();
        if(importedPlayer == 1){
            user[WHITE] = true;
            user[BLACK] = false;
        }
        else if(importedPlayer == 2){
            user[WHITE] = false;
            user[BLACK] = true;
        }
        else{
            throw;
        }
    }
    // Assigning time limit for the AI's moves
    else{
        string t;
        timeLimit = INT_MIN;
        while(timeLimit < 0){
            cout << "AI's allotted time per move (in seconds): ";
            cin >> t;
            if(isdigit(t.c_str()[0]))
                timeLimit = atoi(t.c_str());
        }
        board = Board();
    }
}

// Code called when game is played
void Game::Play(){

    bool gameOver = false;
    cout << "Good Luck!" << endl;
    board.print();

    // Game is over if switchPlayer returns true (isGameOver is true)
    while(!gameOver){
        if(user[board.currentPlayer])
            gameOver = userMove();
        else
            gameOver = computerMove();
    }

    board.print();
    board.gameOver(user);
}

// Method to apply the user's move
bool Game::userMove(){

    int chosenMove = INT_MIN;
    char in;
    // Retrieve legal moves
    vector<Board::Move> potentialMoves = board.legalMoves(board.currentPlayer);
    if(potentialMoves.size() != 0){
        board.print(potentialMoves);
        // List potential moves for user
        for(int i = 0; i < potentialMoves.size(); i++)
            cout << (char)(i+65) << ": [" << (char)(potentialMoves[i].space.x+65) << "," << (int)(potentialMoves[i].space.y+1) << "]" << endl;

        // Asks user for move, checks if valid
        while(!(chosenMove < potentialMoves.size() && chosenMove >= 0)){
            cout << "Type the letter corresponding to your move: " << endl;
            cin >> in;
            in = toupper(in);
            chosenMove = (((int)(in))-65);
        }
        // Apply the move the user chooses
        board.applyMove(potentialMoves[chosenMove]);
        return board.switchPlayer(false); // User did not pass
    }
    else{
        cout << "You must pass (no available moves)" << endl;
        return board.switchPlayer(true); // User passed
    }
}

// Method to apply AI's move
bool Game::computerMove(){
    int depth, eval, chosenMove;
    // Limit is the number of empty squares
    int depthLimit = 64 - (board.score[BLACK] + board.score[WHITE]);
    Board::Move move;
    // Start timer
    startTime = clock();
    player = board.currentPlayer;
    // Expanding the first layer
    vector<Board::Move> legal = board.legalMoves(board.currentPlayer);
    // Pass if no available moves
    if(legal.size() == 0){
        cout << "AI had to pass (no available moves)" << endl; //AI Passed
        return board.switchPlayer(true);
    }

    // Iterate until time runs out or the depth limit is reached
    for(depth = 0; (((float)(clock()-startTime))/CLOCKS_PER_SEC < timeLimit/2.0) && depth < depthLimit; depth++){
        int alpha = INT_MIN, beta = INT_MAX, randMove = 1;
        timeout = false;
        // Iterates through legal moves
        for(int i = 0; i < legal.size(); i++){
            Board child = board;
            child.applyMove(legal[i]);
            child.switchPlayer(false);
            eval = alphabeta(child, depth, alpha, beta, false);

            // If time runs out, take the current best move
            if(timeout)
                break;
            // If new alpha is greater, use new alpha
            if(eval > alpha){
                move = legal[i];
                chosenMove = i;
                alpha = eval;
            }
            // If new alpha is the same as the previous alpha, use the newer move
            else if(eval == alpha){
                if((rand() % randMove++) == 1){
                    move = legal[i];
                    chosenMove = i;
                }
            }
        }
    }
    cout << "AI Searched to depth: " << depth << " in " << ((float)(clock()-startTime))/CLOCKS_PER_SEC << " seconds" << endl;
    cout << "AI chose move: " << chosenMove << endl;
    board.print(vector<Board::Move>(1,move), true);
    board.applyMove(move);
    return board.switchPlayer(false);
}

// Alpha beta search using MINIMAX
// Link: https://stackoverflow.com/questions/45156128/alpha-beta-pruning-pseudo-code-understanding
int Game::alphabeta(Board board, int depth, int alpha, int beta, bool player){
    int a = alpha, b = beta, legalSize;

    // Make sure time is not up
    if((((float)(clock()-startTime))/CLOCKS_PER_SEC) > TIMECUTOFF*timeLimit){
        timeout = true;
        return heuristic(board);
    }
    else if(depth == 0)
        return heuristic(board);
    else
        depth--;

    vector<Board::Move> legalMoves = board.legalMoves(board.currentPlayer);
    legalSize = legalMoves.size();
    // If no legal moves
    if(legalSize == 0){
        // Check if game is over
        if(board.isGameOver(true)){
            Board child = board;
            child.switchPlayer(false);
            return heuristic(child);
        }
        // If game is not over, continue search
        else{
            Board child = board;
            child.switchPlayer(true);
            return alphabeta(child, depth, alpha, beta, !player); // Passed false for fifth parameter, else below
        }
    }

    // Maximize
    if(player){
        int v = INT_MIN;
        for(int i = 0; i < legalSize; i++){
            Board child = board;
            child.applyMove(legalMoves[i]);
            child.switchPlayer(false);

            int eval = alphabeta(child, depth, a, b, false);
            v = MAX(v, eval);

            if(v >= beta)
                return v;
            a = MAX(a, v);
        }
        return v;
    }
    // Minimize
    else{
        int v = INT_MAX;
        for(int i = 0; i < legalSize; i++){
            Board child = board;
            child.applyMove(legalMoves[i]);
            child.switchPlayer(false);

            int eval = alphabeta(child, depth, a, b, true);
            v = MIN(v, eval);

            if(v <= a)
                return v;
            b = MIN(b, v);
        }
        return v;
    }
}

// Heuristic based on comparisons between the player and opponent on: pieces, corners, opponent's potential corners, frontiers, edges, and mobility.
// Guide to understand what these variables are is below.
// Link: http://radagast.se/othello/Help/strategy.html
int Game::heuristic(Board b){
    double pieces, cornerDiff, opponentPotential, edgeRatio, frontierDiff, mobility;
    int playerCorners=0, opponentCorners=0, playerEdges=0, opponentEdges=0, playerFrontier=0, opponentFrontier=0, opponent=0, potentialCorners=0;
    vector<Board::Move> playerLegal = board.legalMoves(player);
    vector<Board::Move> opponentLegal = board.legalMoves(opponent);

    if(player == WHITE)
        opponent = BLACK;
    else
        opponent = WHITE;

    // Incrementing corners (Corners are: [0,0], [0,7], [7,0], and [7,7])
    if(b.board[0][0] == player)
        playerCorners++;
    else if(b.board[0][0] == opponent)
        opponentCorners++;
    if(b.board[0][7] == player)
        playerCorners++;
    else if(b.board[0][7] == opponent)
        opponentCorners++;
    if(b.board[7][0] == player)
        playerCorners++;
    else if(b.board[7][0] == opponent)
        opponentCorners++;
    if(b.board[7][7] == player)
        playerCorners++;
    else if(b.board[7][7] == opponent)
        opponentCorners++;

    // Incrementing edges (Edges are anything where i or j are 0 or 7)
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(i == 0 || i == 7 || j == 0 || j == 7){
                if(b.board[i][j] == player)
                    playerEdges++;
                else if(b.board[i][j] == opponent)
                    opponentEdges++;
            }
            // Incrementing frontier pieces (Frontier pieces defined earlier, see link)
            else if(b.board[i][j] != '0' && b.onFrontier(i,j)){
                if(b.board[i][j] == player)
                    playerFrontier++;
                else if(b.board[i][j] == opponent)
                    opponentFrontier++;
            }
        }
    }

    // Incrementing opponent's potential corners (Potential corners are: [0,0], [0,7], [7,0], and [7,7])
    potentialCorners = 0;
    for(int i = 0; i < opponentLegal.size(); i++){
        if(opponentLegal[i].space.y == 0 && opponentLegal[i].space.x == 0)
            potentialCorners++;
        else if(opponentLegal[i].space.y == 0 && opponentLegal[i].space.x == (7))
            potentialCorners++;
        else if(opponentLegal[i].space.y == (7) && opponentLegal[i].space.x == 0)
            potentialCorners++;
        else if(opponentLegal[i].space.y == (7) && opponentLegal[i].space.x == (7))
            potentialCorners++;
    }

    //Definition of Parameters

    // Total pieces that belong to the player
    pieces = b.score[player];
    // Percentage of legal moves that belong to player (known as mobility)
    mobility = 100.0*playerLegal.size()/(playerLegal.size() + opponentLegal.size());
    // Percentage of edges that below to the player
    edgeRatio = 100.0*playerEdges/(playerEdges + opponentEdges);
    // Difference between player and opponent corners
    cornerDiff = playerCorners - opponentCorners;
    // Difference between player and opponent frontier pieces
    frontierDiff = playerFrontier - opponentFrontier;
    // Total opponent's potential corners
    opponentPotential = potentialCorners;

    // Coefficients of the parameters
    int pieceCO = 195;
    int cornerCO = 125000;
    int edgeCO = 55;
    int mobilityCO = 10*(100-(b.score[player]+b.score[opponent]));
    int potentialCO = 550000;
    int frontierCO = 25000;

    return (pieceCO*pieces + cornerCO*cornerDiff  + edgeCO*edgeRatio + mobilityCO*mobility) - (potentialCO*opponentPotential +  frontierCO*frontierDiff);
}

#endif //_GAME_CPP_