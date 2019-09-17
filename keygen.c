//Marcela Echavarria


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]){

//arguments test
if (argc < 2) {
    fprintf (stderr, "Usage: key count \n");
    return 1;
}

    //initialize variables
    int keyLength = atoi(argv[1]); //convert str to int
    char key[keyLength+1];
    int i;
    char* arr = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    //initialize rand
    srand(time(NULL));

//step through length of key and input random char from A-Z and space
    for (i=0; i < keyLength; i++) {
        printf ("%c", arr[rand() % 27]);
    }
    printf("\n"); //add new line at the end

    return 0;

}
