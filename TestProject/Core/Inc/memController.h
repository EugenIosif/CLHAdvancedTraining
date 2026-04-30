#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "aes.h"

#define BLOCKLEN 16

static uint8_t AES_key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

uint8_t replaceWithEncryptedData(uint8_t size, uint8_t startAddress);
uint8_t * encryptDataWithPadding(uint8_t size, uint8_t * buffer);
void readFromMemory(uint8_t * returnVector, uint8_t size, uint8_t startAddress);
void writeToMemory(uint8_t * sourceBuffer, uint8_t size, uint8_t startAddress);
