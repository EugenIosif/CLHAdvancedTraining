#include <string.h>
#include <stdio.h>

void main (void)
{

    char string1[30];
    for(int i = 0; i < 30; i++)
    {
        string1[i] = 'x';
    }
    memcpy(string1, "Hello this is not a joke", 24);
    string1[24] = '\0';

    // for(int i = 0; i < 30; i++)
    // {
    //     printf("%c", string1[i]);
    // }
    
    printf("%s", string1);

}

