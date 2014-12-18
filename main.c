#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "string_utils.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

#define DELIMS " \t\r\n"
#define BAUDS 115200
#define MAX_LENGTH 80

int i = 0;
char message[MAX_LENGTH] = "";

void toggleLed(int arg){
	switch(arg){
	case 0:
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
		GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_0);
		if (GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_0)){
			GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
			GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00);
		}
		else{
			GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
			GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
		}
		break;
	case 1:
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
		GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_1);
		if (GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1)){
			GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
			GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x00);
		}
		else{
			GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
			GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
		}
		break;
	case 2:
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
		if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
		}
		else{
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
		}
		break;
	case 3:
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
		if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x00);
		}
		else{
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
		}
		break;
	}
}

void processMessage(char* s, uint32_t UARTBase, int length){
	int errno = 0;
	char* cmd;
	char *arg;

	if ((cmd = strtok(s, DELIMS))) {

		// command LED
		if (strcmp(cmd, "led") == 0) {
			arg = strtok(0, DELIMS);

			if (strcmp(arg, "") == 0){
				errno = 2;
			}
			else if (strcmp(arg, "0") == 0 || strcmp(arg, "0") == 1 || strcmp(arg, "0") == 2 || strcmp(arg, "0") == 3){
				toggleLed(arg[0] - 48);
			}
			else{
				errno = 3;
			}
		}

		// command hello
		else if (strcmp(cmd, "hello") == 0) {
			UARTprintf("hello world!\n\r");
		}

		// command PWM
		else if (strcmp(cmd, "pwm") == 0) {
			arg = strtok(0, DELIMS);
			if (!arg){
				errno = 2;
			}
			else{
				if (strcmp(arg, "A0") == 0){
					UARTprintf("DUMMY: pwm in PA2\n\r");
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
					GPIOPinConfigure(GPIO_PA0_T0CCP0);
					GPIOPinTypeTimer(GPIO_PORTA_BASE, GPIO_PIN_0);
					SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
					TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
					TimerLoadSet(TIMER0_BASE, TIMER_A, 100000);
					TimerMatchSet(TIMER0_BASE, TIMER_A, 25000);
					TimerEnable(TIMER0_BASE, TIMER_A);
				}
			}
		}

		// command HELP
		else if (strcmp(cmd, "help") == 0) {
			arg = strtok(0, DELIMS);

			if (strcmp(arg, "") == 0){
				UARTprintf("led <arg> \tToggle on or off the state of the led specified in <arg> [0~3].\n\r");
				UARTprintf("hello \t\tDisplay a screen message.\n\r");
				UARTprintf("pwm <args> \tActivate PWM in port <arg1>, at <arg2> frequency and a \n\r\t\tduty cycle of <arg3>.\n\r");
			}
			else if (strcmp(arg, "led") == 0){
				UARTprintf("DUMMY: led help\n\r");
			}
			else if (strcmp(arg, "pwm") == 0){
				UARTprintf("DUMMY: pwm help\n\r");
			}
			else{
				errno = 4;

			}
		}

		// ERROR command
		else{
			errno = 1;
		}

	}

	switch (errno){
	case 1:
		UARTprintf("ERROR: Invalid command \"%s\"\n\r", s);
		break;
	case 2:
		UARTprintf("ERROR: Missing argument\n\r");
		break;
	case 3:
		UARTprintf("ERROR: Invalid argument \"%s\"\n\r", arg);
		break;
	case 4:
		UARTprintf("ERROR: Topic not found for \"%s\".\n\r", arg);
		break;
	}

	// reset message
	stringReset(message);
}



// UART 0 interrupt
void UARTIntHandler(void){

	uint32_t ui32Status;
	char buffer;

	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts

	while(UARTCharsAvail(UART0_BASE)) {

		buffer = UARTCharGetNonBlocking(UART0_BASE);

		if (isChar(buffer)){
			message[i] = buffer;
			UARTCharPutNonBlocking(UART0_BASE, message[i]);
			i++;
		}

		if (message[i-1] == '\r'){ // ENTER
			UARTCharPutNonBlocking(UART0_BASE, '\n');
			processMessage(message, UART0_BASE, i - 1);
			UARTprintf("> ");
			i = 0;
		}

		if (message[i-1] == 127){ // BACKSPACE
			UARTCharPutNonBlocking(UART0_BASE, message[i-1]);
			message[i-1] = '\0';
			i -= 2;
		}

	}
}


int main(void){

	//clock
	uint32_t ui32SysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	//UART0 (consola)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	UARTStdioConfig(0, BAUDS, ui32SysClkFreq);

	UARTprintf("\033[2J\033[H"); // CLEAR
	UARTprintf("Welcome to the TIVA C launchpad!\n");
	UARTprintf("Type \"help\" for a list of commands\n");
	UARTprintf("> ");

	//enable interrupts
	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	while (1){}
}

