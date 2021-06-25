#include "stm32f4xx.h"

int ticks = 0;
int Enc_Counter = 1;


void init_gpio (void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	
	// IN1 установка 01(output mode) PC9
	GPIOC->MODER |= GPIO_MODER_MODE9_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE9_1;
	
	// IN2 установка 01(output mode) PC5
	GPIOC->MODER |= GPIO_MODER_MODE5_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE5_1;
	
	// IN3 установка 01(output mode) PC6
	GPIOC->MODER |= GPIO_MODER_MODE6_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE6_1;
	
	// IN4 установка 01(output mode) PC8
	GPIOC->MODER |= GPIO_MODER_MODE8_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE8_1;
	
	/* 10: Alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODER6_0;
	GPIOA->MODER |= GPIO_MODER_MODER6_1;

	/* 00: No pull-up, pull-down */
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR6_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR6_1;

	/* 0010: AF1 */
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL6_1;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6_2;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6_3;


	/* 10: Alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODER7_0;
	GPIOA->MODER |= GPIO_MODER_MODER7_1;

	/* 00: No pull-up, pull-down */
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR7_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR7_1;

	/* 0010: AF1 */
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL7_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL7_1;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL7_2;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL7_3;
	
}

void enable_step_tim2(){
	//	enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//	set up-count mode
	TIM2->CR1 &= ~TIM_CR1_DIR;

	//	program presceler (Freq_count = Freq_PSC / PSC+1) = 16MHz / (15999+1)  = 1KHz
	TIM2->PSC = 15999;
	//	program auto-reload reg "ARR" (Freq_Event = Freq_count / ARR+1) 
	TIM2->ARR = 1; //	start value, which will be update
	TIM2->CNT = 0;
	//	enable TIM2 interrupt 
	TIM2->DIER |= TIM_DIER_UIE;
	//	enable preload (shadow register)
	TIM2->CR1 |= TIM_CR1_ARPE;
	//	enable counter 
	TIM2->CR1 |= TIM_CR1_CEN;
	
	NVIC_SetPriority(TIM2_IRQn,1);
	NVIC_ClearPendingIRQ(TIM2_IRQn);
	NVIC_EnableIRQ(TIM2_IRQn); 
}

void enable_encoder_tim3 ()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* 01: CC1 channel is configured as input, IC1 is mapped on TI1
	 * 01: CC2 channel is configured as input, IC2 is mapped on TI2 */
	TIM3->CCMR1 |= (TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0);
	TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S_1 | TIM_CCMR1_CC2S_1);

	/* 00: noninverted/rising edge */
	TIM3->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
	TIM3->CCER &= ~(TIM_CCER_CC2NP | TIM_CCER_CC2NP);

	/* 001: Encoder mode 1 - Counter counts up/down on TI2FP1 edge depending on TI1FP2 level */
	TIM3->SMCR |= TIM_SMCR_SMS_0;
	TIM3->SMCR &= ~TIM_SMCR_SMS_1;
	TIM3->SMCR &= ~TIM_SMCR_SMS_2;

	/* 1111: fSAMPLING = fDTS / 32, N = 8 */
	TIM3->CCMR1 |= (TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC1F_2 | TIM_CCMR1_IC1F_3);
	TIM3->CCMR1 |= (TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_3);

	/* Auto-Reload Register (MAX counter number) */
	TIM3->ARR = 30;

	
}

void Enc_Trig_Init(void){

	/* Trigger Edge Detector */

	/* 0000: No filter, sampling is done at fDTS */
	TIM3->SMCR &= ~(TIM_SMCR_ETF);

	/* 100: TI1 Edge Detector (TI1F_ED) */
	TIM3->SMCR &= ~(TIM_SMCR_TS_0 | TIM_SMCR_TS_1);
	TIM3->SMCR |= TIM_SMCR_TS_2;

	/* 1: Trigger interrupt enabled. */
//	TIM3->DIER |= TIM_DIER_TIE;

//	NVIC_EnableIRQ(TIM3_IRQn);
}



//	responsible for Servo
void TIM2_IRQHandler(){
	//	increment of dec. tacts, which needed for step_driver (include reverse)
	if(TIM2->SR & TIM_SR_UIF){
//		ticks++;
		
		if (Enc_Counter > 15 ){
			TIM2->ARR = Enc_Counter - 14;
			ticks++;
		} else if(Enc_Counter > 1){
			TIM2->ARR = Enc_Counter;
			ticks--;
		}
	TIM2->SR &= ~TIM_SR_UIF;
	}
}

void drive( int *ticks) {
		//	fix ticks 
		if(*ticks > 7) *ticks = 0;
		else if (*ticks < 0) *ticks = 7;
	
	switch(*ticks){
	case 0:
		GPIOC->ODR |= GPIO_ODR_OD9;
		GPIOC->ODR &= ~GPIO_ODR_OD5;
		GPIOC->ODR &= ~GPIO_ODR_OD6;
		GPIOC->ODR &= ~GPIO_ODR_OD8;
	break;
	
	case 1:	
		GPIOC->ODR |= GPIO_ODR_OD9;
		GPIOC->ODR |= GPIO_ODR_OD5;
		GPIOC->ODR &= ~GPIO_ODR_OD6;
		GPIOC->ODR &= ~GPIO_ODR_OD8;	
	break;
	
	case 2:
		GPIOC->ODR &= ~GPIO_ODR_OD9;
		GPIOC->ODR |= GPIO_ODR_OD5;
		GPIOC->ODR &= ~GPIO_ODR_OD6;
		GPIOC->ODR &= ~GPIO_ODR_OD8;	
	break;

	case 3:
		GPIOC->ODR &= ~GPIO_ODR_OD9;
		GPIOC->ODR |= GPIO_ODR_OD5;
		GPIOC->ODR |= GPIO_ODR_OD6;
		GPIOC->ODR &= ~GPIO_ODR_OD8;
	break;
	
	case 4:
		GPIOC->ODR &= ~GPIO_ODR_OD9;
		GPIOC->ODR &= ~GPIO_ODR_OD5;
		GPIOC->ODR |= GPIO_ODR_OD6;
		GPIOC->ODR &= ~GPIO_ODR_OD8;
	break;
	
	case 5:
		GPIOC->ODR &= ~GPIO_ODR_OD9;
		GPIOC->ODR &= ~GPIO_ODR_OD5;
		GPIOC->ODR |= GPIO_ODR_OD6;
		GPIOC->ODR |= GPIO_ODR_OD8;	
	break;

	case 6:
		GPIOC->ODR &= ~GPIO_ODR_OD9;
		GPIOC->ODR &= ~GPIO_ODR_OD5;
		GPIOC->ODR &= ~GPIO_ODR_OD6;
		GPIOC->ODR |= GPIO_ODR_OD8;	
	break;

	case 7:
		GPIOC->ODR |= GPIO_ODR_OD9;
		GPIOC->ODR &= ~GPIO_ODR_OD5;
		GPIOC->ODR &= ~GPIO_ODR_OD6;
		GPIOC->ODR |= GPIO_ODR_OD8;
	break;
	}
}

int main()
{
	
	init_gpio();
	enable_step_tim2();
	enable_encoder_tim3();
	Enc_Trig_Init();
	
	while(1)
	{
		TIM2->CR1 |= TIM_CR1_CEN;
		
		drive(&ticks);
	
		TIM3->CR1 |= TIM_CR1_CEN;
		
		Enc_Counter = TIM3->CNT;
		
	}
	
}