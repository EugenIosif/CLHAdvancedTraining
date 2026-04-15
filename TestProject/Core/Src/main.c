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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <stm32u5xx_hal_def.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KEY_SIZE 4
#define MAX_DID_TABLE_SIZE 8
#define CONFIG_BUFFER_SIZE 8
#define DID_ENTRY_SIZE 16
#define DID_ID_SIZE 2
#define NACK false // Negative Response Code for UDS services
#define NUMBER_OF_SERVICES 3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define MODULOMULTIPLICATION(a,b,n) (safe_mult(a,b) % n)

#define SIMPLEMULTIPLICATION(a, b, c) ({    c[i+j]+=a[i]*b[j]; \
                                            c[i+j+1] += c[i+j]/10; \
                                            c[i+j]%=10; \
                                        })

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;
__IO uint32_t BspButtonState = BUTTON_RELEASED;
ADC_HandleTypeDef hadc1;

HASH_HandleTypeDef hhash;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

typedef bool (*UDSServiceHandler)(uint8_t *data, uint16_t len);
typedef struct
{
    uint16_t did_id;
    unsigned char data[DID_ENTRY_SIZE];
    bool is_encrypted;
} DiagnosticData;

uint8_t stored_ecu_key[KEY_SIZE] = {0xDE, 0xAD, 0xBE, 0xEF};
uint16_t did_ids_to_be_updated[4] = {0x1001, 0x1003, 0x1005, 0x1007};
uint8_t key = 3; // Key for data encryption

DiagnosticData did_table[MAX_DID_TABLE_SIZE] = {
    {0x1001, "VIN123456789", false},
    {0x1002, "FW_VER_1.0.0", false},
    {0x1003, "HW_VER_A", false},
    {0x1004, "PROD_DATE_2020", false},
    {0x1005, "SUPPORT_24/7", false},
    {0x1006, "WARRANTY_3YRS", false},
    {0x1007, "RECALL_NONE", false},
    {0x1008, "OWNER_JOHN", false}
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_HASH_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

bool verify_diagnostic_key(uint8_t *provided_key, uint16_t len);
bool read_did_entry(uint8_t *data, uint16_t len);
bool update_did_entry(uint8_t *raw_payload, uint16_t payload_len);

void flipBits(uint8_t *data, size_t len);
bool secure_compare(uint8_t *a, uint8_t *b, size_t len);
uint64_t simple_rsa_encrypt(uint64_t message, uint64_t e, uint64_t n);

uint16_t storeIntInInverseOrderArray(uint64_t number, uint8_t* array);
uint64_t storeArrayInNumber(uint8_t* array, uint16_t size);
uint64_t safe_mult(uint64_t a, uint64_t b);
uint64_t safe_mult_mod(uint64_t a, uint64_t b, uint64_t n);
HAL_StatusTypeDef ComputeSHA256FromMemory(uint32_t startAddress, uint32_t length, uint8_t *outputHash);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

HAL_StatusTypeDef ComputeSHA256FromMemory(uint32_t startAddress, uint32_t length, uint8_t *outputHash)
{
    HAL_StatusTypeDef status;
    uint32_t currentAddress = startAddress;

    status = HAL_HASHEx_SHA256_Start(&hhash, (uint8_t*)currentAddress, length, outputHash, HAL_MAX_DELAY);
    status = HAL_HASHEx_SHA256_Accmlt(&hhash, (uint8_t*)currentAddress, length);

    uint8_t outputHashnew[32]; // SHA-256 produces a 32-byte hash

    memcpy(outputHashnew, outputHash, 32);

    status = HAL_HASHEx_SHA256_Accmlt_End(&hhash, (uint8_t*)currentAddress, length,outputHash,HAL_MAX_DELAY);

    // Copy the final hash result back to outputHashnew or outputHash
    memcpy(outputHash, outputHashnew, 32);

    return status;
}


UDSServiceHandler service_handlers[3] = {
    (UDSServiceHandler)verify_diagnostic_key,
    (UDSServiceHandler)read_did_entry,
    (UDSServiceHandler)update_did_entry};

inline void flipBits(uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        data[i] ^= 0xFF; // Flip all bits in the byte
    }
}

inline void simpleMultuplication(uint8_t *arr1, uint8_t *arr2, uint8_t *ans, int i, int j) 
{
    ans[i+j] += arr1[i] * arr2[j];
    ans[i+j+1] += ans[i+j]/10;
    ans[i+j] %= 10;
}

uint16_t storeIntInInverseOrderArray(uint64_t number, uint8_t* array) {
    uint16_t size = 0;
    // printf("For number: %llu ", number);
    while (number > 0) {
        *(array + size) = number % 10; // Store the least significant digit first
        number /= 10; // Remove the last digit
        size++;
    }
    // printf("Size of array: %d\n", size);
    return size; // Return the number of digits stored
}

uint64_t storeArrayInNumber(uint8_t* array, uint16_t size) {
    uint64_t number = 0;
    for (int i = size - 1; i >= 0; i--) {
        number = number * 10;
        number += *(array + i); // Reconstruct the number
    }
    return number;
}

uint64_t safe_mult(uint64_t a, uint64_t b)
{
    uint8_t arr1[20] = {0}, arr2[20] = {0}, ans[40] = {0};  // size of (arr1_size + arr2_size)

    uint8_t size1 = 0 , size2 = 0;

    size1 = storeIntInInverseOrderArray(a, &arr1[0]);
    size2 = storeIntInInverseOrderArray(b, &arr2[0]);

    for(int i=0;i<size1;i++)
    {
        for(int j=0;j<size2;j++)
        {
//            simpleMultuplication(&arr1[0], &arr2[0], &ans[0], i, j);
             SIMPLEMULTIPLICATION(arr1, arr2, ans);
        }
    }

    uint64_t product = 0;
    product = storeArrayInNumber(&ans[0], size1+size2);
    // printf("Product: %llu\n", product);

    return product;
}

uint64_t safe_mult_mod(uint64_t a, uint64_t b, uint64_t n)
{
    return safe_mult(a, b) % n;
}

bool secure_compare(uint8_t *a, uint8_t *b, size_t len)
{
    uint8_t result = 0;
    for (size_t i = 0; i < len; i++)
    {
        result |= (a[i] ^ b[i]); // XOR stays 0 if bytes match, becomes non-zero if they don't
    }
    return (result == 0); // Only checks the final accumulated result
}


uint64_t simple_rsa_encrypt(uint64_t message, uint64_t e, uint64_t n)
{
    uint64_t result = 1;
    message = message % n;
    while (e > 0) {
        if (e % 2 == 1){
            result = MODULOMULTIPLICATION(result, message, n);
        }
        e = e >> 1;
        message = MODULOMULTIPLICATION(message, message, n);
    }
    return result;
}

uint64_t concatenateBitRepresentationIntoDecimal(char *str) {
    uint64_t returnValue = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        returnValue = (returnValue << 8) + (uint8_t)str[i];
        printf("Value String %d \n\r", (uint8_t)str[i] );
    }
    return returnValue;
}

void stringFromEncryptedDecimal(uint64_t decimal, char *str, int size) {
    int index = size - 1; // Start from the end of the string
    while (decimal > 0 && index >= 0) {
        str[index--] = (char)(decimal & 0xFF); // Get the last 8 bits
        decimal >>= 8; // Shift right by 8 bits to get the next character
    }
    str[index] = '\0'; // Null-terminate the string
}


bool verify_diagnostic_key(uint8_t *provided_key, uint16_t len)
{
    if (len != KEY_SIZE)
        return NACK;

    bool returnValue = NACK;

    for (int i = 0; i < KEY_SIZE; i++)
    {
        if (secure_compare(&provided_key[i], &stored_ecu_key[i], KEY_SIZE))
        {
            returnValue = true;
        }
    }
    return returnValue;
}

// --- 2. OOB READ (UDS Service 0x22) ---
// Vulnerable: Trusts the caller to have validated the index
bool read_did_entry(uint8_t *data, uint16_t len)
{
    if ((len > DID_ID_SIZE) && (len <= DID_ENTRY_SIZE + DID_ID_SIZE))
    {
        static DiagnosticData entry;

        memcpy(&entry.did_id, &data[0], sizeof(entry.did_id)); // Read the DID ID from the payload

        if (entry.did_id >= did_table[0].did_id && entry.did_id <= MAX_DID_TABLE_SIZE + did_table[0].did_id)
        {
            for (int i = 0; i < MAX_DID_TABLE_SIZE; i++)
            {
                if (memcmp(&did_table[i].did_id, &entry.did_id, sizeof(entry.did_id)) == 0)
                {
                    memcpy(entry.data, did_table[i].data, DID_ENTRY_SIZE);
                    break;
                }
            }
        }
        memcpy(data, &entry, sizeof(entry)); // Copy the entry back to the caller's buffer
    }
    return true;
}


bool update_did_entry(uint8_t *data, uint16_t payload_len)
{
    bool returnValue = NACK;
    DiagnosticData entry;
    if ((payload_len > DID_ID_SIZE) && (payload_len <= DID_ENTRY_SIZE + DID_ID_SIZE))
    {
        memcpy(&entry.did_id, &data[0], sizeof(entry.did_id)); // Read the DID ID from the payload
        if (entry.did_id >= did_table[0].did_id && entry.did_id <= MAX_DID_TABLE_SIZE + did_table[0].did_id)
        {
            memcpy(entry.data, &data[DID_ID_SIZE], payload_len - DID_ID_SIZE);
            for (int i = 0; i < MAX_DID_TABLE_SIZE; i++)
            {
                if (memcmp(&did_table[i].did_id, &entry.did_id, sizeof(entry.did_id)) == 0)
                {
                    uint8_t counter = 0;
                    if (!did_table[i].is_encrypted)
                    {
                        did_table[i].is_encrypted = true;

                        while (did_table[i].data[counter] != '\0')
                        {
                            flipBits((uint8_t *)&did_table[i].data[counter], sizeof(did_table[i].data[counter]));
                            counter++;
                        }
                    }
                    else
                    {
                        did_table[i].is_encrypted = false;
                        while (did_table[i].data[counter] != '\0')
                        {
                            // flipBits((uint8_t *)&entry.data[counter], 1); // Simulate a bit flip in the entry data
                            flipBits(&did_table[i].data[counter], sizeof(did_table[i].data[counter]));
                            counter++;
                        }
                    }

                    memcpy(&did_table[i].data, entry.data, DID_ENTRY_SIZE);
                    returnValue = true;
                    break;
                }
            }
        }
    }
    return returnValue;
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
  /* USER CODE BEGIN 2 */

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
  printf("Welcome to STM32 world !\n\r");

  uint8_t key_attempt[KEY_SIZE] = {0xDE, 0xAD, 0xBE, 0xEF}; // Correct key
  if (service_handlers[0](key_attempt, KEY_SIZE))
  {
      printf("Key verified successfully.\n\r");
  }
  else
  {
      printf("Key verification failed.\n\r");
  }

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

	  char *str = "HELLO WORLD!";
	  char str_encrypt[15];
	  char str_decrypt[15];
	  printf("\n\n\rString: %s\n\r", str);

		uint64_t decimalRepresentation = concatenateBitRepresentationIntoDecimal(str);;
		printf("Decimal representation: %llu data\n\r", decimalRepresentation);

		for (int i = 0; str[i] != '\0'; i++ )
		{
			  int c = 1;
			  for (int i = 0; i < 3; i++)
			  {
			    c = MODULOMULTIPLICATION(c, str[i], 493);
			  }
			printf("Encrypt value %d from %c decimal %d \n\r",str_encrypt[i], str[i], str[i]);
		}

		for (int i = 0; i < 12; i++ )
		{
//			str_decrypt[i] = simple_rsa_encrypt(str_encrypt[i], 75, 493);
			  int c = 1;
			  for (int j = 0; j < 75; i++)
			  {
			    c = MODULOMULTIPLICATION(c, str_encrypt[i], 493);
			  }
			printf("Decrypt value %d to %c from decimal %d \n\r",str_decrypt[i], str_decrypt[i], str_encrypt[i]);
		}

		uint64_t encryptedUint64 = simple_rsa_encrypt(decimalRepresentation, 65537, 493071499771);
		printf("Encrypted uint64: %llu\n\r", encryptedUint64);

		uint64_t decryptedUint64 = simple_rsa_encrypt(encryptedUint64, 266513779457, 493071499771);
		printf("Decrypted uint64: %llu\n\r", decryptedUint64);

		char decrypted_str[9];
		stringFromEncryptedDecimal(decryptedUint64, &decrypted_str[0], (sizeof(decrypted_str)/sizeof(decrypted_str[0])));
		printf("Decrypted string: %s\n\r", decrypted_str);


      /* ..... Perform your action ..... */
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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
  hhash.Init.DataType = HASH_DATATYPE_32B;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
