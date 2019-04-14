#ifndef SERVER_H
#define SERVER_H

#define NAME_LENGTH 31
#define IP_LENGTH 16
#define BOARD_SIZE 3

typedef struct ClientNode {
    int sockfd;
    char ip[IP_LENGTH];
    char name[NAME_LENGTH];
    ClientNode* opponent;
    char board[BOARD_SIZE][BOARD_SIZE];
} ClientNode;

ClientNode *newNode(int sockfd, char* ip) {
    ClientNode *np = (ClientNode *)malloc( sizeof(ClientNode) );
    np->sockfd = sockfd;
    strncpy(np->ip, ip, IP_LENGTH);
    strncpy(np->name, "NULL", 5);
    np->opponent = NULL;
    np->board = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    return np;
}

#endif // SERVER_H