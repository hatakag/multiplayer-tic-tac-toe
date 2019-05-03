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

#endif // SERVER_H
