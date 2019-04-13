#ifndef SERVER_H
#define SERVER_H

#define NAME_LENGTH 31
#define IP_LENGTH 16

typedef struct ClientNode {
    int sockfd;
    char ip[IP_LENGTH];
    char name[NAME_LENGTH];
    int opponentSockfd;
} ClientNode;

ClientNode *newNode(int sockfd, char* ip) {
    ClientNode *np = (ClientNode *)malloc( sizeof(ClientNode) );
    np->sockfd = sockfd;
    np->sockfd = 0;
    strncpy(np->ip, ip, IP_LENGTH);
    strncpy(np->name, "NULL", 5);
    return np;
}

#endif // SERVER_H