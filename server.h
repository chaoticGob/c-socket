#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <netdb.h> 
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <time.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#include "constantes.h"

void prepAddr(struct sockaddr_in *address,  int port);
int createSocket(int port);
void run(int sockfd, int port);
int getRandomNbr(int range);
int checkNbr(int input, int target);
void gameHandling(int toguess, int clientSock);