/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stm32u5xx_hal_def.h>
#include "memController.h"
#include "RSA_keys_newImplementation.h"
// #include "rsa_implementation.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_DATA_LENGTH 9
#define IDLE 0
#define NEGOTIATING 1
#define WAITINGFORSECRET 2
#define RECEIVEENCRYPTEDMESSAGE 3
#define CLOSED 4
#define EXCHANGEINITIALIZATION 0x01
#define SECRETTANSMISSION 0x02
#define SECRETRECEIVED 0x03
#define ENCRYPTEDMESSAGERECEPTION 0x04

#define MAX_ENCRYPT_SIZE 256
#define RSA_KEY_SIZE 2048

#define KEY_TRANSMISSION 0x01
#define PAYLOAD_TRANSMISSION 0x02

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

typedef union uint32ToBytes
{
  /* data */
  uint32_t value;
  uint8_t bytes[4];
}uint32ToBytes;

typedef union uint64ToBytes
{
  /* data */
  uint32_t value;
  uint8_t bytes[8];
}uint64ToBytes;


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;
__IO uint32_t BspButtonState = BUTTON_RELEASED;
ADC_HandleTypeDef hadc1;

CRC_HandleTypeDef hcrc;

HASH_HandleTypeDef hhash;

RNG_HandleTypeDef hrng;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t state = IDLE;
char buffer[100] = {0};
uint8_t DH_e = 2;
uint64_t DH_n = 18446744073709551557ULL;
uint64_t DH_myPrivateIntermediary = 987654321ULL;

uint8_t dummyFunctionDataArray[256] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, \
                                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, \
                                        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, \
                                        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, \
                                        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, \
                                        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, \
                                        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, \
                                        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, \
                                        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                                        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                                        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                                        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                                        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                                        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                                        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                                        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

/* USER CODE BEGIN PV */
UART_HandleTypeDef huart1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_HASH_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CRC_Init(void);
static void MX_RNG_Init(void);
/* USER CODE BEGIN PFP */

static void MX_USART1_UART_Init(void);
uint32_t computeHash (const uint8_t * bytes, size_t numberOfBytes);
uint8_t * prepareTransmission(const uint8_t * transmissionBuffer, uint8_t size, uint8_t mode);
void simpleXORencrypt (uint8_t * bufferToEncrypt, uint8_t size);
HAL_StatusTypeDef ComputeSHA256WithHAL(uint8_t * startAddress, uint32_t length, uint8_t *outputHash);
void bytes_to_hex_string(uint8_t * inbuff, uint8_t size, uint8_t * outbuff);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void bytes_to_hex_string(uint8_t * inbuff, uint8_t size, uint8_t * outbuff)
{
    for (uint8_t i = 0; i < size; i++)
    {
        sprintf((char*)&outbuff[i * 2], "%02x", inbuff[i]);
    }
}

HAL_StatusTypeDef ComputeSHA256WithHAL(uint8_t * startAddress, uint32_t length, uint8_t *outputHash)
{
    HAL_StatusTypeDef status;
    memset(outputHash, 0x00, 32);

    status = HAL_HASHEx_SHA256_Start(&hhash, startAddress, length, outputHash, HAL_MAX_DELAY);
    return status;
}
//
//void encryptU32WithRSA(uint32_t * inputValue, uint64_t * outputValue)
//{
//	if(inputValue != NULL && outputValue != NULL)
//	{
//		uint32ToBytes inputTempValue;
//		uint64ToBytes outputTempValue[8];
//		//8 entries in the array
//		for(uint8_t i = 0; i < 8; i++)
//		{
//		  //used for byte access of the input
//		  inputTempValue.value = inputValue[i];
//		  uint8_t dummyArray[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//		  memcpy(dummyArray+4, inputTempValue.bytes, 4);
////      KEY_GENERATION;
//		  RSA_ENCRYPTION_IF(dummyArray, outputTempValue[i].bytes);
//		}
//	}
//}

uint8_t * prepareTransmission(const uint8_t * inputBuffer, uint8_t size, uint8_t mode)
{
  static uint8_t buffer[TRANSMISSION_BYTE_LEN];
  //reset the previous content of buffer
  memset(buffer, 0x00, sizeof(buffer));
  if(inputBuffer != NULL && size > 0)
  {
    if(mode == KEY_TRANSMISSION)
    {
      //we are here because we are trying to transmit a key to the other side. 
      //Hence the 16 bytes -> only the 4 byte hash needed.
      uint32ToBytes tempValue;
      tempValue.value = 0;
      tempValue.value = computeHash(inputBuffer, size);
      memcpy(&buffer[0], tempValue.bytes, 4);
      memcpy(&buffer[4], inputBuffer, size);
    }
    else if(mode == PAYLOAD_TRANSMISSION)
    {
      //we are here because we are trying to transmit The contents of a function to the other side.
      //SHA256 is used for this
      uint8_t ouputHash[32];
      memset(ouputHash, 0x00, 32);
      ComputeSHA256WithHAL((const uint8_t*)inputBuffer, size, ouputHash);

      memcpy(&buffer[0], ouputHash, 32);
      memcpy(&buffer[32], inputBuffer, size);
    }
  }
  return buffer;
}

uint32_t computeHash(const uint8_t *bytes, size_t numberOfBytes) {
    uint32_t hash = 0x811c9dc5;
    for(size_t i = 0; i < numberOfBytes; i++) {
        hash ^= bytes[i];
        hash *= 0x01000193;
    }
    return hash;
}

void executeDiffieHellman(void)
{
    uint8_t uart_data_tx[UART_DATA_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t uart_data_rx[UART_DATA_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    while(1)
    {
        if(state == IDLE)
        {
            uart_data_tx[0] = EXCHANGEINITIALIZATION; //signal the start of the exchange
            HAL_UART_Transmit(&huart1, (uint8_t*)uart_data_tx, UART_DATA_LENGTH, HAL_MAX_DELAY);
            BspButtonState = BUTTON_RELEASED;
            state = WAITINGFORSECRET;
        }
        else if(state == WAITINGFORSECRET)
        {
            HAL_UART_Receive(&huart1, (uint8_t*)uart_data_rx, UART_DATA_LENGTH, HAL_MAX_DELAY);
            if(uart_data_rx[0] == SECRETRECEIVED)
            {
                uint8_t privIntermediaryArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                uint8_t nArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//                u64_to_u8_array(DH_n, nArr);
//                u64_to_u8_array(DH_myPrivateIntermediary, privIntermediaryArr);
                uint8_t shared_key_arr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                
//                COMPUTE_DH_KEY(&uart_data_rx[1], shared_key_arr, privIntermediaryArr, nArr);
                uint8_t keyBuffer[16];
                memset(keyBuffer, 0x00, 16);
                for(uint8_t i = 0; i < 16; i+=2)
                {
                    keyBuffer[i] = shared_key_arr[i];
                    keyBuffer[i+1] = shared_key_arr[7-i];
                }
                WRITE_AES_KEY(keyBuffer);
                state = NEGOTIATING;
            }
        }
        else if(state == NEGOTIATING)
        {
            //Transition logic for NEGOCIATING
            uint8_t temporaryU64[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//            uint8_t eArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//            uint8_t nArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//            uint8_t privIntArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//            u64_to_u8_array(DH_e, eArr);
//            u64_to_u8_array(DH_n, nArr);
//            u64_to_u8_array(DH_myPrivateIntermediary, privIntArr);
//            COMPUTE_DH_KEY(eArr, temporaryU64, privIntArr, nArr);
            
            uart_data_tx[0] = SECRETTANSMISSION;
            memcpy(uart_data_tx+1, temporaryU64, 8);
            HAL_UART_Transmit(&huart1, (uint8_t*)uart_data_tx, UART_DATA_LENGTH, HAL_MAX_DELAY);

            state = CLOSED;
        }
        
        else if(state == CLOSED)
        {
            state = IDLE;
            break;
        }  
        else
        {
            state = IDLE;
            break;
        }
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the System Power */
  SystemPower_Config();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_HASH_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_CRC_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
  MX_USART1_UART_Init();

  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_RED);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN BSP */

  /* -- Sample board code to send message over COM1 port ---- */

  printf("\n\rWelcome to STM32 world !\n\r");

  uint8_t transmissionBuffer[240];
  memset (transmissionBuffer, 0x00, 240);
  // KEY_GENERATION;
  Aes aes;
  wc_AesInit(&aes, NULL, INVALID_DEVID);
  wc_AesSetKey(&aes, AES_key, 16, NULL, AES_ENCRYPTION);

	extern const uint8_t u8RSAPublicKey[];
  extern const uint8_t u8RSAPrivateKey[];
  RsaKey rsaKey;
  WC_RNG rng;
  wc_InitRng(&rng);
  wc_InitRsaKey(&rsaKey, NULL);
  //The index is used to ensure the right order of key usage
  word32 u32index = 0;
  wc_RsaPrivateKeyDecode(u8RSAPrivateKey, &u32index, &rsaKey, sizeof(u8RSAPrivateKey));
  u32index = 0;
  wc_RsaPublicKeyDecode(u8RSAPublicKey, &u32index, &rsaKey, sizeof(u8RSAPublicKey));
  //Binding the Key and RNG
  wc_RsaSetRNG(&rsaKey, &rng);
  // Aes

  /* -- Sample board code to switch on leds ---- */
  BSP_LED_On(LED_GREEN);
  BSP_LED_On(LED_BLUE);
  BSP_LED_On(LED_RED);

  /* USER CODE END BSP */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* -- Sample board code for User push-button in interrupt mode ---- */
    if (BspButtonState == BUTTON_PRESSED)
    {
      /* Update button state */
      BspButtonState = BUTTON_RELEASED;
      /* -- Sample board code to toggle leds ---- */
      BSP_LED_Toggle(LED_GREEN);
      BSP_LED_Toggle(LED_BLUE);
      BSP_LED_Toggle(LED_RED);

      executeDiffieHellman();
      HAL_Delay(100);

      memcpy(transmissionBuffer, prepareTransmission(u8RSAPublicKey, sizeof(u8RSAPublicKey), KEY_TRANSMISSION), sizeof(u8RSAPublicKey)+4);
      HAL_UART_Transmit(&huart1, transmissionBuffer, sizeof(u8RSAPublicKey)+4, HAL_MAX_DELAY);

     HAL_Delay(1000);
//
////      returnPrivateKey(returnBuffer, 16);
//      wc_AesEcbEncrypt(&aes, returnBuffer, returnBuffer, WC_AES_BLOCK_SIZE);
//      memcpy(transmissionBuffer, prepareTransmission(returnBuffer, 16), 20);
//      HAL_UART_Transmit(&huart1, transmissionBuffer, 20, HAL_MAX_DELAY);
//
//      HAL_Delay(1000);

      memcpy(transmissionBuffer, prepareTransmission((uint8_t *)dummyFunctionDataArray, 208, PAYLOAD_TRANSMISSION), 240);
	    for(uint8_t i = 0; i < 32; i+=8)
      {
          // SIGN_CHUNK(transmissionBuffer+i, transmissionBuffer+i);
          wc_RsaPrivateDecrypt(transmissionBuffer+i, 8, transmissionBuffer+i, MAX_ENCRYPT_SIZE, &rsaKey);
      }
      HAL_UART_Transmit(&huart1, transmissionBuffer, 240, HAL_MAX_DELAY);

      HAL_Delay(1000);
      
      //AES encryption
      uint8_t AES_transmission_array[240] = {0x00};
      memcpy(AES_transmission_array, transmissionBuffer, 240);
      for(uint8_t i = 0; i < 240; i+=16)
      {
        wc_AesEcbEncrypt(&aes, AES_transmission_array + i, AES_transmission_array + i, WC_AES_BLOCK_SIZE);
      }

      HAL_UART_Transmit(&huart1, AES_transmission_array, 240, HAL_MAX_DELAY);

      HAL_Delay(1000);
      //RSA encryption
     uint8_t RSA_transmission_array[240] = {0x00};
     memcpy(RSA_transmission_array, transmissionBuffer, 240);
     for(uint8_t i = 0; i < 240; i+=8)
     {
        //  RSA_ENCRYPTION_IF(RSA_transmission_array + i, RSA_transmission_array + i);
         wc_RsaPublicEncrypt(RSA_transmission_array + i, 8, RSA_transmission_array + i, 8, &rsaKey, &rng);
     }

     HAL_UART_Transmit(&huart1, RSA_transmission_array, 240, HAL_MAX_DELAY);
     HAL_Delay(1000);
      /* ..... Perform your action ..... */
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
    wc_AesFree(&aes);
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
   */
  HAL_PWREx_DisableUCPDDeadBattery();

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_14B;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief HASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_HASH_Init(void)
{

  /* USER CODE BEGIN HASH_Init 0 */

  /* USER CODE END HASH_Init 0 */

  /* USER CODE BEGIN HASH_Init 1 */

  /* USER CODE END HASH_Init 1 */
  hhash.Init.DataType = HASH_DATATYPE_8B;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HASH_Init 2 */

  /* USER CODE END HASH_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = {0};

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x7;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if (HAL_SPIEx_SetConfigAutonomousMode(&hspi1, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
//
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */
//
  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
//
  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief BSP Push Button callback
  * @param Button Specifies the pressed button
  * @retval None
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  if (Button == BUTTON_USER)
  {
    BspButtonState = BUTTON_PRESSED;
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
