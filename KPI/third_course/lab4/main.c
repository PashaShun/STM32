typedef unsigned long uint32;
typedef unsigned long long uint64;

//  AHB1 bus enable register
#define RCC_AHB1_ENABLE_REG (uint32*)0x40023830

//  mode register
#define GPIOA_MODER         (uint32*)0x40020000

#define GPIOC_MODER         (uint32*)0x40020800

//#define GPIOA_IDR           (uint32*)0x40020010

#define GPIOA_IDR           (uint32*)0x40020810

//  bit set/reset register (atomic)
#define GPIOA_BSRR          (uint32*)0x40020018

#define RCC_AHB1_GPIOA_CLOCK 0
#define GPIOA_LED_PIN_BLUE   5
#define GPIOA_LED_PIN_RED    6
#define GPIOA_LED_PIN_YELLOW 7

#define GPIOA_PIN_BUTTON	 13

void delay() {
	
	volatile uint32  i = 500000;	
	while ( i!=0) i--;

	}
		


void wait_5ms () {
	volatile uint32  j = 1250;		// для затримки ~5 мс.
	while ( j!=0) j--; 
}

volatile uint32 BUTTON (char state){
	volatile uint32  k = 0;
	volatile uint32  counter = 0;
	if ((*GPIOA_IDR & 0x2000) == 0x0){
	counter = 0;
	state = 0;
	} else if ((*GPIOA_IDR & 0x2000) == 0x1)
	{
	for (k = 0; k <10; k++)
	{
		wait_5ms();
		if ((*GPIOA_IDR & 0x2000) == 0x0){
			counter = 0;
		} else {
			counter += 1;
		}
		if (counter >= 4){
			state = 1;
		}
	}
	
	}
	return state;
}

	


void main_loop()
{
	volatile uint32	state = 0;
	char led_on = 0;
	
	*RCC_AHB1_ENABLE_REG |= (1 << RCC_AHB1_GPIOA_CLOCK);
	*GPIOA_MODER &= ~(1 << (GPIOA_LED_PIN_BLUE * 2 + 1));
	*GPIOA_MODER |= (1 << (GPIOA_LED_PIN_BLUE * 2));
	*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_BLUE);
	
	*GPIOA_MODER &= ~(1 << (GPIOA_LED_PIN_RED * 2 + 1));
	*GPIOA_MODER |= (1 << (GPIOA_LED_PIN_RED * 2));
	*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_RED);
	
	*GPIOA_MODER &= ~(1 << (GPIOA_LED_PIN_YELLOW * 2 + 1));
	*GPIOA_MODER |= (1 << (GPIOA_LED_PIN_YELLOW * 2));
	*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_YELLOW);
	
	*GPIOC_MODER &= ~(0 << (GPIOA_PIN_BUTTON * 2 + 1));
	*GPIOC_MODER |= (0 << (GPIOA_PIN_BUTTON * 2));
	

	while (1) {		
		
		state = BUTTON(state);
		
		if (state > 0) {
			*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_BLUE);
			*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_RED);
			*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_YELLOW);
		}
		
	    if (led_on == 0) {
			*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_BLUE);
			//*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_RED);
			//*GPIOA_BSRR |= (1 << GPIOA_LED_PIN_YELLOW);
		} else {
			*GPIOA_BSRR |= (1 << (GPIOA_LED_PIN_BLUE + 16));
			*GPIOA_BSRR |= (1 << (GPIOA_LED_PIN_RED + 16));
			*GPIOA_BSRR |= (1 << (GPIOA_LED_PIN_YELLOW + 16));
		}
		delay();
		if (led_on == 0)
		{
			led_on = 1;
		} else led_on = 0;
	}
}