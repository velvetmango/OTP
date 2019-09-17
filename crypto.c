//Marcela Echavarria

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>

int charValue (int num)
{
    if (num == 0)
    { //if index 0 return space ascii value
        return 32;
    }
    return num + 64; //returns ascii value of index 
}

int numVal (char letter)
{
    if (letter == 32)
    { //if ascii value is 32 return index 0 for space
        return 0;
    }
    return letter - 64; //returns index value of letter
}

int enc (char plain, char key)
{
    //add plain text value + key text value and perform modulo 27 to return cipher text
    return charValue((numVal(plain) + numVal(key)) % 27);

}


int dec (char cipher, char key)
{
    int temp;
    temp = (numVal(cipher) - numVal(key)); //subtract cipher text from key text

    //negatvive value case add 27
    if (temp < 0)
    {
        temp += 27;
    }
    return charValue(temp % 27); //perform modulo 27 and return decrypted text
}


//used for testing--compiling script will ignore
#ifdef MAIN

int main ()
{
    int i;
    printf("hello world\n");
    char* plainString = "PLAIN";
    char* keyString = "KEY CODE";
    for (i = 0; i < strlen(plainString); i++)
    {
        printf("plain:%c key:%c encrypt:%c decrpt:%c\n",
               plainString[i], keyString[i], enc(plainString[i], keyString[i]),
               dec(enc(plainString[i], keyString[i]), keyString[i]));
        if (plainString[i] != dec(enc(plainString[i], keyString[i]), keyString[i]))
        {
            fprintf (stderr, "Failure\n");
        }
    }
}

#endif
