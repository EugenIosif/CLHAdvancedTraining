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
#include "adc.h"
#include "crc.h"
#include "hash.h"
#include "icache.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <stm32u5xx_hal_def.h>
#include "memController.h"
// #include "spif.h"
// #include "z_flash_W25QXXX.h"

#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define REPLACEWITHENCRYPTEDDATA_FUNCLEN 240
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


#define PAGE_SIZE 4096

#define DBG(...) printf(__VA_ARGS__);\

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

/* USER CODE BEGIN PV */
W25QXX_HandleTypeDef w25qxx; // Handler for all w25qxx operations!


/* USER CODE BEGIN PV */
UART_HandleTypeDef huart1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */

static void MX_USART1_UART_Init(void);
uint32_t computeHash (const uint8_t * bytes, size_t numberOfBytes);
uint8_t * prepareTransmission(uint8_t * transmissionBuffer, uint8_t size);
void simpleXORencrypt (uint8_t * bufferToEncrypt, uint8_t size);
HAL_StatusTypeDef ComputeSHA256WithHAL(uint32_t startAddress, uint32_t length, uint8_t *outputHash);
void bytes_to_hex_string(uint8_t * inbuff, uint8_t size, uint8_t * outbuff);



void Read_device_ID (void);

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

HAL_StatusTypeDef ComputeSHA256WithHAL(uint32_t startAddress, uint32_t length, uint8_t *outputHash)
{
    HAL_StatusTypeDef status;
    memset(outputHash, 0x00, 32);

    status = HAL_HASHEx_SHA256_Start(&hhash, (uint8_t*)startAddress, length, outputHash, HAL_MAX_DELAY);
    return status;
}



uint8_t * prepareTransmission(uint8_t * inputBuffer, uint8_t size)
{
  static uint8_t buffer[TRANSMISSION_BYTE_LEN];
  //reset the previous content of buffer
  memset(buffer, 0x00, sizeof(buffer));
  if(inputBuffer != NULL && size > 0)
  {
    if(size == 16)
    {
      //we are here because we are trying to transmit a key to the other side. 
      //Hence the 16 bytes -> only the 4 byte hash needed.
      uint32ToBytes tempValue;
      tempValue.value = 0;
      tempValue.value = computeHash(inputBuffer, size);
      memcpy(&buffer[0], tempValue.bytes, 4);
      memcpy(&buffer[4], inputBuffer, size);
    }
    else
    {
      //we are here because we are trying to transmit The contents of a function to the other side.
      //SHA256 is used for this
      uint8_t ouputHash[32];
      memset(ouputHash, 0x00, 32);
      ComputeSHA256WithHAL((uint32_t)replaceWithEncryptedData, 208, ouputHash);

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

//void executeDiffieHellman(void)
//{
//    uint8_t uart_data_tx[UART_DATA_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//    uint8_t uart_data_rx[UART_DATA_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//
//    while(1)
//    {
//        if(state == IDLE)
//        {
//            uart_data_tx[0] = EXCHANGEINITIALIZATION; //signal the start of the exchange
//            HAL_UART_Transmit(&huart1, (uint8_t*)uart_data_tx, UART_DATA_LENGTH, HAL_MAX_DELAY);
//            BspButtonState = BUTTON_RELEASED;
//            state = WAITINGFORSECRET;
//        }
//        else if(state == WAITINGFORSECRET)
//        {
//            HAL_UART_Receive(&huart1, (uint8_t*)uart_data_rx, UART_DATA_LENGTH, HAL_MAX_DELAY);
//            if(uart_data_rx[0] == SECRETRECEIVED)
//            {
//                uint8_t privIntermediaryArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//                uint8_t nArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//
//                u64_to_u8_array(DH_n, nArr);
//                u64_to_u8_array(DH_myPrivateIntermediary, privIntermediaryArr);
//                uint8_t shared_key_arr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//
//                COMPUTE_DH_KEY(&uart_data_rx[1], shared_key_arr, privIntermediaryArr, nArr);
//                uint8_t keyBuffer[16];
//                memset(keyBuffer, 0x00, 16);
//                for(uint8_t i = 0; i < 16; i+=2)
//                {
//                    keyBuffer[i] = shared_key_arr[i];
//                    keyBuffer[i+1] = shared_key_arr[7-i];
//                }
//                WRITE_AES_KEY(keyBuffer);
//                state = NEGOTIATING;
//            }
//        }
//        else if(state == NEGOTIATING)
//        {
//            //Transition logic for NEGOCIATING
//            // uint32_t dh_value = simple_rsa_encrypt(e, mypublickey, n);
//            uint8_t temporaryU64[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//            uint8_t eArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//            uint8_t nArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//            uint8_t privIntArr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//
//            u64_to_u8_array(DH_e, eArr);
//            u64_to_u8_array(DH_n, nArr);
//            u64_to_u8_array(DH_myPrivateIntermediary, privIntArr);
//            COMPUTE_DH_KEY(eArr, temporaryU64, privIntArr, nArr);
//
//            uart_data_tx[0] = SECRETTANSMISSION;
//            memcpy(uart_data_tx+1, temporaryU64, 8);
//            // uint64_t dh_value = u8_array_to_u64(temporaryU64);
//
//            // u64_to_u8_array(dh_value, uart_data_tx+1);
//            HAL_UART_Transmit(&huart1, (uint8_t*)uart_data_tx, UART_DATA_LENGTH, HAL_MAX_DELAY);
//
//            state = CLOSED;
//        }
//
//        else if(state == CLOSED)
//        {
//            state = IDLE;
//            break;
//        }
//        else
//        {
//            state = IDLE;
//            break;
//        }
//    }
//}

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
  
  /* -- Sample board code to switch on leds ---- */
  BSP_LED_On(LED_GREEN);
  BSP_LED_On(LED_BLUE);
  BSP_LED_On(LED_RED);
  
  //config for SPIF
  SPIF_HandleTypeDef spifHandle;
  spifHandle.Inited = 0;
  GPIO_TypeDef *localGPIO = GPIOD;
  SPIF_Init (&spifHandle, &hspi1, localGPIO, GPIO_PIN_14);

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
      /* ..... Perform your action ..... */

      // uint8_t buffer[8]= {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
      // SPIF_WriteSector (&spifHandle, 1 , buffer, 8, 0);
      // uint8_t readdata[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      // SPIF_ReadSector (&spifHandle, 1, readdata, 8, 0);
      // printf("Data read from SPI flash: %02x\n\r", readdata[7]);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
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
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_0;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV4;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
