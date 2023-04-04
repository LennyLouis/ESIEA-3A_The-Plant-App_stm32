/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <retarget.h>
#include <getch.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char *message;
uint8_t plant_hum;
uint8_t plant_bright;
uint8_t plant_temp;
uint8_t esp_status = 0;
uint8_t water_status = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief waterPlant - Active le transistor pour arroser la plante et affiche un message de confirmation sur la console série.
 *
 * @return void
 */
void waterPlant() {
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
	printf("Arrosage en cours...\r\n");
	HAL_Delay(4000);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0);
	printf("Arrosage effectué\r\n");
}

/**
 * @brief HAL_UART_RxCpltCallback - Fonction de callback appelée lorsque des données ont été reçues sur le port UART. Traite les données reçues en fonction de leur contenu. En cours de développement.
 *
 * @param *huart: pointeur vers le port huart
 *
 * @return void
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	printf("it works");
	// Traitement du message reçu
	if (strstr((char*) message, "restartModule") != NULL) {
		// Instructions pour redémarrer le module
		printf("Redémarrage");
	} else if (strstr((char*) message, "waterPlant") != NULL) {
		// Instructions pour arroser la plante
		waterPlant();
	}

	// Réinitialisation du message
	memset(message, 0, sizeof(message));

	// Préparation à la réception du prochain message
	HAL_UART_Receive_IT(&huart1, (uint8_t*) message, sizeof(message));
}

/**
 @brief Affiche le menu principal du programme MyGarden.
 Affiche les options disponibles pour l'utilisateur.
 Pour afficher la température appuyer sur 't'.
 Pour afficher l'humidité de la terre appuyer sur 'h'.
 Pour afficher le taux d'ensoleillement appuyer sur 'l'.
 Pour arroser la plante appuyer sur 'e'.
 */
void showMainMenu() {
	printf("\r\nMyGarden\r\n");
	printf("Pour afficher la température appuie sur : t\r\n");
	printf("Pour afficher l'humidité de la terre appuie sur : h\r\n");
	printf("Pour afficher le taux d'ensoleillement appuie sur : l\r\n");
	printf("Pour arroser la plante appuie sur : e\r\n");
}

/* USER CODE END 0 */

/**
 * @brief main - Fonction principale du programme.
 *
 * @return int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); // flag receive
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TC); // flat Tx_IT

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_ADC_Init();
	MX_TIM6_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	RetargetInit(USART2);
	getchInit();
	LL_USART_EnableIT_RXNE(USART2);

	LL_ADC_Enable(ADC1);
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
	LL_ADC_PATH_INTERNAL_TEMPSENSOR | LL_ADC_PATH_INTERNAL_VREFINT);
	printf("\x1B[2J"); //Clear console (putty)
	// Affiche les possibilités d'action
	showMainMenu();
	HAL_TIM_Base_Start(&htim6);

	MX_GPIO_Init();

	HAL_UART_Receive_IT(&huart1, (uint8_t*) message, sizeof(message));
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		if (kbhit()) //if one char received on USART2
		{
			char c = getch();
			if (c == 'h') {
				// humidité terre
				printf("\r\n");
				printf("Vous avez choisi h\r\n");
				printf("Humidité terre: %d%%\r\n", (int) plant_hum);

			} else if (c == 'l') {
				// lumiere
				printf("\r\n");
				printf("Vous avez choisi l\r\n");
				printf("Lumière: %d%%\r\n", plant_bright);

			} else if (c == 'e') {
				// envoie de l'eau
				printf("\r\n");
				printf("Vous avez choisi e\r\n");
				char data[100];
				sprintf(data, "{\"lastWatering\": \"now\"}\r\n");
				HAL_UART_Transmit(&huart1, data, sizeof(data), 150);
				waterPlant();

			} else if (c == 't') {
				// température
				printf("\r\n");
				printf("Vous avez choisi t\r\n");
				printf("Température: %d°C\r\n", plant_temp);

			} else {
				showMainMenu();

			}
		}
		if (esp_status) {
			esp_status = 0;
			char data[100];
			// Préparation du json à envoyer au broker mqtt
			sprintf(data,
					"{\"temperature\": \"%d\", \"dirtHumidity\": \"%d\",\"brightness\": \"%d\"}\r\n",
					plant_temp, plant_hum, plant_bright);
			HAL_UART_Transmit(&huart1, data, sizeof(data), 150);
		}
		if (water_status) {
			printf("\r\n");
			printf("Arrosage automatique car taux d'humidité < 30%% (taux actuel: %d)\r\n",
					water_status);
			water_status = 0;
			char data[100];
			sprintf(data, "{\"lastWatering\": \"now\"}\r\n");
			HAL_UART_Transmit(&huart1, data, sizeof(data), 150);
			waterPlant();
		}
		if (LL_GPIO_IsInputPinSet(B1_GPIO_Port, B1_Pin) == 0) {
			printf("\r\n");
			printf("Bouton pressé, arrosage en cours !\r\n");
			char data[100];
			sprintf(data, "{\"lastWatering\": \"now\"}\r\n");
			HAL_UART_Transmit(&huart1, data, sizeof(data), 150);
			waterPlant();
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
void SystemClock_Config(void) {
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0) {
	}
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	LL_RCC_HSI_Enable();

	/* Wait till HSI is ready */
	while (LL_RCC_HSI_IsReady() != 1) {

	}
	LL_RCC_HSI_SetCalibTrimming(16);
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {

	}
	LL_SetSystemCoreClock(16000000);

	/* Update the time base */
	if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
		Error_Handler();
	}
	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);
	LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
