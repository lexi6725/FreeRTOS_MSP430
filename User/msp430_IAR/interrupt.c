#include "FreeRTOS.h"
#include "nrf24l01.h"
#include "hmc5883l.h"
#include "mpu9050.h"

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
	if (P1IFG & BIT4)
		nrf_isr();
	if (P1IFG & BIT5)
		HMC_ISR();

	P1IFG = 0;
}

