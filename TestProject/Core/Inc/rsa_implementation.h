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