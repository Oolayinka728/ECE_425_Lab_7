/**
 * @file UART1.c
 *
 * @brief source code for the UART

 * To verify the pinout of the user LED, refer to the Tiva C Series TM4C123G LaunchPad User's Guide
 * Link: https://www.ti.com/lit/pdf/spmu296
 *
 * @author Olayinka Olowookere
*/




#include "GPIO.h"
#include "SysTick_Delay.h"
#include "UART1.h"



void UART1_Init(void)
{
    // Enable the clock to the UART1 module by setting the R1 bit (Bit 1) in the RCGCUART register
    SYSCTL->RCGCUART |= 0x02;
    
    // Enable the clock to Port B by setting the R1 bit (Bit 1) in the RCGCGPIO register
    SYSCTL->RCGCGPIO |= 0x02;
    
    // Disable the UART1 module before configuration by clearing the UARTEN bit (Bit 0) in the CTL register
    UART1->CTL &= ~0x0001;
    
    // Configure the UART1 module to use the system clock (50MHz) divided by 16 by clearing the HSE bit (Bit 5) in the CTL register
    UART1->CTL &= ~0x0020;
    
    // Set the baud rate by writing to the DIVINT (integer) and DIVFRAC (fractional) fields
    // Assuming the same calculation as in UART1
    UART1->IBRD = 325;  // Integer part of the baud rate divisor(9600)
    UART1->FBRD = 33;   // Fractional part of the baud rate divisor
    
    // Configure the line control for 8-bit data, no parity, and one stop bit
    UART1->LCRH |= 0x60;  // Set WLEN = 3 (8-bit data length)
    UART1->LCRH &= ~0x10; // Clear PEN (Parity Enable) for no parity
    UART1->LCRH &= ~0x08; // Clear STP2 (1 stop bit)
    UART1->LCRH &= ~0x02; // Clear EPS (Even Parity Select)
    // Enable UART1 by setting the UARTEN bit (Bit 0) in the CTL register
    UART1->CTL |= 0x01;
		
    
    // Configure GPIO Port B pins for UART1 (PB0 = U1Rx, PB1 = U1Tx)
    GPIOB->AFSEL |= 0x03;       // Enable alternate function on PB0 and PB1
    GPIOB->PCTL &= ~0x000000FF; // Clear PCTL for PB0 and PB1
    GPIOB->PCTL |= 0x00000011;  // Set PCTL to select UART1 for PB0 and PB1
    GPIOB->DEN |= 0x03;         // Enable digital functionality on PB0 and PB1
}

char UART1_Input_Character(void)
{	
	while((UART1->FR & UART1_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
	return (char)(UART1->DR & 0xFF);
}

void UART1_Output_Character(char data)
{
	while ((UART1 -> FR & UART1_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
	
	UART1 -> DR = data;
}

void UART1_Input_String(char *buffer_pointer, uint16_t buffer_size) 
{
	int length = 0;
	char character = UART1_Input_Character();
	
	while(character != UART1_CR)
	{
		if (character == UART1_BS)
		{
			if (length)
			{
				buffer_pointer--;
				length--;
				UART1_Output_Character(UART1_BS);
			}
		}
		else if (length < buffer_size)
		{
			*buffer_pointer = character;
			buffer_pointer++;
			length++;
			UART1_Output_Character(character);
		}
		character = UART1_Input_Character();
			
	}
		*buffer_pointer = 0;
}

void UART1_Output_String(char *pt)
{
	while(*pt)
	{
		UART1_Output_Character(*pt);
		pt++;
	}
}

uint32_t UART1_Input_Unsigned_Decimal(void)
{
	uint32_t number = 0;
	uint32_t length = 0;
	char character = UART1_Input_Character();
	
	//Accept unitl <enter> is typed
	//The next line checks that the inputs is a digit 0-9
	//If the chacater is not 0-9, it is ignored and not echoed
	while(character !=UART1_CR)
	{
		if((character >= '0') && (character <= '9'))
		{
			//number will overflow if it is above 4,294,967,295
			number = (10 * number) + (character - '0');
			length++;
			UART1_Output_Character(character);
		}
		//if the input is a bakcspace then return number is
		//change and the backbage is outputted to the screen
		else if ((character == UART1_BS) && length)
		{
			number /= 10;
			length--;
			UART1_Output_Character(character);
		}
		character = UART1_Input_Character();
	}
	return number;
}

void UART1_Output_Unsigned_Decimal(uint32_t n)
{
	//use recusion to conver decimal number
	//of unspecified length as an ASCII string
	if(n>= 10)
	{
		UART1_Output_Unsigned_Decimal(n/10);
		n = n % 10;
	}
	//n is between  0 and 9
	UART1_Output_Character(n + '0');
	
}

uint32_t UART1_Input_Unsigned_Hexadecimal(void)
{
	uint32_t number = 0;
	uint32_t digit = 0;
	uint32_t length = 0;
	char character = UART1_Input_Character();
	
	
	while (character != UART1_CR)
	{
		//Initialize digit and assume that the hexdecimal character is invalid
		digit = 0x10;
		if((character>= '0') && (character <= '9'))
		{
			digit = character - '0';
			
		} 
		else if ((character>='A') &&(character <= 'F'))
		{
			digit = (character - 'A') + 0xA;
		}
		else if((character >= 'a') && (character <='f'))
		{
			digit = (character - 'a') + 0xA;
		}
		 //if the character is not 0-9 or A-F, it is ignored and not echoed
		if(digit<= 0xF)
		{
			number = (number * 0x10) + digit;
			length++;
			UART1_Output_Character(character);
		}
		//Backspace outputted and return value changed if a backspace is inputeed
		else if((character == UART1_BS) && length)
		{
			number /= 0x10;
			length--;
			UART1_Output_Character(character);
		}
		character = UART1_Input_Character();
	}
	return number;
}

void UART1_Output_Unsigned_Hexadecimal(uint32_t number)
{
	//Us erecursino to conver the number of 
	//unspecified length as an ascii string
	if (number >= 0x10)
	{
		UART1_Output_Unsigned_Hexadecimal(number / 0x10);
		UART1_Output_Unsigned_Hexadecimal(number % 0x10);		
	}
	else{
		if(number < 0xA)
		{
			UART1_Output_Character(number + '0');
			
		}
		else
		{
			UART1_Output_Character((number - 0x0A) + 'A');
		}
		
	}
}

void UART1_Output_Newline(void)
{
	UART1_Output_Character(UART1_CR);
	UART1_Output_Character(UART1_LF);
}
