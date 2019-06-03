#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "queue.h"
#include "server.h"
#include "Tic-Tac-Toe_Lib.h"
#include "define.h"

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */

Queue playerQueue;

void sig_chld(int signo)
{
  pid_t pid;
  int stat;
  while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("child %d terminated, status %d\n", pid, stat);
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
  printf("%s\n", buf);
  free(buf);
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
  printf("%s\n", buf);
  free(buf);
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
  printf("%s\n", buf);
  free(buf);
}

void sendEndMsg(int sockfd, int winnerturn) {
  int wlen;
  wlen = snprintf(NULL, 0, "%d", winnerturn);
  char *wstr = (char*)malloc((wlen+1)*sizeof(char));
  snprintf(wstr, wlen+1, "%d", winnerturn);
  int len = strlen(END) + 1 + wlen;
  char *buf = (char*)malloc(sizeof(char)*(len+1));
  strcpy(buf, END);
  strcat(buf, " ");
  strcat(buf, wstr);
  buf[len] = '\0';
  send(sockfd, buf, len+1, 0);
  printf("%s\n", buf);
  free(buf);
}

bool checkUsernamePassword(char* username, char* password) {
  FILE * fp = fopen("src/user.txt", "r");
  char *name = (char*)malloc(NAME_LENGTH*sizeof(char));
  char *pass = (char*)malloc(STRING_LENGTH*sizeof(char));
  int check = 0;
  while (!feof(fp)) {
    fscanf(fp, "%s%*c%s%*c", name, pass);__fpurge(stdin);
    if (strcmp(username, name) == 0 && strcmp(password, pass) == 0)
      check = 1;
  }
  fclose(fp);
  free(name);
  free(pass);
  if (check == 1) return true;
  else return false;
}

void handleLoginReq(ClientNode* clinode, char* username, char* password) {
  if (clinode->status != NONE) {
    sendFailMsg(clinode->sockfd, LOGIN, "Login_fail---Cannot_login_at_this_stage");
    return;
  }
  if (username == NULL || password == NULL) {
    sendFailMsg(clinode->sockfd, LOGIN, "Login_fail---Invalid_username_&_password");
    return;
  }
  if (checkUsernamePassword(username, password)) {
    strcpy(clinode->name, username);
    clinode->status = LOGGED;
    //printf("Send OK LOGIN msg %s %s\n", username, password);
    sendOKMsg(clinode->sockfd, LOGIN, "Login_successfully");
  } else {
    //printf("Send FAIL LOGIN msg %s %s\n", username, password);
    sendFailMsg(clinode->sockfd, LOGIN, "Login_fail---Wrong_username_&_password");
  }
}

void handleJoinReq(ClientNode* clinode, Queue *playerQueue) {
  if (clinode->status == NONE) {
    sendFailMsg(clinode->sockfd, JOIN, "Join_fail---Cannot_join_at_this_stage");
    return;
  }
  if (clinode->status == JOINED || clinode->status == WAITING || clinode->status == MARKING) {
    sendFailMsg(clinode->sockfd, JOIN, "Join_fail---Already_joined");
    return;
  }
  enQueue(clinode, playerQueue);
  clinode->status = JOINED;
  //printf("%d\n", clinode->sockfd);
  //printQueue(*playerQueue);
  if (fullQueue(*playerQueue)) {
    ClientNode* opponent = deQueue(playerQueue);
    opponent->opponent = clinode;
    clinode->opponent = opponent;
    deQueue(playerQueue);
    clinode->status = WAITING;
    opponent->status = MARKING;
    clinode->mark = 'O';
    opponent->mark = 'X';
    //printf("Send OK JOIN msg %d %d\n", clinode->sockfd, opponent->sockfd);
    sendOKMsg(clinode->sockfd, JOIN, "2");
    sendOKMsg(opponent->sockfd, JOIN, "1");
  }
}

void endMatch(ClientNode* clinode, int winnerturn) {
  sendEndMsg(clinode->sockfd, winnerturn);
  sendEndMsg(clinode->opponent->sockfd, winnerturn);
  int i,j;
  for (i=0;i<BOARD_SIZE;i++) {
    for (j=0;j<BOARD_SIZE;j++) {
      clinode->board[i][j] = ' ';
      clinode->opponent->board[i][j] = ' ';
    }
  }
  clinode->status = LOGGED;
  clinode->opponent->status = LOGGED;
  clinode->opponent->opponent = NULL;
  clinode->opponent = NULL;
}

void handlePosReq(ClientNode* clinode,int x, int y) {
  if (clinode->status == WAITING) {
    sendFailMsg(clinode->sockfd, POS, "Pos_fail---Not_your_turn");
    return;
  }
  if (clinode->status != MARKING && clinode->status != WAITING) {
    sendFailMsg(clinode->sockfd, POS, "Pos_fail---Not_join_a_game_yet");
    return;
  }
  if (checkMarkPosition(x, y) && !isMark(clinode->board, x, y)) {
    //printf("Send OK POS msg %d %d %d %d\n", clinode->opponent->sockfd, clinode->sockfd, x, y);
    sendPosMsg(clinode->opponent->sockfd, x, y);
    sendOKMsg(clinode->sockfd, POS, "Pos_successfully---Valid_mark_position");
    clinode->board[x][y] = clinode->mark;
    clinode->opponent->board[x][y] = clinode->mark;
    clinode->status = WAITING;
    clinode->opponent->status = MARKING;
  } else {
    //printf("Send FAIL POS msg %d %d %d %d\n", clinode->opponent->sockfd, clinode->sockfd, x, y);
    sendFailMsg(clinode->sockfd, POS, "Pos_fail---Invalid_mark_position");
  }
  if (checkWin(clinode->board) != 0) {
    sleep(1);
    endMatch(clinode, checkWin(clinode->board));
  }
}

void handleQuitReq(ClientNode* clinode) {
  if (clinode->status == MARKING || clinode->status == WAITING) {
    if (clinode->mark == 'X')
      endMatch(clinode, 2);
    else endMatch(clinode, 1);
  }
  if (clinode->status == JOINED)
    deQueue(&playerQueue);
  printf("Delete data of client %d\n", clinode->sockfd);
  free(clinode); //delete client data
}

void handleClient(void* c) {
  int n;
  char buf[MAXLINE];
  ClientNode* clinode = (ClientNode*) c;
  printf("Client connect at sockfd: %d\n", clinode->sockfd);
  while ( (n = recv(clinode->sockfd, buf, MAXLINE,0)) > 0)  {
    //printf("\{%s\}", buf);
    int i = 0;
    char *p = strtok (buf, " ");
    char *token[3];
    while (p != NULL)
      {
	token[i++] = p;
	p = strtok (NULL, " ");
      }
			  
    if (strcmp(token[0], LOGIN) == 0) {
      printf("Handle request: LOGIN %s %s\n", token[1], token[2]);
      handleLoginReq(clinode, token[1], token[2]);
    } else if (strcmp(token[0], JOIN) == 0) {
      printf("Handle request: JOIN\n");
      handleJoinReq(clinode, &playerQueue);
    } else if (strcmp(token[0], POS) == 0) {
      printf("Handle request: POS %d %d\n", atoi(token[1]), atoi(token[2]));
      handlePosReq(clinode, atoi(token[1]), atoi(token[2]));
    } else if (strcmp(token[0], QUIT) == 0) {
      printf("Handle request: QUIT\n");
      handleQuitReq(clinode);
      close(clinode->sockfd);
    } else {
      printf("%s\n", "Bad request");
      sendFailMsg(clinode->sockfd, REQ, "Bad_request");
    }
  }
  /*
    if (n < 0) {
    printf("%s\n", "Read error");
    exit(0);
    }
  */
}

void catch_ctrl_c_and_exit(int sig) {
  //delete all client
  printf("Bye\n");
  exit(0);
}

int main (int argc, char **argv)
{

  signal(SIGINT, catch_ctrl_c_and_exit);

  int listenfd, connfd, n;
  pid_t childpid;
  socklen_t clilen;
  char buf[MAXLINE];
  struct sockaddr_in cliaddr, servaddr;

  //Queue playerQueue;
  makeNullQueue(&playerQueue);
  
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
    printf("%d\n", connfd);
  
    printf("%s\n","Received request...");

    char *ip = NULL;
    ip = malloc(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(cliaddr.sin_addr), ip, INET_ADDRSTRLEN);
    ClientNode* clinode = newNode(connfd, ip);

    printf ("%s\n","Thread created for dealing with client requests");
    //create thread
    pthread_t id;
    if (pthread_create(&id, NULL, (void *)handleClient, (void *)clinode) != 0) {
      perror("Create pthread error!\n");
      exit(0);
    }

    //close(connfd);
  }
}
