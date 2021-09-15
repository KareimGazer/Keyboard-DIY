/*
keyboard module using interrupt
extinsion: use UART interrupt to put values in fifo and send them via UART
*/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "keypad.h"

#define FOREVER 1

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void SystemInit(void);


int main(){
	// init keybad
	// init uart
	PLL_Init(Bus50MHz);       // set system clock to 50 MHz
	Keypad_Init();
	// OutCRLF();
	EnableInterrupts();
	while(FOREVER){
		WaitForInterrupt();
	}
}

void SystemInit(void){}
