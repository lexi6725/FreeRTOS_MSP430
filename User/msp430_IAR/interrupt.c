#include "FreeRTOS.h"
#include "nrf24l01.h"
#include "hmc5883l.h"
#include "mpu9050.h"
#include "pwm.h"

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
	if (P1IFG & BIT4)
		nrf_isr();
	if (P1IFG & BIT5)
		HMC_ISR();
	if (P1IFG & BIT6)
		MPU9050_ISR();
		
	P1IFG = 0;
}

extern void vTickISR(void);
#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0_ISR(void)
{
	vTickISR();
}

#pragma vector=TIMERB0_VECTOR
__interrupt void TimerB0_ISR(void)
{
	TimerB_ISR();
}
