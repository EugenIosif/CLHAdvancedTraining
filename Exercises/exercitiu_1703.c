#include <stdio.h>
int main()
{    
    // char c = 5;
    // char d = 9;
    // char or = c | d;
    // printf("%d", or ); 
    // unsigned char mask4 = (1 << 4);
    // unsigned char mask5 = (1 << 5);
    // unsigned char bit4 = (or & mask4) ;
    // unsigned char bit5 = (or & mask5) ;
    // or &= ~mask4;      
    // or  &= ~mask5;      
    // or &= ~(1 << 0);   
    // or &= ~(1 << 1); 
    // or |= (bit4 >> 4);
    // or |= (bit5 >> 5);
    // printf("%d", or);

unsigned char value = 0x2F; // 0010 1111 in binary
unsigned char mask = 0b00000101 << 3; // 101 in binary is 0000 0101, shifting left by 3 gives 0010 1000
unsigned char result = value & mask; // Perform bitwise AND
result = result >> 3; // Shift the result right by 3 to get the original bits in the correct position
printf("%d\n\r", result); // Output the result (should be 5)
// result &= 0b11111101; // Clear the 2nd bit (bit 1) - do not use this ever as it is prone to mistakes and is not readable
mask = 1<<1; // 1 shifted left by 1 gives 0000 0010
result &= ~mask; // Clear the 2nd bit (bit 1)
//result = result & mask;// Clear the 2nd bit (bit 1)
//result &= ~(1 << 1); // Clear the 2nd bit (bit 1)
mask = 1<<2; // 1 shifted left by 2 gives 0000 0100
unsigned char temp = result & mask; // store the value of the 3rd bit (bit 2) in a temporary variable
result &= ~mask; // Clear the 3rd bit (bit 2)
temp = temp >> 1; // Shift the temporary variable right by 2 to get the original bit in the correct position
result = result | temp; // Set the 2nd bit (bit 1) to the value of the temporary variable
printf("%d\n\r", result); // Output the result (should be 3)
}