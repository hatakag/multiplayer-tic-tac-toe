#include <stdio.h>
#include "Tic-Tac-Toe_Lib.h"

#define STRING_SIZE 50

// print board
void board(char square[][3])
{
  //system("cls");

  printf("     |     |     \n");
  printf("  %c  |  %c  |  %c  \n", square[0][0], square[0][1], square[0][2]);
  printf("_____|_____|_____\n");
  printf("     |     |     \n");
  printf("  %c  |  %c  |  %c  \n", square[1][0], square[1][1], square[1][2]);
  printf("_____|_____|_____\n");
  printf("     |     |     \n");
  printf("  %c  |  %c  |  %c  \n", square[2][0], square[2][1], square[2][2]);
  printf("     |     |     \n");
}

// checkWin return 1 if X wins, 2 if O wins, return -1 if draw, return if unfinish
int checkWin(char square[][3]) {
  int i;
  for (i = 0; i < 3; i++) {
    if (square[i][0] != ' ' && square[i][0] == square[i][1] && square[i][0] == square[i][2]) {
      if (square[i][0] == 'X')
	return 1;
      else return 2;
    }    
    if (square[0][i] != ' ' && square[0][i] == square[1][i] && square[0][i] == square[2][i]) {
      if (square[i][0] == 'X')
	return 1;
      else return 2;
    }
  }
  if (square[0][0] != ' ' && square[0][0] == square[1][1] && square[0][0] == square[2][2]) {
    if (square[0][0] == 'X')
      return 1;
    else return 2;
  }
  if (square[2][0] != ' ' && square[2][0] == square[1][1] && square[2][0] == square[0][2]) {
    if (square[2][0] == 'X')
      return 1;
    else return 2;
  }
  int isDraw = 1;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (square[i][j] == ' ') isDraw = 0;
    }
  }
  if (isDraw == 1) return -1;
  else return 0;
}

// write player steps to file
void writeToFile(FILE *fp, char square[][3]) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (square[i][j] == ' ') fprintf(fp, "T ");
      else fprintf(fp, "%c ", square[i][j]);
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
}

// read player steps from file
void readFromFile(FILE * fp) {
  char c;
  while (!feof(fp)) {
    fscanf(fp, "%c", &c);
    printf("%c", c);
  }
}

// enter player name with number of players
void enterPlayerName(int num_players, char playerName[][STRING_SIZE]) {
  for (int i = 0; i < num_players; i++) {
    printf("Enter Player %d Name: ", i + 1);
    scanf("%s", playerName[i]);
    while ((getchar()) != '\n');
  }
}

// check mark position return true if right, return false if wrong
bool checkMarkPosition(int x, int y) {
  if (x >= 0 && x <= 2 && y >= 0 && y <= 2) return true;
  else return false;
}

// check position is marked return true if it is marked, return false if not
bool isMark(char square[][3], int x, int y) {
  if (square[x][y] == ' ') return false;
  else return true;
}

// play game, print winner and save steps to file
void playGame(char playerName[][STRING_SIZE], FILE *savefile) {
  char square[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
  board(square);
  int player = 1;
  char mark;
  int x=0, y=0;
  while (checkWin(square) == 0) {
    player = (player % 2) ? 1 : 2;
    mark = (player == 1) ? 'X' : 'O';
    printf("\nYour turn %s - %c: ", playerName[player - 1], mark);
    scanf("%d", &x);
    scanf("%d", &y);
    if (checkMarkPosition(x, y)) {
      if (!isMark(square, x, y)) {
	square[x][y] = mark;
	player++;
      }
      else printf("Already marked - Remark!\n");
    }
    else printf("Invalid position - Remark!\n");
    board(square);
    writeToFile(savefile, square);
  }
  if (checkWin(square) == -1) printf("\nDRAW\n");
  else {
    printf("\nPlayer %d - %s win\n", player - 1, playerName[player - 1 - 1]);
  }
}

