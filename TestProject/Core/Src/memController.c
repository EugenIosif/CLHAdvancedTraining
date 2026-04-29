
#include "memController.h"

#define BLOCKLEN 16

static void readFromMemory(uint8_t * returnVector, uint8_t size, uint8_t startAddress)
{
    if(returnVector != NULL && size > 0 && (startAddress + size) <= sizeof(memoryLikeBuffer))
    {
        memcpy(returnVector, &memoryLikeBuffer[startAddress], size);
    }
}

static void writeToMemory(uint8_t * sourceBuffer, uint8_t size, uint8_t startAddress)
{
    if(sourceBuffer != NULL && size > 0 && (startAddress + size) <= sizeof(memoryLikeBuffer))
    {
        memcpy(&memoryLikeBuffer[startAddress], sourceBuffer, size);
    }
}

uint8_t replaceWithEncryptedData(uint8_t size, uint8_t startAddress)
{
  //MAKE SURE THE SIZE IS 16 BYTES LONG, NEEDED TO BE EQUAL WITH THE AES BLOCK SIZE
  if((size < BLOCKLEN) || (startAddress + size) > sizeof(memoryLikeBuffer))
  {
    printf("Error: Out of bounds.\n");
    return 0xFF;
  }

  //temp buffer for memory manipulation
  uint8_t tempBuffer[BLOCKLEN] = {0};
  uint8_t rounds = size / BLOCKLEN; // Calculate the number of full rounds
  uint8_t remainingBytes = size % BLOCKLEN; // Calculate remaining bytes after full rounds
  //init the AES library
  struct AES_ctx ctx;
  AES_init_ctx(&ctx, AES_key);

  for(int i = 0; i < rounds; i++)
  {
    readFromMemory(tempBuffer, BLOCKLEN, startAddress + (i * BLOCKLEN));
    AES_ECB_encrypt(&ctx, tempBuffer);
    writeToMemory(tempBuffer, BLOCKLEN, startAddress + (i * BLOCKLEN));
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
  if(size > BLOCKLEN)
  {
    printf("Error: Size exceeds block size.\n");
    return NULL;
  }
  
  //init the AES library
  struct AES_ctx ctx;
  AES_init_ctx(&ctx, AES_key);

  static uint8_t returnBuffer[BLOCKLEN];
  memset(returnBuffer, 0x00, 16);

  memcpy(&returnBuffer[BLOCKLEN - size], buffer, size);

  AES_ECB_encrypt(&ctx, returnBuffer);
  return returnBuffer;
}

//void main(void)
//{
//  uint8_t dataBuffer[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
//  uint8_t * encryptedData = encryptDataWithPadding(8, dataBuffer);
//}
