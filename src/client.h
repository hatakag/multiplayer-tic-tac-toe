#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "define.h"
#include "Tic-Tac-Toe_Lib.h"

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/

#ifndef O_ASYNC
#define O_ASYNC FASYNC
#endif

void signio_handler(int signo);
void catch_ctrl_c_and_exit(int sig);
void sendLoginReq(char *username, char* password);
void sendJoinReq();
void sendPosReq(int x, int y);
void sendQuitReq();
void handleRes(char *res);
void displayLoginScreen();
void displayMenuScreen();
void displayPlayingScreen();
void clearBoard();
void markAt(char m, int x, int y);

#endif
