//Marcela Echavarria

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256000

// Error function used for reporting issues
void error(const char *msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
}

int main(int argc, char *argv[])
{
    //initializing variables
    int numRead;
    int i;
    int pid;
    int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
    socklen_t sizeOfClientInfo;
    char buffer[BUFFER_SIZE];
    char plainBuffer [BUFFER_SIZE];
    char keyBuffer [BUFFER_SIZE];
    char cipherBuffer [BUFFER_SIZE];
    struct sockaddr_in serverAddress, clientAddress;
    int enc(char, char);
    if (argc < 2)
    {
        fprintf(stderr,"USAGE: %s port\n", argv[0]);    // Check usage & args
        exit(1);
    }

    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

    // Set up the socket
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (listenSocketFD < 0)
    {
        error("ERROR opening socket");
        perror("");
        exit(-1);
    }

    // Enable the socket to begin listening
    if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)   // Connect socket to port
    {
        error("ERROR on binding");
        perror("");
        exit(-1);
    }
    listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

    while (1) //keep looping
    {
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0)
        {
            error("ERROR on accept");
            continue;
        }
        pid = fork(); //begin fork
        if (pid == -1) //error case
        {
            close(establishedConnectionFD);
            continue;
        }
        else if (pid > 0) //parent
        {
            close(establishedConnectionFD);
            continue;
        }

        else //child
        {
            //fill plain buffer and buffer with nulls
            memset(plainBuffer, '\0', sizeof(plainBuffer));
            memset(buffer, '\0', sizeof(buffer));
            //loop variables
            int done = 0;
            int plainindex = 0;
            //lopp while not done
            while (!done)
            {
                numRead = recv(establishedConnectionFD, buffer, 10000, 0); //receiving chunks of 10000 bytes at a time
                if (numRead < 0) error("ERROR reading from socket"); //error case if nothing received
                for (i= 0; i< numRead; i++){ //looping through and concatenating to the end of plainbuffer
                    plainBuffer[plainindex++] = buffer[i];
                }
                //filling buffer with nulls
                memset(buffer, '\0', sizeof(buffer));

                char * first = strstr(plainBuffer,"#"); //look for # to indicate end of file
                if(first!=NULL) //if found
                {
                    *first = '\0'; //remove hash key
                    done=1; //end loop
                }
            }

            done = 0;
            int keyindex = 0;
            //loop for keyBuffer same as above but for reading the key text instead of plain text
            while (!done)
            {
                numRead = recv(establishedConnectionFD, buffer, 10000, 0); //receiving text in 10000 byte chunks
                if (numRead < 0) error("ERROR reading from socket"); //error case
                for (i= 0; i< numRead; i++){ //looping throught and concatenating to the end of plainbuffer
                    keyBuffer[keyindex++] = buffer[i];
                }
                memset(buffer, '\0', sizeof(buffer)); //fill buffer with nulls
                char * first = strstr(keyBuffer,"#"); //look for # in keyBuffer to indicate end of file
                if(first!=NULL) //if found
                {
                    *first = '\0'; //remove hash key
                    done=1; //end loop
                }
            }
            //fill cipherBuffer with nulls
            memset(cipherBuffer, '\0', sizeof(cipherBuffer));
            for (i = 0; i < strlen(plainBuffer); i++)
            {
                cipherBuffer[i] = enc(plainBuffer[i], keyBuffer[i]); //encypt each byte into cipher buffer using func from crpyto.c
            }

            int cipherIndex = 0;
            int cipherLength = strlen(plainBuffer);

            //send cipher text in 2000 byte chunks
            while(cipherIndex < cipherLength) {
                charsRead = send(establishedConnectionFD, &cipherBuffer[cipherIndex], 2000, 0); // Send success back in 2000 bytes at a time
                if (charsRead < 0) error("ERROR writing to socket"); //error case
                cipherIndex+=charsRead;
            }
            close(establishedConnectionFD); //close connection
            exit(0);
        }

    }
    close(listenSocketFD); //Close the listening socket
    return 0;
}
