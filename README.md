## **Background**
This is an intelligent game-playing AI to play a human in Othello.
The user can specify a time limit for each AI move.
The AI utilizes alpha-beta pruning, A* searching, and modified heuristics.
The AI can also play itself and the user can import a previous unfinished game.

### **How to play the AI** 
Once the folder is open in terminal:
```
g++ -o main board.cpp main.cpp game.cpp

./main
```

The user is then prompted to pick between three game modes.

Here is a sample move when the user decides to play second.

*Green = white pieces* | *Blue = black pieces* | *Black = empty squares* | *Red = CPU's move* | *Purple = User's potential moves*

[![example.png](https://i.postimg.cc/V6HwnZT4/example.png)](https://postimg.cc/jwNBrcRJ)
