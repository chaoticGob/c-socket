#include "server.h"

void prepAddr(struct sockaddr_in *address,  int port)   {
    size_t addrSize = sizeof(address);
    memset(address, 0, addrSize);
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = htonl(INADDR_ANY);
    address->sin_port = htons(port);
}


int createSocket(int port)  {
    struct sockaddr_in address;
    int sockfd;

    // socket creation and checking 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        printf("Server socket creation failed.\n"); 
        exit(0); 
    } 
    else
    {
        printf("Server socket successfully created.\n");
    }
    prepAddr(&address, port);

    // binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&address, sizeof(address))) != 0) { 
        printf("Server socket binding failed.\n"); 
        exit(0); 
    } 
    else
    {
        printf("Server socket successfully binded.\n");
    }

    // now server is ready to listen and verification 
    if ((listen(sockfd, MAX_PENDING)) != 0) { 
        printf("Server failed to listen.\n"); 
        exit(0); 
    } 
    else
    {
        printf("Server listening...\n");
    }

    return sockfd;
}

void gameHandling(int toguess, int clientSock){
    // msg buffer
    char buff[MAX]; 
    int maxTries = 5;
    int tries = 1;

    while (tries <= maxTries){
        // emptying buffer 
        bzero(buff, MAX); 
        
        // reading the message from the client and copying it in the buffer 
        read(clientSock, buff, sizeof(buff)); 
        
        // printing the buffer's content
        printf("The client %d chose %s \n", clientSock, buff);


        // checks if the client guessed right 
        int thatGuess = checkNbr(atoi(buff), toguess); 
        switch (thatGuess)
        {
        case WIN:
            printf("Client %d wins in %d tries!!\n", clientSock, tries);
            break;
        case TOO_HIGH:
            tries += 1;
            break;
        case TOO_LOW:
            tries += 1;
            break;
        default:
            printf("There was a problem when verifying the guess\n");
            exit(EXIT_FAILURE);
            break;
        }

        // emptying buffer 
        bzero(buff, MAX);

        // filling the buffer with the status of the guess (i.e WIN, LOSE, etc)
        buff[0] = thatGuess;
        buff[1] = toguess;
          
        // sending the game state
        if (write(clientSock, buff, sizeof(buff)) != sizeof(buff)) {
            perror("Failed to send the game state");
            exit(EXIT_FAILURE);
        } else { 
            printf("Game State successfully sent\n");
        }

    } 

    // emptying buffer 
    bzero(buff, MAX);
    if(tries > maxTries)    {
        buff[0] = LOSE;
        write(clientSock, buff, sizeof(buff));
    }
}


// Function handling the server's acceptance of incoming connections and communication
void run(int sockfd, int port)  {
    // guessing range
    char range[] = "[0, 255]";
    
    // client's internet address
    struct sockaddr_in clientAddress;
    unsigned int clientLength = sizeof(clientAddress);
    int rand;

    // client socket's file descriptor
    int clientSock;

    // connections handling loop
    while(1)
    {  
        printf( "Waiting for incoming connections...\n");

        if((clientSock = accept(sockfd, (SA*) &clientAddress, &clientLength)) < 0)  
        {
            printf("Failed to accept client connection\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf( "Client %d connected with ip: %s: %d\n", clientSock,  inet_ntoa(clientAddress.sin_addr), port);
            
            // generating random number 
            rand = getRandomNbr(RANGE);
            printf("Value %u chosen for client %d\n", rand, clientSock);
            
            // Starting communication
            
            // sending the range in which the client has to guess
            if (write(clientSock, range, sizeof(range)) != sizeof(range)) {
                perror("Failed to send the range");
                exit(EXIT_FAILURE);
            } else { printf("Range successfully sent\n");}

            gameHandling(rand, clientSock);
            printf("Last client done, waiting for new clients\n");       
        }

        char entry[5];
        fgets(entry, 5, stdin);
        // checking end of chat
        if (strncmp("exit", entry, 4) == 0) { 
            printf("Server Exit...\n"); 
            exit(EXIT_SUCCESS);
        }  
    }
}



// Function that generates a random nbr in a given range 
int getRandomNbr(int range) {
    
    unsigned char buffer[range];
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, buffer, range);
    //buffer now contains the random data
    close(fd);

    return buffer[0];
}


// Function to check if the number given by the client is correct or not 
int checkNbr(int input, int target) {
    if (input < target)
    {
        return TOO_LOW;
    }
    else if (input > target)
    {
        return TOO_HIGH;
    }
    else
    {
        return WIN;
    }
}
  

// Driver function for the server side
int main(int argc, const char *argv[])  { 
    // socket's file descriptor
    int sockfd;
    int port;
    
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // retrieving port
    port = atoi(argv[1]);
    // checking the port entered is valid 
    if(port > 1024 && port < 65535) {
        // creating socket's hook on server side 
        sockfd = createSocket(port);
        printf( "Server running on port %d\n", port);
        // running connection handling
        run(sockfd, port);

         // closing the socket
        close(sockfd); 

        return EXIT_SUCCESS;

    } else  {
        printf("Port number must be between 1025 and 65534.\n");
        exit(EXIT_FAILURE);
    }
} 