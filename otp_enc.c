//marcela echavarria

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 256000

void error(const char *msg)
{
    perror(msg);    // Error function used for reporting issues
    exit(0);
}

int main(int argc, char *argv[])
{
    //initializing variables
    int plainTextFile;
    int myKey;
    int numReadKey;
    int numReadPlain;
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char buffer[BUFFER_SIZE];
    char plainBuffer [BUFFER_SIZE];
    char keyBuffer [BUFFER_SIZE];

    if (argc < 3)
    {
        fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]);    // Check usage & args
        exit(0);
    }

    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
    if (serverHostInfo == NULL) //error case
    {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0) error("CLIENT: ERROR opening socket");

    // Connect socket to address
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
        error("CLIENT: ERROR connecting");


    //if unable to open plainText file return error
    if ((plainTextFile = open(argv[1],O_RDONLY)) == -1)
    {
        error("CLIENT: ERROR opening file");
    }

    //if unable to open keyText file return error
    if ((myKey = open(argv[2],O_RDONLY)) == -1)
    {
        error("CLIENT: ERROR opening file");
    }

    //fill plain buffer with nulls
    memset(plainBuffer, '\0', sizeof(plainBuffer));

    //if unable to read plaintext file return error
    if ((numReadPlain = read(plainTextFile, plainBuffer, sizeof(plainBuffer))) < 0)
    {
        error("CLIENT: read error");
    }
    //loop to remove newline before sending
    while (plainBuffer[strlen(plainBuffer)-1]=='\n')
    {
        plainBuffer[strlen(plainBuffer)-1] = '\0';
        numReadPlain--;
    }
    //loop to check for bad character in plaintext file
    int i;
    for(i=0;i<strlen(plainBuffer);i++){
        if((plainBuffer[i]<'A'||plainBuffer[i]>'Z') && plainBuffer[i] != ' '){
            fprintf(stderr,"ERROR: Invalid Character\n");
            return -1;
        }
    }
    strcat(plainBuffer, "#"); //add # to indicate end of file
    numReadPlain++;

    //fill keybuffer with nulls
    memset(keyBuffer, '\0', sizeof(keyBuffer));

    //if unable to read keytext file return error
    if ((numReadKey = read(myKey, keyBuffer, sizeof(keyBuffer))) < 0)
    {
        error("CLIENT: ERROR read error");
    }
    //loop to remove new line from key file
    while (keyBuffer[strlen(keyBuffer)-1]=='\n')
    {
        keyBuffer[strlen(keyBuffer)-1] = '\0';
        numReadKey--;
    }

    //loop to look for invalid character in key buffer
    for(i=0;i<strlen(keyBuffer);i++){
        if((keyBuffer[i]<'A'||keyBuffer[i]>'Z') && keyBuffer[i] != ' '){
            fprintf(stderr,"ERROR: Invalid Character\n");
            return -1;
        }
    }
    strcat(keyBuffer, "#"); //add # to end of keybuffer to indicate end of file
    numReadKey++;

    //too short error if plain text file is longer than key text file
    if (numReadPlain > numReadKey)
    {
        error("CLIENT: ERROR key too short");
        return -1;
    }

    //if unable to send plaintext return error
    if ((charsWritten = send(socketFD, plainBuffer, numReadPlain, 0)) < 0)
    {
        error("CLIENT: ERROR writing to socket");
        return -1;
    }

    //iff unable to send keytext return error
    if ((charsWritten = send(socketFD, keyBuffer, numReadKey, 0)) < 0)
    {
        error("CLIENT: ERROR writing to socket");
        return -1;

    }

    // Get return message from server
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
    char recvBuffer[2000]; //temp buffer
    charsRead = 999999;//number of characters in buffer
    int bufferIndex = 0;

    //loop through read character in chunks of 2000 bites from socket
    while (charsRead >= 2000){
        charsRead = recv(socketFD, recvBuffer, 2000, 0); // Read data from the socket, leaving \0 at end
        if (charsRead < 0) error("CLIENT: ERROR reading from socket"); //error
        for(i=0;i<charsRead;i++){ //copy characters from recv buffer into output buffer
            buffer[bufferIndex++] = recvBuffer[i];
        }
    }
    printf("%s\n", buffer); //print output buffer
    close(socketFD); // Close the socket
    return 0;
}
