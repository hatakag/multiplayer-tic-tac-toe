#ifndef SERVER_H
#define SERVER_H

#define NAME_LENGTH 31
#define IP_LENGTH 16
#define BOARD_SIZE 3

typedef struct ClientNode {
    int sockfd;
    char ip[IP_LENGTH];
    char name[NAME_LENGTH];
    char mark;
    struct ClientNode* opponent;
    char board[BOARD_SIZE][BOARD_SIZE];
} ClientNode;

ClientNode *newNode(int sockfd, char* ip) {
    ClientNode *n = (ClientNode *)malloc( sizeof(ClientNode) );
    n->sockfd = sockfd;
    strncpy(n->ip, ip, IP_LENGTH);
    strncpy(n->name, "NULL", 5);
    n->mark = ' ';
    n->opponent = NULL;
    int i,j;
    for (i=0;i<BOARD_SIZE;i++) {
      for (j=0;j<BOARD_SIZE;j++) {
	n->board[i][j] = ' ';
      }
    }
    return n;
}

void freeNode(ClientNode* n) {
    n->opponent = NULL;
    free(n);
}

void sendOKMsg(int sockfd, char* req, char* msg);
void sendFailMsg(int sockfd, char* req, char* msg);
void sendPosMsg(int sockfd, int x, int y);
bool checkUsernamePassword(char* username, char* password);
void handleLoginReq(ClientNode* clinode, char* username, char* password);
void handleJoinReq(ClientNode* clinode, Queue playerQueue);
void handlePosReq(ClientNode* clinode,int x, int y);
void handleQuitReq(ClientNode* clinode);

#endif // SERVER_H
