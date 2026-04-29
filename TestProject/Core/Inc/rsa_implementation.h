#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Key Generation
void generate_and_check_rsa_keys(uint8_t n_key[8], uint8_t e_key[8], uint8_t d_key[8]);

// RSA Operations
void rsa_encrypt(const uint8_t message[8], uint8_t ciphertext[8], const uint8_t e_key[8], const uint8_t n_key[8]);
void rsa_decrypt(const uint8_t ciphertext[8], uint8_t decrypted_message[8], const uint8_t d_key[8], const uint8_t n_key[8]);

void u64_to_u8_array(uint64_t val, uint8_t arr[8]);
uint64_t u8_array_to_u64(const uint8_t arr[8]);

void test_rsa_algorithm(void);
