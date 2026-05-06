#include "aes.h"

//static const uint8_t RSA_n[8] = {0xE9, 0xCB, 0x9A, 0x6F, 0x4E, 0x79, 0x26, 0x00};
//static const uint8_t RSA_e[8] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
//static const uint8_t RSA_d[8] = {0x5D, 0x2D, 0xEE, 0xE2, 0xE6, 0x97, 0x1B, 0x00};
static uint8_t RSA_n[8] = {0};
static uint8_t RSA_e[8] = {0};
static uint8_t RSA_d[8] = {0};
void rsa_encrypt(const uint8_t message[8], uint8_t ciphertext[8], const uint8_t e_key[8], const uint8_t n_key[8]);
void rsa_decrypt(const uint8_t ciphertext[8], uint8_t decrypted_message[8], const uint8_t d_key[8], const uint8_t n_key[8]);

// RSA Operations
void returnPublicKey(uint8_t * publicKey, uint8_t size);
void generate_and_check_rsa_keys(uint8_t n_key[8], uint8_t e_key[8], uint8_t d_key[8]);

#define RSA_ENCRYPTION_IF(a, b) rsa_encrypt(a, b, RSA_e, RSA_n)
#define RSA_DECRYPTION_IF(a, b) rsa_decrypt(a, b, RSA_d, RSA_n)
#define KEY_GENERATION generate_and_check_rsa_keys(RSA_n, RSA_e, RSA_d)

// Test Function
void test_rsa_algorithm(void);
