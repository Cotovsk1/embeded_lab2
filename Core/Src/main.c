
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
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum
{
    ALGO_BLINK_ALL = 0,
    ALGO_SEQUENTIAL,
    ALGO_REVERSE,
    ALGO_PAIRS,
    ALGO_COUNT
} AlgorithmType;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define LED_ALL (GREEN_Pin | ORANGE_Pin | RED_Pin | BLUE_Pin)

#define BUTTON_DEBOUNCE 200
#define AUTO_SWITCH_INTERVAL 3000
#define SEQUENCE_INTERVAL 500

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

static volatile AlgorithmType current_algorithm = ALGO_BLINK_ALL;

static uint8_t sequence_step = 0;

static uint32_t last_button_time = 0;
static uint32_t last_auto_switch_time = 0;
static uint32_t last_sequence_time = 0;

static volatile uint8_t button_pressed = 0;

static uint8_t use_auto_switch = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */

static void ProcessLEDs(void);
static void SwitchAlgorithm(void);
static void AutoSwitchCheck(void);
static void SetAllLEDs(GPIO_PinState state);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void SetAllLEDs(GPIO_PinState state)
{
    HAL_GPIO_WritePin(GPIOD, LED_ALL, state);
}

static void SwitchAlgorithm(void)
{
    current_algorithm++;

    if (current_algorithm >= ALGO_COUNT)
    {
        current_algorithm = ALGO_BLINK_ALL;
    }

    sequence_step = 0;

    last_sequence_time = HAL_GetTick();

    SetAllLEDs(GPIO_PIN_RESET);
}

static void AutoSwitchCheck(void)
{
    uint32_t current_time = HAL_GetTick();

    if (use_auto_switch &&
        (current_time - last_auto_switch_time >= AUTO_SWITCH_INTERVAL))
    {
        SwitchAlgorithm();

        last_auto_switch_time = current_time;
    }
}

static void ProcessLEDs(void)
{
    uint32_t current_time = HAL_GetTick();

    switch (current_algorithm)
    {
        /* Усі світлодіоди блимають одночасно */
        case ALGO_BLINK_ALL:

            if ((current_time % 1000) < 500)
            {
                SetAllLEDs(GPIO_PIN_SET);
            }
            else
            {
                SetAllLEDs(GPIO_PIN_RESET);
            }

            break;

        /* Послідовне ввімкнення зліва направо */
        case ALGO_SEQUENTIAL:

            if (current_time - last_sequence_time >= SEQUENCE_INTERVAL)
            {
                last_sequence_time = current_time;

                SetAllLEDs(GPIO_PIN_RESET);

                switch (sequence_step)
                {
                    case 0:
                        HAL_GPIO_WritePin(GPIOD, GREEN_Pin, GPIO_PIN_SET);
                        break;

                    case 1:
                        HAL_GPIO_WritePin(GPIOD, ORANGE_Pin, GPIO_PIN_SET);
                        break;

                    case 2:
                        HAL_GPIO_WritePin(GPIOD, RED_Pin, GPIO_PIN_SET);
                        break;

                    case 3:
                        HAL_GPIO_WritePin(GPIOD, BLUE_Pin, GPIO_PIN_SET);
                        break;
                }

                sequence_step = (sequence_step + 1) % 4;
            }

            break;

        /* Послідовне ввімкнення справа наліво */
        case ALGO_REVERSE:

            if (current_time - last_sequence_time >= SEQUENCE_INTERVAL)
            {
                last_sequence_time = current_time;

                SetAllLEDs(GPIO_PIN_RESET);

                switch (sequence_step)
                {
                    case 0:
                        HAL_GPIO_WritePin(GPIOD, BLUE_Pin, GPIO_PIN_SET);
                        break;

                    case 1:
                        HAL_GPIO_WritePin(GPIOD, RED_Pin, GPIO_PIN_SET);
                        break;

                    case 2:
                        HAL_GPIO_WritePin(GPIOD, ORANGE_Pin, GPIO_PIN_SET);
                        break;

                    case 3:
                        HAL_GPIO_WritePin(GPIOD, GREEN_Pin, GPIO_PIN_SET);
                        break;
                }

                sequence_step = (sequence_step + 1) % 4;
            }

            break;

        /* Почергове ввімкнення пар світлодіодів */
        case ALGO_PAIRS:

            if ((current_time % 1000) < 300)
            {
                SetAllLEDs(GPIO_PIN_RESET);

                HAL_GPIO_WritePin(GPIOD,
                    GREEN_Pin | ORANGE_Pin,
                    GPIO_PIN_SET);
            }
            else if ((current_time % 1000) < 600)
            {
                SetAllLEDs(GPIO_PIN_RESET);

                HAL_GPIO_WritePin(GPIOD,
                    RED_Pin | BLUE_Pin,
                    GPIO_PIN_SET);
            }
            else
            {
                SetAllLEDs(GPIO_PIN_RESET);
            }

            break;

        default:

            SetAllLEDs(GPIO_PIN_RESET);

            break;
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

    /* MCU Configuration------------------------------------------------------*/

    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */

    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */

    MX_GPIO_Init();

    /* USER CODE BEGIN 2 */

    last_auto_switch_time = HAL_GetTick();
    last_sequence_time = HAL_GetTick();

    SetAllLEDs(GPIO_PIN_RESET);

    /* USER CODE END 2 */

    /* Infinite loop */

    /* USER CODE BEGIN WHILE */

    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        /* Обробка натискання кнопки */

        if (button_pressed)
        {
            button_pressed = 0;

            uint32_t current_time = HAL_GetTick();

            if (current_time - last_button_time >= BUTTON_DEBOUNCE)
            {
                SwitchAlgorithm();

                last_button_time = current_time;
                last_auto_switch_time = current_time;
            }
        }

        /* Автоматичне перемикання алгоритмів */

        AutoSwitchCheck();

        /* Виконання поточного алгоритму */

        ProcessLEDs();

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

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
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

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Початковий стан світлодіодів */

    HAL_GPIO_WritePin(GPIOD, LED_ALL, GPIO_PIN_RESET);

    /* Налаштування кнопки PA0 */

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Налаштування світлодіодів PD12-PD15 */

    GPIO_InitStruct.Pin = LED_ALL;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* EXTI interrupt initialization */

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    /* USER CODE BEGIN MX_GPIO_Init_2 */

    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* Обробник переривання кнопки */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        button_pressed = 1;
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */

void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */

    __disable_irq();

    while (1)
    {
    }

    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
  * @brief Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */

void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */

    /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */
