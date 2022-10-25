#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h> 

#include "constantes.h"

void prepAddr(struct sockaddr_in *address, const char *host, int port);
int createSocket(const char *host, int port);
void cliChat(int sockfd);

