#include <iostream>
#include "game.h"

using namespace std;


int main(int argc, char *argv[]){
    char mode;
    Game game;

    cout << "Pick an option:" << endl
         << "1: Play black (move first)" << endl
         << "2. Play white (move second)" << endl
         << "3: Play neither (AI vs. AI)" << endl;


    for( ; ; ){
        cin >> mode;
        if((mode == '1') || (mode == '2') || (mode == '3')){
            game.Setup(mode -'0');
            break;
        }
    }

    game.Play();
    return 0;
}