/**
 * @file UART0.c
 *
 * @brief Source code for the UART0 driver.
 *
 * This file contains the function definitions for the UART0 driver.
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 *   - Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @note Assumes that the frequency of the system clock is 50 MHz.
 *
 * @author Aaron Nanas
 */
#include <stdio.h>
#include <string.h>
#include "UART0.h"
#include "GPIO.h"
#include "SysTick_Delay.h"

void UART0_Init(void)
{
//Enabke the clock to the UART0 module by setting the
	//R0 bit (Bit 0) in the RCGCUART register
	SYSCTL -> RCGCUART |= 0x01;
	
	//Enable the clock to Port A by setting th e
	//R0 bit (Bit 0) in the RCGCGPIO register
	SYSCTL -> RCGCGPIO |=0x01;
	
	//Disable the UART0 module before configuration by clearing 
	//the UARTEN bit (Bit 0) in thr CTL register
	UART0 -> CTL &= ~0x0001;
	
	//Configure the UART0 mdule to use the system clock 50MHz
	//divided by 16 by celaring the HSE bit (Bit 5) in the CTL register
	UART0 -> CTL &= ~0x0020;
	
	//Set the baud rate by wirting to the DIVINT field (Bit 15 ot 0)
	//and the DIVFRAC field (Bits 5 to 0) in the IBRD register, respectively.
	//The integer part of the calculated constant will be wrtten to the IBRD register
	//while the fractinoal part will be written to the FBRD register.
	//BRDI = (50,000,000 )/ (16 * 115200 = 27.12673611 (IBRD = 27)
	//BRDF = (0.12673611 * 64) + 0.5) = 5.611 (FBRD =8)
	UART0 -> IBRD = 27;
	UART0 ->FBRD = 8;
	
	//Write the rest of the initialization functino below
	//Refer to the Pre-Lab section
	UART0 -> LCRH |= 0x60;
	UART0 -> LCRH |= 0x10;
	UART0 -> LCRH &= ~0x08;
	UART0 -> LCRH &= ~0x02;
	UART0 -> CTL |= 0x01;
	GPIOA -> AFSEL |= 0x03;
	GPIOA -> PCTL &= ~0x000000FF;
	GPIOA -> PCTL |= 0x00000010;
	GPIOA -> PCTL |= 0x00000001;
	GPIOA -> DEN |= 0x03;
}

char UART0_Input_Character(void)
{	
	while((UART0->FR & UART0_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
	return (char)(UART0->DR & 0xFF);
}

void UART0_Output_Character(char data)
{
	while ((UART0 -> FR & UART0_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
	
	UART0 -> DR = data;
}

void UART0_Input_String(char *buffer_pointer, uint16_t buffer_size) 
{
	int length = 0;
	char character = UART0_Input_Character();
	
	while(character != UART0_CR)
	{
		if (character == UART0_BS)
		{
			if (length)
			{
				buffer_pointer--;
				length--;
				UART0_Output_Character(UART0_BS);
			}
		}
		else if (length < buffer_size)
		{
			*buffer_pointer = character;
			buffer_pointer++;
			length++;
			UART0_Output_Character(character);
		}
		character = UART0_Input_Character();
			
	}
		*buffer_pointer = 0;
}

void UART0_Output_String(char *pt)
{
	while(*pt)
	{
		UART0_Output_Character(*pt);
		pt++;
	}
}

uint32_t UART0_Input_Unsigned_Decimal(void)
{
	uint32_t number = 0;
	uint32_t length = 0;
	char character = UART0_Input_Character();
	
	//Accept unitl <enter> is typed
	//The next line checks that the inputs is a digit 0-9
	//If the chacater is not 0-9, it is ignored and not echoed
	while(character !=UART0_CR)
	{
		if((character >= '0') && (character <= '9'))
		{
			//number will overflow if it is above 4,294,967,295
			number = (10 * number) + (character - '0');
			length++;
			UART0_Output_Character(character);
		}
		//if the input is a bakcspace then return number is
		//change and the backbage is outputted to the screen
		else if ((character == UART0_BS) && length)
		{
			number /= 10;
			length--;
			UART0_Output_Character(character);
		}
		character = UART0_Input_Character();
	}
	return number;
}

void UART0_Output_Unsigned_Decimal(uint32_t n)
{
	//use recusion to conver decimal number
	//of unspecified length as an ASCII string
	if(n>= 10)
	{
		UART0_Output_Unsigned_Decimal(n/10);
		n = n % 10;
	}
	//n is between  0 and 9
	UART0_Output_Character(n + '0');
	
}

uint32_t UART0_Input_Unsigned_Hexadecimal(void)
{
	uint32_t number = 0;
	uint32_t digit = 0;
	uint32_t length = 0;
	char character = UART0_Input_Character();
	
	
	while (character != UART0_CR)
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
			UART0_Output_Character(character);
		}
		//Backspace outputted and return value changed if a backspace is inputeed
		else if((character == UART0_BS) && length)
		{
			number /= 0x10;
			length--;
			UART0_Output_Character(character);
		}
		character = UART0_Input_Character();
	}
	return number;
}

void UART0_Output_Unsigned_Hexadecimal(uint32_t number)
{
	//Us erecursino to conver the number of 
	//unspecified length as an ascii string
	if (number >= 0x10)
	{
		UART0_Output_Unsigned_Hexadecimal(number / 0x10);
		UART0_Output_Unsigned_Hexadecimal(number % 0x10);		
	}
	else{
		if(number < 0xA)
		{
			UART0_Output_Character(number + '0');
			
		}
		else
		{
			UART0_Output_Character((number - 0x0A) + 'A');
		}
		
	}
}

void UART0_Output_Newline(void)
{
	UART0_Output_Character(UART0_CR);
	UART0_Output_Character(UART0_LF);
}


void Uart_Command (char *Command)
{
	char* command[] = {"RGB LED RED","RGB LED GREEN", "RGB LED BLUE", "RGB LED CYCLE"};	
	for (int i =0; i < 4; i++){
		if (strcmp(Command, command[0]) == 0)
		{
				RGB_LED_Output(0x02);
				SysTick_Delay1ms(50);
		}
		else if (strcmp(Command, command[1]) == 0)
		{
			RGB_LED_Output(0x08);	
			SysTick_Delay1ms(50);
		}
		else if (strcmp (Command, command[2]) == 0)
		{
			RGB_LED_Output(0x04);	
			SysTick_Delay1ms(50);
		}
		else if (strcmp(Command,command[3]) == 0)
		{
			RGB_LED_Output(0x02);	
			SysTick_Delay1ms(100);
			RGB_LED_Output(0x04);
			SysTick_Delay1ms(100);
			RGB_LED_Output(0x08);
			SysTick_Delay1ms(100);
			
		}else if (strcmp(Command, command[i]) != 0)
		{
			UART0_Output_String("Invalid command, Try Again ");
			RGB_LED_Output(RGB_LED_OFF);
		}
	}
}