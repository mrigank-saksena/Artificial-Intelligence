#ifndef _CONST_H_
#define _CONST_H_

#define MAX(a,b) (a > b) ? a : b
#define MIN(a,b) (a < b) ? a : b


#define WHITE 1
#define BLACK 2
#define TIMECUTOFF 0.95

//Colors taken from https://gist.github.com/RobinMalfait/7881398
#define RESET   "\033[0m"
#define BOLDBLACK   "\033[1m\033[30m"      /* Used for coordinates */
#define BOLDRED     "\033[1m\033[31m"      /* Used for computer's move */
#define BOLDGREEN   "\033[1m\033[32m"      /* Used for white */
#define BOLDBLUE    "\033[1m\033[34m"      /* Used for black */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Used for potential moves */




#endif