/*
 * @file main.c
 *
 * @brief Main source code for the UART program.
 *
 * This file contains the main entry point and function definitions for the UART program.
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 *   - Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @author
 */

#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include "UART0.h"
#include "UART1.h"
#include "GPIO.h"

int main(void)
{
	SysTick_Delay_Init();
	UART0_Init();
	UART1_Init();
	RGB_LED_Init();
	while(1)
	{
		/*
		UART0_Output_String("Hello World");
		UART0_Output_Newline();
		SysTick_Delay1ms(500);
		*/
		
		
		/*
		UART0_Output_String("Enter Decimal Number: ");
		uint32_t input_decimal = UART0_Input_Unsigned_Decimal();
		UART0_Output_Newline();
		UART0_Output_String("Output Decimal");
		UART0_Output_Unsigned_Decimal(input_decimal);
		UART0_Output_Newline();
		SysTick_Delay1ms(500);
		*/
		
		
		/*
		char str_buffer[64];
		UART0_Output_String("Enter Your Name(s): ");
		UART0_Input_String(str_buffer, 64);
		UART0_Output_Newline();
		
		UART0_Output_String("Name(s) Received: ");
		UART0_Output_String(str_buffer);
		UART0_Output_Newline();
		SysTick_Delay1ms(500);
		
		UART0_Output_String("Enter a character: ");
		UART0_Input_String(str_buffer, 64);
		UART0_Output_Newline();
		
		UART0_Output_String("Character Receieved: ");
		UART0_Output_String(str_buffer);
		UART0_Output_Newline();
		SysTick_Delay1ms(500);
		
		UART0_Output_String("Enter Decimal Number: ");
		uint32_t input_decimal = UART0_Input_Unsigned_Decimal();
		UART0_Output_Newline();
		UART0_Output_String("Output Decimal ");
		UART0_Output_Unsigned_Decimal(input_decimal);
		UART0_Output_Newline();
		SysTick_Delay1ms(500);
		
		UART0_Output_String("Enter a hexadecimal value: ABCDEF");
		uint32_t hexadecimal_received = UART0_Input_Unsigned_Hexadecimal();
		UART0_Output_Newline();
		UART0_Output_String("Hexadecimal Received ");
		UART0_Output_Unsigned_Hexadecimal(hexadecimal_received);
		UART0_Output_Newline();
		SysTick_Delay1ms(500);
		
	*/
	
	/*
		char str_buffer[64];
		UART0_Output_String("Enter Command: ");
		UART0_Input_String(str_buffer, 64);
		UART0_Output_Newline();
		
		Uart_Command(str_buffer);
		SysTick_Delay1ms(100);
	*/
	
	

		UART1_Output_Character(0x55);
		SysTick_Delay1ms(500);
	
	}
}

void RGC_UART_Controller(void)
{
	char str_buffer[64];
	UART0_Output_String("Enter a character: ");
	UART0_Output_String(str_buffer);
	
	
}