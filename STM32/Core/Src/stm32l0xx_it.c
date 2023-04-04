/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32l0xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32l0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t g_rxCarUSART2;
uint32_t cpt1 = 0;
uint32_t cpt2 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 @brief Récupère la valeur du capteur d'humidité de la terre et la stocke dans plant_hum.
 La formule de conversion de la valeur brute en pourcentage d'humidité est la suivante :
 plant_hum = ((((float) LL_ADC_REG_ReadConversionData12(ADC1) - 1300) / (3500 - 1300)) - 1) * -100;
 */
void getHumidity() {
	// Récupération de la valeur du capteur d'humidité de la terre
	LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_7);
	LL_ADC_REG_StartConversion(ADC1);
	while (!LL_ADC_IsActiveFlag_EOC(ADC1))
		;

	plant_hum = ((((float) LL_ADC_REG_ReadConversionData12(ADC1) - 1300)
			/ (3500 - 1300)) - 1) * -100;
}

/**
 @brief Récupère la valeur du capteur de luminosité et la stocke dans plant_bright.
 La formule de conversion de la valeur brute en pourcentage de luminosité est la suivante :
 plant_bright = (1 - ((float) LL_ADC_REG_ReadConversionData12(ADC1) / 4095)) * 100;
 */
void getBrightness() {
	// Récupération de la valeur du capteur de luminosité
	LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_6);
	LL_ADC_REG_StartConversion(ADC1);
	while (!LL_ADC_IsActiveFlag_EOC(ADC1))
		;

	plant_bright = (1 - ((float) LL_ADC_REG_ReadConversionData12(ADC1) / 4095))
			* 100;
}

/**
 @brief Récupère la valeur du capteur de température et la stocke dans plant_temp.
 La formule de conversion de la valeur brute en degrés Celsius est la suivante :
 plant_temp = (valueInMv - 500) / 10;
 Où valueInMv est la tension mesurée en millivolts.
 */
void getTemperature() {
	// Récupération de la valeur du capteur de température
	LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_4);
	LL_ADC_REG_StartConversion(ADC1);
	while (!LL_ADC_IsActiveFlag_EOC(ADC1))
		;

	uint32_t value = LL_ADC_REG_ReadConversionData12(ADC1);
	uint32_t valueInMv = value * 3300 / 4096;
	plant_temp = (valueInMv - 500) / 10;
}
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */

	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
	/* USER CODE BEGIN SVC_IRQn 0 */

	/* USER CODE END SVC_IRQn 0 */
	/* USER CODE BEGIN SVC_IRQn 1 */

	/* USER CODE END SVC_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
	/* USER CODE BEGIN PendSV_IRQn 0 */

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
	/* USER CODE BEGIN SysTick_IRQn 0 */

	// On exécute le code toute les 1,5 secondes
	if (cpt1 > 1500) {
		// Reinitialisation du timer
		cpt1 = 0;

		// Récupération de la valeur du capteur d'humidité de la terre
		getHumidity();
		// Récupération de la valeur du capteur de luminosité
		getBrightness();
		// Récupération de la valeur du capteur de température
		getTemperature();

		// Changement du flag pour envoyer à l'ESP les valeurs des capteurs
		esp_status = 1;
	} else {
		cpt1++;
	}

	// On exécute le code toute les 10 secondes
	if (cpt2 > 10000) {
		cpt2 = 0;
		// Si l'humidité de la terre < 30%, on arrose la plante
		if (plant_hum < 30) {
			water_status = plant_hum;
		}
	} else {
		cpt2++;
	}

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
 *
 * @note Cette fonction est appelée automatiquement par les librairies HAL quand une interruption USART1 arrive.
 *
 * @retval void
 */
void USART1_IRQHandler(void) {
	/* USER CODE BEGIN USART1_IRQn 0 */

	/* USER CODE END USART1_IRQn 0 */
	HAL_UART_IRQHandler(&huart1);
	/* USER CODE BEGIN USART1_IRQn 1 */

	/* USER CODE END USART1_IRQn 1 */
}

/**
 * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
 */
void USART2_IRQHandler(void) {
	/* USER CODE BEGIN USART2_IRQn 0 */
	if (LL_USART_IsActiveFlag_RXNE(USART2)) {
		g_rxCarUSART2 = LL_USART_ReceiveData8(USART2);
	}
	/* USER CODE END USART2_IRQn 0 */
	/* USER CODE BEGIN USART2_IRQn 1 */

	/* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void getchInit(void) {
	g_rxCarUSART2 = 255; //no char received
}

//Return  a char received on UART2
// Wait if none received yet
uint8_t getch(void) {
	volatile uint8_t l_car = 1;
	do {
		if (g_rxCarUSART2 == 255) {
			__WFI(); //wait for interrupt (for a key ==> data RX)
		}
	} while (g_rxCarUSART2 == 255); //while no char received

	l_car = g_rxCarUSART2; //save received data
	g_rxCarUSART2 = 255; //clear char received
	return l_car; //return received data
}
//Return 1 if a char received on UART2
// else 0
int kbhit(void) {
	if (g_rxCarUSART2 == 255)
		return 0;
	else
		return 1;
}

/* USER CODE END 1 */
