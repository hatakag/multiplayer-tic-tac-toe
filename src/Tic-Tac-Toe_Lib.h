#ifndef TIC_TAC_TOE_LIB_H
#define TIC_TAC_TOE_LIB_H

#include <stdio.h>
#define STRING_SIZE 50

// print board
void board(char square[][3]);

// checkWin return 1 if one wins, return -1 if draw, return if unfinish
int checkWin(char square[][3]);

// write player steps to file
void writeToFile(FILE *fp, char square[][3]);

// read player steps from file
void readFromFile(FILE * fp);

// enter player name with number of players
void enterPlayerName(int num_players, char playerName[][STRING_SIZE]);

// check mark position return true if right, return false if wrong
bool checkMarkPosition(int x, int y);

// check position is marked return true if it is marked, return false if not
bool isMark(char square[][3], int x, int y);

// play game, print winner and save steps to file
void playGame(char playerName[][STRING_SIZE], FILE *savefile);

#endif