#include <stdio.h>
// #include <string.h>
typedef void (*func_ptr)();


// Define a simple function to be called through the function pointer
void say_hello() {
    printf("Hello, World!\n");
}

// Define an array of function pointers
void greet() {
    printf("Greetings!\n");
}

void farewell() {
    printf("Goodbye!\n");
}

void functionPointerArray(void) {
    func_ptr func_array[3] = {say_hello, greet, farewell};
    
    // Call each function through the array
    for (int i = 0; i < 3; i++) {
        func_array[i]();
    }
}
    
void main(void)
{
    // Call the function that demonstrates the use of function pointers in an array
    functionPointerArray();
}