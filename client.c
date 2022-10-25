#include "client.h"

// Function to prepare the internet addresses
void prepAddr(struct sockaddr_in *address, const char *host, int port)  {
    size_t addrSize = sizeof(address);
    // erasing address
    memset(address, 0, addrSize);
    // assigning IP, port
    address->sin_family = AF_INET;
    inet_pton(AF_INET, host, &(address->sin_addr));
    address->sin_port = htons(port);
}


int createSocket(const char *host, int port)    {
    // internet adresses for the server and the client
    struct sockaddr_in servAddr;
    
    // socket's file descriptor 
    int sockfd;
    
    // creating a socket of stream type and checking 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        printf("%d\n", sockfd);
        printf("Client socket creation failed.\n"); 
        exit(EXIT_FAILURE); 
    } 
    else
    {
        printf("Client socket successfully created.\n"); 
    }
    
    // setting up addresses and port
    prepAddr(&servAddr, host, port);
  
    // connecting the client socket to server socket 
    if (connect(sockfd, (SA*)&servAddr, sizeof(servAddr)) != 0) { 
        printf("Connection with the server failed.\n"); 
        exit(EXIT_FAILURE); 
    } 
    else
    {
        printf("Connected to the server at the adress: %s:%d\n", host, port);
        return sockfd;
    }
}


// Function designed for chat between client and server 
void cliChat(int sockfd)    { 
    // buff is holding the server's message and guess the client's guess
    char buff[MAX], guess[GSIZE];
    int temp = 0;

    // retrieving the guessing range
    read(sockfd, buff, sizeof(buff)); 
    printf("Your guess must be in the range : %s\n", buff);      

    while(1)
    { 
        // emptying message buffer
        bzero(buff, sizeof(buff)); 

        printf("Enter your guess (it must be a number): ");

        // retrieving value from stdin
        fgets(guess, GSIZE, stdin);

            // checks if the guess is a number
            // if (strcmp(guess, "0\n") == 0) { temp = 1; }
           // else { temp = atoi(guess); }
       
        
        printf("Your guess is: %s\n", guess);

        // writing message in the socket's file descriptor
        if (write(sockfd, guess, strlen(guess)) != strlen(guess)) {
            perror("Failed to send the guessed number");
            exit(EXIT_FAILURE);
        } else { printf("Guess successfully sent\n");}

        // reading for server's message
        read(sockfd, &buff, sizeof(buff)); 

        // printing message from server to stdout
        printf("From Server : %d\n", buff[0]); 
        printf("The target value is: %u\n", buff[1]);

        int status = (int) buff[0];

        switch (status)
        {
        // exiting when won or lost
        case WIN:
            printf("Won the game, client exiting.\n");
            exit(0);
            break;
        case TOO_HIGH:
            printf("Guess is too high, try again.\n");
            break;
        case TOO_LOW:
            printf("Guess is too low, try again.\n");
            break;
        default:
            printf("Lost the game, client exiting.\n");
            exit(0);
            break;
        }
        
    } 
} 


// Driver function for the client side
int main(int argc, const char *argv[])    { 
    // socket's file descriptor
    int sockfd;
    const char *host;
    int port;

    // checking for correct nbr of arguments 
    if (argc != 3) {
        fprintf(stderr, "USAGE: %s <host> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    } 

    // formatting host and port input 
    host = argv[1];
    port = atoi(argv[2]);
    // creating a socket with the server
    sockfd = createSocket(host, port);

    // enabling chat between client and server
    cliChat(sockfd); 
  
    // closing the socket 
    close(sockfd); 

    return 0;
} 
