#include <stdio.h>
#include <string.h>

// Global variables are often stored in the data segment
// The compiler may place 'admin_privilege' immediately after 'password'
char password[15];
char admin_privilege = 0; // '0' for No, '1' for Yes
int account_money = 0;

void check_access() {
    printf("Enter password: \n\r");
    
    // DANGER: gets() does not check the size of the destination buffer.
    // If the user enters 16 characters, the 16th character will 
    // overwrite the memory address where 'admin_privilege' is stored.
    // Using sprintf to write 15 characters + 'Y' to overflow the buffer:
    // sprintf(password, "123456789012345"); // 15 chars to fill the buffer
    sprintf(password, "123456789012345\x01\x7F\x03\x04\x05\x06\x07\x08\x09"); // 15 chars + 1 char to set admin_privilege to 1 and some extra bytes to overwrite adjacent memory 
                                                                             //(if needed)
    // gets(password);

    if (admin_privilege == 1) {
        printf("Access Level: Administrator\n");
    } else {
        printf("Access Level: Standard User\n");
    }
    printf("Account Balance: $%d\n", account_money);
}

int main() {
    check_access();
    return 0;
}
