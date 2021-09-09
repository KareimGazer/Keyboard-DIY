/*
keyboard module using interrupt
extinsion: use UART interrupt to put values in fifo and send them via UART
*/

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define NVIC_EN0_INT1 0x00000002  // Interrupt 1 enable portB assumed
#define FOREVER 1

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void SystemInit(void);
void PORTB_init(void);
void GPIOPortB_Handler(void);
void PORTF_init(void);
// sends low signals periodically to keypad
// to cause interrupts when the user press a button
void keypadOn(void);
void testKeypad(void);

/*
uint8_t KEYS[4][4] = {{'1', '2', '3', 'A'},
                      {'4', '5', '6', 'B'},
                      {'7', '8', '9', 'C'},
                      {'*', '0', '#', 'D'}
                     };
*/
/*
uint8_t KEYS[4][4] = {{'A', '3', '2', '1'},
                      {'B', '6', '5', '4'},
                      {'C', '9', '8', '7'},
                      {'D', '#', '0', '*'}
                     };
*/


uint8_t KEYS[4][4] = {
                      {'D', '#', '0', '*'},
											{'C', '9', '8', '7'},
											{'B', '6', '5', '4'},
											{'A', '3', '2', '1'}
                     };


uint8_t dataReady = 0, data = '1', row = 0, col = 0; // keypad mailbox

int main(){
	PORTF_init();
	PORTB_init();
	
	while(FOREVER){
		keypadOn();
		testKeypad();
	}
}

void SystemInit(void){}

void PORTB_init(void){
	DisableInterrupts();
	
	// port config
  SYSCTL_RCGCGPIO_R |= 0x00000002; // activate port B
	dataReady = 0;
  GPIO_PORTB_AMSEL_R &= ~0xFF;// disable analog function on PE5-4
  GPIO_PORTB_PCTL_R &= ~0xFFFFFFFF; // configure PE5-4 as GPIO 
  GPIO_PORTB_DIR_R = 0xF0;  // high nibble out - low nibble in
  GPIO_PORTB_AFSEL_R &= ~0xFF;// disable alt funct on PE5-4 
  GPIO_PORTB_DEN_R |= 0xFF;   // enable digital I/O on PE5-4
	GPIO_PORTB_PUR_R |= 0x0F; 
	
  GPIO_PORTB_IS_R &= ~0x0F;   // PE5-4 is edge-sensitive 
  GPIO_PORTB_IBE_R &= ~0x0F;  // PE5-4 is not both edges 
  GPIO_PORTB_IEV_R &= ~0x0F;   // PE5-4 rising edge event
  GPIO_PORTB_ICR_R = 0x0F;    // clear flag5-4
  GPIO_PORTB_IM_R |= 0x0F;    // enable interrupt on PE5-4
                              // GPIO PortB = priority 2
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF00FF)|0x00004000; // bits 5-7
  NVIC_EN0_R = NVIC_EN0_INT1; // enable interrupt 1 in NVIC
  EnableInterrupts();
}

void GPIOPortB_Handler(void){
  if(GPIO_PORTB_RIS_R & 0x01){  // poll PB0
    GPIO_PORTB_ICR_R = 0x01;  // acknowledge flag4
    col = 0;                  // column detected
  }
  if(GPIO_PORTB_RIS_R & 0x02){  // poll PB1
    GPIO_PORTB_ICR_R = 0x02;  // acknowledge flag4
    col = 1;                  // column detected
  }
	if(GPIO_PORTB_RIS_R & 0x04){  // poll PB2
    GPIO_PORTB_ICR_R = 0x04;  // acknowledge flag4
    col = 2;                  // column detected
  }
	if(GPIO_PORTB_RIS_R & 0x08){  // poll PB3
    GPIO_PORTB_ICR_R = 0x08;  // acknowledge flag4
    col = 3;                  // column detected
  }
	
	data = KEYS[row][col];
	dataReady = 1;
}

void keypadOn(void){
	// assumes the high nipple is output
	// and the low nibble is input
	uint8_t outPin, index;
	GPIO_PORTB_DATA_R |= 0xF0;
	outPin = 0x10;
	for(index = 0; index < 4; index++){
	row = index;
	GPIO_PORTB_DATA_R = ((GPIO_PORTB_DATA_R&0X0F) | (~outPin));
	outPin = outPin << 1;
	}
}

void PORTF_init(void){
	SYSCTL_RCGCGPIO_R |= 0X20;
	dataReady = 0; // just more cycles added for init
	GPIO_PORTF_DIR_R = 0x0E;
	GPIO_PORTF_DEN_R = 0x0E;
	GPIO_PORTF_DATA_R = 0;
}

// test using portF
void testKeypad(void){
	if(dataReady == 1){
		if(data == '1') GPIO_PORTF_DATA_R = 0x02;
		else GPIO_PORTF_DATA_R &= ~0x02;
		if(data == '5') GPIO_PORTF_DATA_R = 0x04;
		else GPIO_PORTF_DATA_R &= ~0x04;
		if(data == '9') GPIO_PORTF_DATA_R = 0x08;
		else GPIO_PORTF_DATA_R &= ~0x08;
	}
}
