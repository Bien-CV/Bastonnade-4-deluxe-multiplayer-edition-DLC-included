#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <netdb.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

#ifndef DEF_MAIN
#define DEF_MAIN
int getInt(void);
void menu(int argc, char **argv);
int mainClient(int argc, char **argv);
int mainServer(int argc, char **argv);
void menu(int argc, char **argv);
void renvoi(int sock);
void showAddrList(char ** list );
void showHostent(hostent* h);
void showAliases(char ** list );
#endif
