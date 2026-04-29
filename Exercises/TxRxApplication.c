
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "aes.h"

#define TX_RX_BUFFER_SIZE 16

uint8_t memoryLikeBuffer[128] = {   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, \
                                    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, \
                                    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, \
                                    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, \
                                    0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, \
                                    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, \
                                    0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, \
                                    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, \
                                    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, \
                                    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79
                                };

uint8_t AES_key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

void readFromMemory(uint8_t * returnVector, uint8_t size, uint8_t startAddress)
{
    if(returnVector != NULL && size > 0 && (startAddress + size) <= sizeof(memoryLikeBuffer))
    {
        memcpy(returnVector, &memoryLikeBuffer[startAddress], size);
    }
}

void writeToMemory(uint8_t * sourceBuffer, uint8_t size, uint8_t startAddress)
{
    if(sourceBuffer != NULL && size > 0 && (startAddress + size) <= sizeof(memoryLikeBuffer))
    {
        memcpy(&memoryLikeBuffer[startAddress], sourceBuffer, size);
    }
}

uint8_t replaceWithEncryptedData(uint8_t size, uint8_t startAddress)
{
  //MAKE SURE THE SIZE IS 16 BYTES LONG, NEEDED TO BE EQUAL WITH THE AES BLOCK SIZE
  if((size < AES_BLOCKLEN) || (startAddress + size) > sizeof(memoryLikeBuffer))
  {
    printf("Error: Out of bounds.\n");
    return 0xFF;
  }

  //temp buffer for memory manipulation
  uint8_t tempBuffer[AES_BLOCKLEN] = {0};
  uint8_t rounds = size / AES_BLOCKLEN; // Calculate the number of full rounds
  uint8_t remainingBytes = size % AES_BLOCKLEN; // Calculate remaining bytes after full rounds
  //init the AES library
  struct AES_ctx ctx;
  AES_init_ctx(&ctx, AES_key);

  for(int i = 0; i < rounds; i++)
  {
    readFromMemory(tempBuffer, AES_BLOCKLEN, startAddress + (i * AES_BLOCKLEN));
    AES_ECB_encrypt(&ctx, tempBuffer);
    writeToMemory(tempBuffer, AES_BLOCKLEN, startAddress + (i * AES_BLOCKLEN));
  }
  return remainingBytes;
}

uint8_t * encryptDataWithPadding(uint8_t size, uint8_t * buffer)
{
  if(buffer == NULL)
  {
    printf("Error: Buffer is NULL.\n");
    return NULL;
  }  
  if(size > AES_BLOCKLEN)
  {
    printf("Error: Size exceeds block size.\n");
    return NULL;
  }
  
  //init the AES library
  struct AES_ctx ctx;
  AES_init_ctx(&ctx, AES_key);

  static uint8_t returnBuffer[AES_BLOCKLEN] = {0};

  memcpy(&returnBuffer[AES_BLOCKLEN - size], buffer, size);

  AES_ECB_encrypt(&ctx, returnBuffer);
  return returnBuffer;
}

void main(void)
{
  uint8_t dataBuffer[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  uint8_t * encryptedData = encryptDataWithPadding(8, dataBuffer);
}