#ifndef DEFINE_H
#define DEFINE_H

#define NAME_LENGTH 31
#define IP_LENGTH 16
#define BOARD_SIZE 3

#define STRING_LENGTH 50

//request type
#define LOGIN "LOGIN"
#define JOIN "JOIN"
#define POS "POS"
#define QUIT "QUIT"
#define OK "OK"
#define FAIL "FAIL"
#define END "END"
#define REQ "REQ"

//client status
#define NONE 0
#define LOGGED 1
#define JOINED 2
#define WAITING 3
#define MARKING 4

#endif