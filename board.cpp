#ifndef _BOARD_CPP_
#define _BOARD_CPP_

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include "const.h"
#include "board.h"

using namespace std;

//Variables for traversals
int angles[4] = {1, 2, 3, 4}; // 1: E | 2: NE | 3: N | 4: NW
int directions[2] = {-1, 1}; // -1: Backwards | 1: Forwards

// Board Constructor to give initial board and constraints
Board::Board(){
    //Constraints
    currentPlayer = BLACK;
    previousPass = false;
    score[BLACK] = 2;
    score[WHITE] = 2;

    //Setting initial board
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if((i==3 && j==3)||(i==4 && j==4))
                board[i][j] = WHITE;
            else if((i==3 && j==4) || (i==4 && j==3))
                board[i][j] = BLACK;
            else
                board[i][j] = 0;
        }
    }
}

// Copy of Board Constructor
Board::Board(Board &b){
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            this->board[i][j] = b.board[i][j];
    for(int i = 0; i < 3; i++)
        this->score[i] = b.score[i];
    //Copy of constraints
    this->currentPlayer = b.currentPlayer;
    this->previousPass = b.previousPass;
}

// Board Constructor to initialize board and scores with inputted board state
Board::Board(char startPosition[8][8], int currentPlayer){
    score[BLACK] = 0;
    score[WHITE] = 0;

    //Copies board and increments respective scores
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            board[i][j] = startPosition[i][j];
            if(startPosition[i][j] == WHITE) // Increments white score for each white piece
                score[WHITE]++;
            else if(startPosition[i][j] == BLACK)// Increments black score for each black piece
                score[BLACK]++;
        }
    }
    //Copy of constraints
    this->currentPlayer = currentPlayer;
    previousPass = false;
}

// Default Space Constructor
Board::Space::Space(){}

// Alternate Space Constructor
Board::Space::Space(char y, char x){
    this->y = y;
    this->x = x;
}

// Default Move constructor
Board::Move::Move(){}

// Alternate Move constructor
Board::Move::Move(char y, char x){
    this->space.y = y;
    this->space.x = x;
    valid = false;
}

// Method to switch turns
bool Board::switchPlayer(bool currentPass){
    // Check if game is over
    if(isGameOver(currentPass))
        return true;
    // If the game is not over, switch the current player
    if(currentPlayer == WHITE)
        currentPlayer = BLACK;
    else
        currentPlayer = WHITE;
    // If the current player passed, once turns switch, current player becomes previous player
    previousPass = currentPass;

    return false;
}

//  Method to traverse through the board
bool Board::traverse(char &y, char &x, const int angle, const int direction){

    if((direction != 1) && (direction != -1))
        return false;

    if(angle == 1){
        x += direction;
    }
    else if(angle == 2){
        y += direction;
        x += direction;
    }
    else if(angle == 3){
        y += direction;
    }
    else if(angle == 4){
        y += direction;
        x -= direction;
    }
    return true;
}

// Ensure valid space
bool Board::onBoard(const char y, const char x){
    return (x >= 0) && (x < 8) && (y >= 0) && (y < 8);
}

// Method to determine if a piece is a frontier piece
// A frontier piece is a piece that is adjacent to an empty space
// Link: http://radagast.se/othello/Help/strategy.html
bool Board::onFrontier(int y, int x){
    bool frontier = false;
    //If it is an empty space it is not on the frontier
    if(board[y][x] == '0')
        return frontier;
    //Traverse space's neighbors
    for(int n = 0; n < 4; n++){
        int angle = angles[n];
        for(int m = 0; m < 2; m++){
            int direction = directions[m];
            char Y = y, X = x;
            traverse(Y, X, angle, direction);
            if(onBoard(Y, X))
                if(board[Y][X] == '0') // If it has an empty space neighbor, it is on a frontier
                    frontier = true;
        }
    }
    return frontier;
}

// Method to find all legal moves
vector<Board::Move> Board::legalMoves(int player){
    vector<Board::Move> moves;


    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            // Must be an empty space
            if(board[i][j] == 0){
                Board::Move move = Board::Move(i, j);
                for(int n = 0; n < 4; n++){
                    int angle = angles[n];
                    for(int m = 0; m < 2; m++){
                        int direction = directions[m];
                        char y = move.space.y, x = move.space.x;
                        vector<Board::Space> trace;
                        traverse(y, x, angle, direction);
                        // Check if neighbor is opponent's piece
                        if((board[y][x] != 0) && (board[y][x] != player)){
                            while(onBoard(y, x)){
                                // If piece is user's, then it is a valid move
                                if(board[y][x] == player){
                                    move.valid = true;
                                    move.flips.insert(move.flips.end(), trace.begin(), trace.end());
                                    break;
                                }
                                else if(board[y][x] == 0)
                                    break;
                                    // Keep track of potential flips
                                else{
                                    trace.push_back(Board::Space(y, x));
                                }
                                traverse(y, x, angle, direction);
                            }
                        }
                    }
                }
                if(move.valid)
                    moves.push_back(move);
            }
        }
    }
    return moves;
}

//  Method to apply a move
void Board::applyMove(Board::Move move){
    //Increment score for piece just placed
    board[move.space.y][move.space.x] = currentPlayer;
    if(currentPlayer == WHITE)
        score[WHITE]++;
    else
        score[BLACK]++;

    //Flipping pieces and changing score
    for(int i = 0; i < move.flips.size(); i++){
        board[move.flips[i].y][move.flips[i].x] = currentPlayer;
        if(currentPlayer == WHITE){
            score[WHITE]++;
            score[BLACK]--;
        }
        else{
            score[BLACK]++;
            score[WHITE]--;
        }
    }
}

// Method to print the board
// Magenta - Possible Moves | Red - Computer's Move | White pieces - Green | Black pieces - Blue
// potentialMove - Options for the user's next move | computersMove - Is it the AI's turn
void Board::print(vector<Board::Move> moves, bool computersMove){

    cout << BOLDBLACK << "    A  B  C  D  E  F  G  H" << endl;
    cout << BOLDBLACK << "   ------------------------" << RESET << endl;

    for(int i = 0; i < 8; i++){
        cout << BOLDBLACK << (int)(i+1) << " |" << RESET;
        for(int j = 0; j < 8; j++){
            bool potentialMove = false;
            for(int k = 0; k < moves.size(); k++){
                if(moves[k].space.y == i && moves[k].space.x == j){
                    if(computersMove)
                        cout << BOLDRED << setw(2) << (int)board[i][j] << RESET << " ";
                    else{
                        cout << BOLDMAGENTA << setw(2) << char(k+65) << RESET << " ";
                    }
                    potentialMove = true;
                }
            }

            if(!potentialMove){
                if(board[i][j] == WHITE)
                    cout << BOLDGREEN;
                else if(board[i][j] == BLACK)
                    cout << BOLDBLUE;
                cout << setw(2) << (int)board[i][j] << RESET << " ";
            }
        }
        cout << BOLDBLACK << "|" << RESET << endl;
    }
    cout << BOLDBLACK << "   ------------------------" << RESET << endl;
}

// Method to determine if the game is over (Both pass || Board full)
bool Board::isGameOver(bool currentPass){
    return (previousPass && currentPass) || (score[BLACK]+score[WHITE] == 64);
}

// Method for when game is over
void Board::gameOver(bool whoWon[3]){
    cout << "Game Over!" << endl;
    cout << "What a good game! The final score was... White: " << BOLDGREEN << score[WHITE] << RESET << " | Black: " << BOLDBLUE << score[BLACK] << RESET << endl;
    if((score[WHITE] > score[BLACK]) && (whoWon[BLACK]))
        cout << "You lost as black!" << endl;
    else if((score[WHITE] > score[BLACK]) && (whoWon[WHITE]))
        cout << "You won as white!" << endl;
    else if((score[WHITE] < score[BLACK]) && (whoWon[BLACK]))
        cout << "You won as black!" << endl;
    else if((score[WHITE] < score[BLACK]) && (whoWon[WHITE]))
        cout << "You lost as white!" << endl;
    else
        cout << "It's a tie game! That's all folks!" << endl;
}
#endif //_BOARD_CPP_