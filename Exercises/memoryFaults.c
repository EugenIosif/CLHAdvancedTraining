#include <stdio.h>
#include <string.h>

void sensitive_operation() {
    // This variable resides on the stack
    char password[16];
    
    // Simulate entering a secret
    strncpy(password, "SECRET_PASS_123", sizeof(password));
    printf("[+] Sensitive operation: Password processed.\n");

    //do something with the password...

    strncpy(password, "\x00", sizeof(password)); // Attempt to clear the password from memory
    
    // RISK: The function returns without zeroing out the 'password' buffer.
    // The memory remains on the stack until overwritten by another frame.
}

void attacker_function() {
    // This variable will likely occupy the same stack offset 
    // as 'password' in the previous function call.
        // char leaked_data123123[16];

    char leaked_data[16];
    
    printf("[!] Attacker function: Reading uninitialized stack memory...\n");
    printf("[!] Leaked data found: %s\n", leaked_data);
}

int main() {
    printf("--- Stack Residual Data Risk Demo ---\n");

    // 1. Run sensitive logic
    sensitive_operation();

    // 2. Call another function immediately after.
    // Because the stack pointer moves back up and then down again,
    // the new stack frame overlaps with the old one.
    attacker_function();

    printf("\nNote: If the compiler optimizes the stack or uses different\n");
    printf("calling conventions, the leak might require padding adjustments,\n");
    printf("but the fundamental risk of data persistence remains.\n");

    return 0;
}