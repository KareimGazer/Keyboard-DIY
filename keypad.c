#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "UART.h"

#define NVIC_EN0_INT1 0x00000002  // Interrupt 1 enable portB assumed

#define MS 50000

#define FIFOSIZE   16         // size of the FIFOs (must be power of 2)
#define FIFOSUCCESS 1         // return value on success
#define FIFOFAIL    0         // return value on failure

uint8_t KEYS[4][4] = {
                      {'D', '#', '0', '*'},
											{'C', '9', '8', '7'},
											{'B', '6', '5', '4'},
											{'A', '3', '2', '1'}
                     };


uint8_t dataReady = 0, data = '1', row = 0, col = 0, count=0; // keypad mailbox
										 
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value


void PORTB_Init(void){
	// DisableInterrupts();
	
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
  GPIO_PORTB_IEV_R &= ~0x0F;   // PE5-4 falling edge event
  GPIO_PORTB_ICR_R = 0x0F;    // clear flag5-4
  GPIO_PORTB_IM_R |= 0x0F;    // enable interrupt on PE5-4
                              // GPIO PortB = priority 2
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF00FF)|0x00004000; // bits 5-7
  NVIC_EN0_R = NVIC_EN0_INT1; // enable interrupt 1 in NVIC
	
  // EnableInterrupts();
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

void SysTick_Init(uint32_t period){long sr;
  sr = StartCritical();
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x80000000; // priority 3
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
  EndCritical(sr);
}

void SysTick_Handler(void){
  keypadOn();
	if(dataReady==1){ UART_OutChar(data); count++;}
	dataReady = 0;
}

void Keypad_Init(void){
	PORTB_Init();
	SysTick_Init(50*MS);
	UART_Init();              // initialize UART
}
