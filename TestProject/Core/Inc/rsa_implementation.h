#include "aes.h"

// RSA Operations
void rsa_encrypt(const uint8_t message[8], uint8_t ciphertext[8], const uint8_t e_key[8], const uint8_t n_key[8]);
void rsa_decrypt(const uint8_t ciphertext[8], uint8_t decrypted_message[8], const uint8_t d_key[8], const uint8_t n_key[8]);

void returnPublicKey(uint8_t * publicKey, uint8_t size);

// Test Function
void test_rsa_algorithm(void);