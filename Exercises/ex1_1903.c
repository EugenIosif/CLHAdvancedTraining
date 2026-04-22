#include <stdio.h>

int main(void)
{
    printf("Litera A are valoarea: %d\n", 'A');
    printf("Litera a are valoarea: %d\n", 'a');

    printf("Litera H este a %d-a litera din alfabet\n", ('H' - 'A' + 1));
    return 0;
}