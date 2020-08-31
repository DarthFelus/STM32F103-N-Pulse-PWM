#include "misc.h"                       // Keil::Device:StdPeriph Drivers:Framework
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM





void init_TIM1 (void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	// Конфигурация прескейлера
	TIM1->PSC = 1;
	// Конфигурация периода
	TIM1->ARR = 64;
	// Конфигурация счётчика повторения
	TIM1->RCR = 12;
	// Конфигурация ширины импульса
	TIM1->CCR1 = 32;

	// Выбор коефициента деления Clock Division 1
	// Сброс поля бита коефициента деления тактов
	TIM1->CR1 &= ~ TIM_CR1_CKD;
	// Выбор DIV1 в качестве коефициента деления
	TIM1->CR1 |= TIM_CKD_DIV1;
	// Выбор счета вверх для TIM1. Сброс полей выбора режима:
	TIM1->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	// Выбор счета вверх:
	TIM1->CR1 |= TIM_CounterMode_Up;
	// Установка режима PWM1. Сброс бит режима Output Compare:
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;
	// Выбор output compare mode 1:
	TIM1->CCMR1 |= TIM_OCMode_PWM1;

	/****************** Конфигурация OPM ********************/

	// Выбор One Pulse Mode (OPM):
	TIM1->CR1 |= TIM_CR1_OPM;
	
	/********************************************************/

	/********* Конфигурация slave-режима запуска ************/

	// Выбор сигнала TIM_TS_ITR1 в качестве триггера для TIM1:
	TIM1->SMCR &= ~TIM_SMCR_TS;
	TIM1->SMCR |= TIM_TS_ITR1;

	// Выбор Slave Mode:
	TIM1->SMCR &= ~TIM_SMCR_SMS;
	TIM1->SMCR |= TIM_SlaveMode_Trigger;

	/********************************************************/

	// Разрешение предзагрузки Output Compare:
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
	// Установка бита UG, чтобы разрешить UEV:
	TIM1->EGR |= TIM_EGR_UG;
	// Разрешение основного выхода TIM1:
	TIM1->BDTR |= TIM_BDTR_MOE;
	// Выбор активного нуля в качестве выходной полярности.
	// Сброс уровня Output Polarity:
	TIM1->CCER &= ~TIM_CCER_CC1P;
	// Установка выхода на низком уровне:
	TIM1->CCER |= TIM_OCPolarity_Low;
	// Разрешение выхода CC1 на высоком уровне:
	TIM1->CCER |= TIM_CCER_CC1E;
	// Разрешение счетчика TIM1:
	TIM1->CR1 |= TIM_CR1_CEN;
} 
void init_TIM2 (void)
	{
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 
	// Вычисление настройки для прескалера
	TIM2->ARR=800-1;
	// Конфигурирование прескалера таймера:
	TIM2->PSC = 1;
	// Выбор коэффициента деления тактов (Clock Divison) = 1.
	// Сброс битового поля деления частоты тактов:
	TIM2->CR1 &= ~ TIM_CR1_CKD;
	// Выбор DIV1 в качестве коэффициента деления тактов:
	TIM2->CR1 |= TIM_CKD_DIV1;
	// Выбор счета вверх. Сброс полей выбора режима таймера:
	TIM2->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	// Режим счета вверх:
	TIM2->CR1 |= TIM_CounterMode_Up;
	
	/***** Конфигурация режима Master для обновления триггера ****/
	// Срабатывание триггера TIM2 для обновления TIM1 Slave:
	TIM1->CR2 &= ~ TIM_CR2_MMS;
	TIM2->CR2 |= TIM_TRGOSource_Update;
	/*************************************************************/
	
	// Разрешение счетчика таймера TIM2:
	TIM2->CR1 |= TIM_CR1_CEN;
}
void GpioInit (void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// Инициализация порта PA8	
	GPIO_InitTypeDef Output;
	// Настройка порта PA8	
	Output.GPIO_Pin = GPIO_Pin_8;
	Output.GPIO_Mode = GPIO_Mode_AF_PP;
	Output.GPIO_Mode = GPIO_Mode_IPU;
	Output.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &Output);		
}
	
void SetSystemClockto64 (void)
{
	// Включить HSE
	
	RCC->CR |= RCC_CR_HSEON;
	// Ждать пока HSE не будет готова
	while((RCC->CR & RCC_CR_HSERDY) == 0) {}
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1  		 |  //HLCK = SYSCLK
					 RCC_CFGR_PPRE2_DIV1 		 |	//PCLK2 = HCLK
					 RCC_CFGR_PPRE1_DIV2;			//PCLK1 = HCLK
		// Обнулить значение PLLMULL
		RCC->CFGR &= ~RCC_CFGR_PLLMULL;
		FLASH->ACR |= FLASH_ACR_PRFTBE;
		FLASH->ACR |= FLASH_ACR_LATENCY_2;
		RCC->CFGR |= RCC_CFGR_PLLSRC_HSE 		 |  //PLLSRC = HSE
					 RCC_CFGR_PLLMULL8;				//8 * 8 = 64 MHz
		// Включить PLL
		RCC->CR |= RCC_CR_PLLON;
		// Ждём пока PLL не будет готова
		while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}
		// Выбираем PLL в качестве источника тактирования
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		// Ждём пока PLL не будет в качестве источника тактирования
		while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1) {}
		
}	
int main ()
{
	SetSystemClockto64();
	
	GpioInit();
	
	init_TIM1();
	init_TIM2();
		
}
