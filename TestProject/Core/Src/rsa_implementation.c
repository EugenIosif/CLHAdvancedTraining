#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "rsa_implementation.h"

//static const uint8_t RSA_n[8] = {0xE9, 0xCB, 0x9A, 0x6F, 0x4E, 0x79, 0x26, 0x00};
//static const uint8_t RSA_e[8] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
//static const uint8_t RSA_d[8] = {0x5D, 0x2D, 0xEE, 0xE2, 0xE6, 0x97, 0x1B, 0x00};
uint8_t RSA_n[8] = {0};
uint8_t RSA_e[8] = {0};
uint8_t RSA_d[8] = {0};
// --- Helper Functions ---

/**
 * @brief Converts a 64-bit unsigned integer to a little-endian byte array.
 */
void u64_to_u8_array(uint64_t val, uint8_t arr[8]) {
    for (int i = 0; i < 8; i++) {
        arr[i] = (val >> (i * 8)) & 0xFF;
    }
}

/**
 * @brief Converts a little-endian byte array to a 64-bit unsigned integer.
 */
uint64_t u8_array_to_u64(const uint8_t arr[8]) {
    uint64_t val = 0;
    for (int i = 0; i < 8; i++) {
        val |= (uint64_t)arr[i] << (i * 8);
    }
    return val;
}

/**
 * @brief Returns the public key in a 16 byte array (e, n)
 */
void returnPublicKey(uint8_t * publicKey, uint8_t size)
{
    if((publicKey != NULL) && (size == 16))
    {
        memset(publicKey, 0x00, 16); //set the whole buffer to 0
        memcpy(publicKey, RSA_e, 8); //copy on the first 8 bytes the public exponent
        memcpy(publicKey+8, RSA_n, 8);//copy on the last 8 bytes the modulo
    }    
}

/**
 * @brief Performs modular multiplication (a * b) % mod, avoiding overflow.
 *        Uses a double-and-add approach (Russian Peasant multiplication) 
 *        to prevent 64-bit integer overflow.
 */
uint64_t modular_mul(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t res = 0;
    a %= mod;
    while (b > 0) {
        if (b % 2 == 1) {
            // Safe modular addition: (res + a) % mod
            res = (res >= mod - a) ? (res - (mod - a)) : (res + a);
        }
        // Safe modular doubling: (a * 2) % mod
        a = (a >= mod - a) ? (a - (mod - a)) : (a + a);
        b /= 2;
    }
    return res;
}

/**
 * @brief Performs modular exponentiation (base^exp) % mod using binary exponentiation.
 */
uint64_t modular_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = modular_mul(res, base, mod);
        base = modular_mul(base, base, mod);
        exp /= 2;
    }
    return res;
}

/**
 * @brief Computes the greatest common divisor (GCD) of two numbers using the Euclidean algorithm.
 */
uint64_t gcd(uint64_t a, uint64_t b) {
    while (b) {
        a %= b;
        uint64_t temp = a; a = b; b = temp;
    }
    return a;
}

/**
 * @brief Finds the modular multiplicative inverse of 'a' modulo 'm'.
 * Uses the Extended Euclidean Algorithm.
 * @return The inverse, or -1 if it does not exist.
 */
int64_t mod_inverse(int64_t a, int64_t m) {
    int64_t m0 = m, t, q;
    int64_t x0 = 0, x1 = 1;

    if (m == 1) return 0;

    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;

    return x1;
}

/**
 * @brief A deterministic Miller-Rabin primality test for 64-bit integers.
 *        It is deterministic for all 64-bit integers.
 */
bool is_prime(uint64_t n) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    if (n < 25) return true;

    uint64_t d = n - 1;
    d >>= __builtin_ctzll(d);

    uint64_t bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (int i = 0; i < sizeof(bases)/sizeof(bases[0]); ++i) {
        uint64_t a = bases[i];
        if (n == a) return true;
        uint64_t t = d;
        uint64_t y = modular_pow(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = modular_mul(y, y, n);
            t <<= 1;
        }
        if (y != n - 1 && (t & 1) == 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Generates a random 32-bit prime number.
 */
uint32_t generate_prime(void) {
    uint32_t p;
    do {
        // rand() is not cryptographically secure, but sufficient for this example.
        p = ((uint32_t)rand() << 16) | (uint32_t)rand();
        p |= 1; // Ensure it's odd
    } while (!is_prime(p));
    return p;
}

// --- Main RSA Function Implementations ---

void generate_and_check_rsa_keys(uint8_t n_key[8], uint8_t e_key[8], uint8_t d_key[8]) {
    uint64_t n_val, e_val, d_val_64;
    
    // Loop until valid keys are generated and verified
    while (1) {
        // 1. Find two distinct 32-bit prime numbers p and q
        uint32_t p = generate_prime();
        uint32_t q;
        do {
            q = generate_prime();
        } while (p == q);

        // 2. Calculate n and phi(n)
        n_val = (uint64_t)p * q;

//        if(n_val <= UINT32_MAX)
//        {
//            continue;
//        }

        uint64_t phi_n = (uint64_t)(p - 1) * (q - 1);

        // 3. Choose public exponent e. e=65537 is a common and efficient choice.
        e_val = 65537;
        if (gcd(e_val, phi_n) != 1) {
            // This is very unlikely but possible. If it happens, regenerate p and q.
            continue;
        }

        // 4. Calculate private exponent d, the modular inverse of e mod phi(n)
        int64_t d_val = mod_inverse(e_val, phi_n);
        
        // 5. Check keys: (d * e) % phi_n must be 1
        if (d_val > 0 && modular_mul(d_val, e_val, phi_n) == 1) {
            d_val_64 = (uint64_t)d_val;
            break; // Keys are valid, exit loop
        }
        // If check fails, the loop will continue and regenerate keys.
    }

    // 6. Store keys in the output byte arrays (little-endian)
    u64_to_u8_array(n_val, n_key);
    u64_to_u8_array(e_val, e_key);
    u64_to_u8_array(d_val_64, d_key);
}

void rsa_encrypt(const uint8_t message[8], uint8_t ciphertext[8], const uint8_t e_key[8], const uint8_t n_key[8]) {
    uint64_t m = u8_array_to_u64(message);
    uint64_t e = u8_array_to_u64(e_key);
    uint64_t n = u8_array_to_u64(n_key);

    // printf("\n\r0x");
    // for(uint8_t i = 0; i<8; i++)
    // {
    //     printf("%2x", message[i]);
    // }
    // printf("\n\r");


    if (m >= n) {
        printf("Error: Message is larger than or equal to modulus n. Cannot encrypt.\n\r");
        printf("m = %llu\n\r", m);
        printf("n = %llu\n\r", n);
        

        return;
    }

    uint64_t c = modular_pow(m, e, n);
    u64_to_u8_array(c, ciphertext);
}

void rsa_decrypt(const uint8_t ciphertext[8], uint8_t decrypted_message[8], const uint8_t d_key[8], const uint8_t n_key[8]) {
    uint64_t c = u8_array_to_u64(ciphertext);
    uint64_t d = u8_array_to_u64(d_key);
    uint64_t n = u8_array_to_u64(n_key);

    uint64_t m = modular_pow(c, d, n);
    u64_to_u8_array(m, decrypted_message);
}

void print_key_array(const char* name, const uint8_t key[8]) {
    printf("%s (hex, big-endian): 0x", name);
    for (int i = 7; i >= 0; i--) {
        printf("%02X", key[i]);
    }
    printf("\n");
}

//void test_rsa_algorithm(void) {
//    srand(time(NULL));
//
//    // uint8_t n[8] = {0}, e[8] = {0}, d[8] = {0};
//
//    // printf("Generating 64-bit RSA keys...\n");
//    // generate_and_check_rsa_keys(n, e, d);
//
//    uint64_t n_val = u8_array_to_u64(n);
//
//    printf("Keys generated and validated.\n");
//    print_key_array("n", n);
//    print_key_array("e", e);
//    print_key_array("d", d);
//
//    for (int i = 7; i >= 0; i--) {
//        printf("%02X", n[i]);
//    }
//    printf("\n");
//    for (int i = 7; i >= 0; i--) {
//        printf("%02X", e[i]);
//    }
//    printf("\n");
//    for (int i = 7; i >= 0; i--) {
//        printf("%02X", d[i]);
//    }
//    // Test with a sample message
//    uint8_t original_message[8];
//    uint8_t encrypted_message[8];
//    uint8_t decrypted_message[8];
//
//    // A sample 64-bit message. Must be less than n.
//    uint64_t original_message_val = 0x123456789ABCDEF0;
//    if (original_message_val >= n_val) {
//        printf("\nWarning: Sample message is too large for the generated n. Using a smaller message.\n");
//        original_message_val = n_val / 2;
//    }
//    u64_to_u8_array(original_message_val, original_message);
//
//    printf("\nOriginal message (decimal): %llu\n", original_message_val);
//    print_key_array("Original message", original_message);
//
//    printf("\nEncrypting...\n");
//    rsa_encrypt(original_message, encrypted_message, e, n);
//    print_key_array("Encrypted message", encrypted_message);
//
//    printf("\nDecrypting...\n");
//    rsa_decrypt(encrypted_message, decrypted_message, d, n);
//    uint64_t decrypted_val = u8_array_to_u64(decrypted_message);
//    print_key_array("Decrypted message", decrypted_message);
//
//    // Verification
//    if (original_message_val == decrypted_val) {
//        printf("\nSUCCESS: Decrypted message matches the original.\n");
//    } else {
//        printf("\nFAILURE: Decrypted message does NOT match the original.\n");
//    }
//}

// --- Main Function ---

//int main(void) {
//    test_rsa_algorithm();
//    return 0;
//}
