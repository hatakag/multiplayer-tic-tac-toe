#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "queue.h"
#include "server.h"
#include "Tic-Tac-Toe_Lib.h"

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */

#define STRING_LENGTH 31

#define LOGIN "LOGIN"
#define JOIN "JOIN"
#define POS "POS"
#define QUIT "QUIT"
#define OK "OK"
#define FAIL "FAIL"
#define BOARD_SIZE 3

void sig_chld(int signo)
{
  pid_t pid;
  int stat;
  while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("child %d terminated\n", pid);
  return;
}
/*
WNOHANG: waitpid does not block
while loop: waitpid repeatedly until there is no child
process change status, i.e until waitpid returns 0.
*/

void sendOKMsg(int sockfd, char* req, char* msg) {
  int len = strlen(req) + strlen(msg) + strlen(OK) + 2;
  char *buf = (char*)malloc(sizeof(char)*(len+1));
  strcpy(buf, OK);
  strcat(buf, " ");
  strcat(buf, req);
  strcat(buf, " ");
  strcat(buf, msg);
  buf[len] = '\0';
  send(sockfd, buf, len+1, 0);
}

void sendFailMsg(int sockfd, char* req, char* msg) {
  int len = strlen(req) + strlen(msg) + strlen(FAIL) + 2;
  char *buf = (char*)malloc(sizeof(char)*(len+1));
  strcpy(buf, FAIL);
  strcat(buf, " ");
  strcat(buf, req);
  strcat(buf, " ");
  strcat(buf, msg);
  buf[len] = '\0';
  send(sockfd, buf, len+1, 0);
}

void sendPosMsg(int sockfd, int x, int y) {
  int xlen, ylen; 
  xlen = snprintf(NULL, 0, "%d", x);
  ylen = snprintf(NULL, 0, "%d", y);
  char *xstr, *ystr;
  xstr = (char*)malloc((xlen+1)*sizeof(char));
  ystr = (char*)malloc((ylen+1)*sizeof(char));
  snprintf(xstr, xlen+1, "%d", x);
  snprintf(ystr, ylen+1, "%d", y);
  int len = strlen(POS) + 2 + xlen + ylen;
  char *buf = (char*)malloc(sizeof(char)*(len+1));
  strcpy(buf, POS);
  strcat(buf, " ");
  strcat(buf, xstr);
  strcat(buf, " ");
  strcat(buf, ystr);
  buf[len] = '\0';
  send(sockfd, buf, len+1, 0);
}

bool checkUsernamePassword(char* username, char* password) {
  FILE * fp = fopen("user.txt", "r");
  char name[STRING_LENGTH], pass[STRING_LENGTH];
  int check = 0;
  while (!feof(fp)) {
    fscanf(fp, "%s%*c%s%*c", name, pass);
    if (strcmp(username, name) == 0 && strcmp(password, pass) == 0)
      check = 1;
  }
  fclose(fp);
  if (check == 1) return true;
  else return false;
}

void handleLoginReq(ClientNode* clinode, char* username, char* password) {
  if (checkUsernamePassword(username, password)) {
    strcpy(clinode->name, username);
    sendOKMsg(clinode->sockfd, LOGIN, "login successfully");
  } else {
    sendFailMsg(clinode->sockfd, LOGIN, "login fail");
  }
}

void handleJoinReq(ClientNode* clinode, Queue playerQueue) {
  enQueue(clinode, &playerQueue);
  if (fullQueue(playerQueue)) {
    ClientNode* opponent = deQueue(&playerQueue);
    opponent->opponent = clinode;
    clinode->opponent = opponent;
    deQueue(&playerQueue);
    sendOKMsg(clinode->sockfd, JOIN, "match found");
    sendOKMsg(opponent->sockfd, JOIN, "match found");
    clinode->mark = 'X';
    opponent->mark = 'O';
  }
}

void handlePosReq(ClientNode* clinode,int x, int y) {
  if (checkMarkPosition(x, y) && !isMark(clinode->board, x, y)) {
    sendPosMsg(clinode->opponent->sockfd, x, y);
    sendOKMsg(clinode->sockfd, POS, "valid mark position");
    clinode->board[x][y] = clinode->mark;
    clinode->opponent->board[x][y] = clinode->mark;
  } else {
    sendFailMsg(clinode->sockfd, POS, "invalid mark position");
  }
  if (checkWin(clinode->board)==1 || checkWin(clinode->board)==-1) {
    int i,j;
    for (i=0;i<BOARD_SIZE;i++) {
      for (j=0;j<BOARD_SIZE;j++) {
	clinode->board[i][j] = ' ';
	clinode->opponent->board[i][j] = ' ';
      }
    }
  }
}

void handleQuitReq(ClientNode* clinode) {
  //delete client data
  printf("Delete data of client %d\n", clinode->sockfd);
  free(clinode);
}


int main (int argc, char **argv)
{
  int listenfd, connfd, n;
  pid_t childpid;
  socklen_t clilen;
  char buf[MAXLINE];
  struct sockaddr_in cliaddr, servaddr;

  Queue playerQueue;
  
  //Create a socket for the soclet
  //If sockfd<0 there was an error in the creation of the socket
  if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    perror("Problem in creating the socket");
    exit(2);
  }

  //preparation of the socket address 
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  //bind the socket
  bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  //listen to the socket by creating a connection queue, then wait for clients
  listen (listenfd, LISTENQ);

  printf("%s\n","Server running...waiting for connections.");

  for ( ; ; ) {

    clilen = sizeof(cliaddr);
    //accept a connection
    connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
  
    printf("%s\n","Received request...");

    if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

      printf ("%s\n","Child created for dealing with client requests");

      //close listening socket
      close (listenfd);

      //
      char *ip = NULL;
      ip = malloc(INET_ADDRSTRLEN);
      inet_ntop(AF_INET, &(cliaddr.sin_addr), ip, INET_ADDRSTRLEN);
      ClientNode* clinode = newNode(connfd, ip);
	
      while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {

	int i = 0;
	char *p = strtok (buf, " ");
	char *token[3];
	while (p != NULL)
	  {
	    token[i++] = p;
	    p = strtok (NULL, " ");
	  }
		  
	if (strcmp(token[0], LOGIN) == 0) {
	  handleLoginReq(clinode, token[1], token[2]);
	} else if (strcmp(token[0], JOIN) == 0) {
	  handleJoinReq(clinode, playerQueue);
	} else if (strcmp(token[0], POS) == 0) {
	  handlePosReq(clinode, atoi(token[1]), atoi(token[2]));
	} else if (strcmp(token[0], QUIT) == 0) {
	  handleQuitReq(clinode);
	  close(connfd);
	} else {
	  printf("%s\n", "Bad request");
	}
      }

      if (n < 0) {
	printf("%s\n", "Read error");
	exit(0);
      }
      //
		
    }
    signal(SIGCHLD,sig_chld);
    close(connfd);
  }
}
