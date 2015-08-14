/*
    FreeRTOS V8.2.1 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 *
 * This demo is configured to execute on the ES449 prototyping board from
 * SoftBaugh. The ES449 has a built in LCD display and a single built in user
 * LED.  Therefore, in place of flashing an LED, the 'flash' and 'check' tasks
 * toggle '*' characters on the LCD.  The left most '*' represents LED 0, the
 * next LED 1, etc.
 *
 * Main. c also creates a task called 'Check'.  This only executes every three
 * seconds but has the highest priority so is guaranteed to get processor time.
 * Its main function is to check that all the other tasks are still operational.
 * Each task that does not flash an LED maintains a unique count that is
 * incremented each time the task successfully completes its function.  Should
 * any error occur within such a task the count is permanently halted.  The
 * 'check' task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have
 * changed all the tasks are still executing error free, and the check task
 * toggles an LED with a three second period.  Should any task contain an error
 * at any time the LED toggle rate will increase to 500ms.
 *
 * Please read the documentation for the MSP430 port available on
 * http://www.FreeRTOS.org.
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

/* application includes. */
#include "main.h"
#include "partest.h"
#include "flash.h"
#include "comtest2.h"
#include "pwm.h"
#include "nrf24l01.h"
#include "mpu9050.h"
#include "hmc5883l.h"

/* App task priorities. */
#define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainComm_TASK_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainMPU_TASK_PRIORITY			( tskIDLE_PRIORITY + 3 )

/* The constants used in the calculation. */
#define intgCONST1				( ( long ) 123 )
#define intgCONST2				( ( long ) 234567 )
#define intgCONST3				( ( long ) -3 )
#define intgCONST4				( ( long ) 7 )
#define intgEXPECTED_ANSWER		( ( ( intgCONST1 + intgCONST2 ) * intgCONST3 ) / intgCONST4 )

/*
 * Perform the hardware setup required by the ES449 in order to run the demo
 * application.
 */
static void prvSetupHardware( void );

portBASE_TYPE xLocalError = pdFALSE;
volatile unsigned long ulIdleLoops = 0UL;

/* Event Group*/
EventGroupHandle_t xEventGroup = NULL;

extern void vPortSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/
static portTASK_FUNCTION_PROTO( vCommTask, pvParameters );
static portTASK_FUNCTION( vMPDTask, pvParameters );

/*
 * Start the demo application tasks - then start the real time scheduler.
 */
int main( void )
{
	/* Setup the hardware ready for the demo. */
	prvSetupHardware();                                                                                                                                                                           
	vParTestInitialise();
	Init_PWM();
	
	if ((xEventGroup = xEventGroupCreate()) != NULL)
	{
		/* Start the standard demo application tasks. */
		vStartLEDFlashTasks( mainLED_TASK_PRIORITY );
		xTaskCreate( vCommTask, "nRF", configMINIMAL_STACK_SIZE*3, NULL, mainComm_TASK_PRIORITY, ( TaskHandle_t * ) NULL );
		xTaskCreate( vMPDTask, "mpu", configMINIMAL_STACK_SIZE*3, NULL, mainMPU_TASK_PRIORITY, ( TaskHandle_t * ) NULL );

		/* Start the scheduler. */
		vTaskStartScheduler();
	}
	vParTestSetLED(7, 1);
	while(1);

	/* As the scheduler has been started the demo applications tasks will be
	executing and we should never get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void CommProcess(uint8_t *buf)
{
	if (buf[0] != 0x01)
		return;

	if ((buf[1] & KEY_SPD_RDC) == KEY_SPD_RDC)
	{
		ControlProcess(CMD_RDU_RATE);
	}
	else if ((buf[1] & KEY_SPD_ADD) == KEY_SPD_ADD)
	{
		ControlProcess(CMD_ADD_RATE);
	}
	else if ((buf[1] & KEY_UP) == KEY_UP)
	{
		ControlProcess(CMD_DIR_FRONT);
	}
	else if ((buf[1] & KEY_DOWN) == KEY_DOWN)
	{
		ControlProcess(CMD_DIR_BACK);
	}
	else if ((buf[1] & KEY_LEFT) == KEY_LEFT)
	{
		ControlProcess(CMD_DIR_LEFT);
	}
	else if ((buf[1] & KEY_RIGHT) == KEY_RIGHT)
	{
		ControlProcess(CMD_DIR_RIGHT);
	}
	else if ((buf[1] & KEY_BRK) == KEY_BRK)
	{
		ControlProcess(CMD_STOP);
	}
}

static portTASK_FUNCTION( vCommTask, pvParameters )
{
	uint8_t buf[nRF_PLOAD_WIDTH];
	(void)pvParameters;

	nRF_Init();

	nrf_rx_mode();
	vTaskDelay(1);
	
	for(;;)
	{
		if (nrf_start_rx(buf, nRF_PLOAD_WIDTH) == RX_OK)
		{
			vParTestToggleLED(6);
			CommProcess(buf);
			buf[0] |= 0x80;
			nrf_start_tx(buf, nRF_PLOAD_WIDTH);
			vParTestToggleLED(6);
		}
		memset(buf, 0, nRF_PLOAD_WIDTH);
	}
}

static void prvSetupHardware( void )
{
	/* Stop the watchdog. */
	WDTCTL = WDTPW + WDTHOLD;

	/* Setup XT2 To MCLK 8MHz */
	volatile uint8_t i;

	BCSCTL1	&= ~XT2OFF;

	do{
		IFG1 &= ~OFIFG;
		for (i = 0xff; i > 0; --i);
	}while(IFG1&OFIFG);

	BCSCTL2 = SELM_2 + SELS;

	/* Setup the IO.  This is just copied from the demo supplied by SoftBaugh
	 for the ES449 demo board. */
	/*P1SEL = 0x32;
	P2SEL = 0x00;
	P3SEL = 0x00;
	P4SEL = 0xFC;
	P5SEL = 0xFF;*/
}
/*-----------------------------------------------------------*/

/* The idle hook is just a copy of the standard integer maths tasks.  See
Demo/Common/integer.c for rationale. */

void vApplicationIdleHook( void )
{
/* These variables are all effectively set to constants so they are volatile to
ensure the compiler does not just get rid of them. */
volatile long lValue;

	/* Keep performing a calculation and checking the result against a constant. */
	for( ;; )
	{
		/* Perform the calculation.  This will store partial value in
		registers, resulting in a good test of the context switch mechanism. */
		lValue = intgCONST1;
		lValue += intgCONST2;

		/* Yield in case cooperative scheduling is being used. */
		#if configUSE_PREEMPTION == 0
		{
			taskYIELD();
		}
		#endif

		/* Finish off the calculation. */
		lValue *= intgCONST3;
		lValue /= intgCONST4;

		/* If the calculation is found to be incorrect we stop setting the
		TaskHasExecuted variable so the check task can see an error has
		occurred. */
		if( lValue != intgEXPECTED_ANSWER ) /*lint !e774 volatile used to prevent this being optimised out. */
		{
			/* Don't bother with mutual exclusion - it is only read from the
			check task and never written. */
			xLocalError = pdTRUE;
		}
		/* Yield in case cooperative scheduling is being used. */
		#if configUSE_PREEMPTION == 0
		{
			taskYIELD();
		}
		#endif

        ulIdleLoops++;

        /* Place the processor into low power mode. */
        LPM0;
	}
}

static portTASK_FUNCTION( vMPDTask, pvParameters )
{
	TickType_t xRate, xLastTime;
	mpu9050_t mpu9050;
	HMC_Data_t hmc_data;
	BaseType_t uxBits = 0;
	const TickType_t xTickToWait = 100;
	//uint8_t counter[2] = {0, 0};

	/* The parameters are not used. */
	( void ) pvParameters;
	
	Init_MPU9050();
	HMC_Init();
	xRate = 100;
	xRate /= portTICK_PERIOD_MS;
	
	/* We need to initialise xLastFlashTime prior to the first call to 
	vTaskDelayUntil(). */
	xLastTime = xTaskGetTickCount();

	for(;;)
	{
		uxBits = xEventGroupWaitBits(xEventGroup, MPU_DATA_READY|HMC_DATA_READY, pdTRUE, pdFALSE, xTickToWait);

		if (uxBits & MPU_DATA_READY)
		{
			MPU9050_Read(&mpu9050);
			vParTestSetLED( 4, 1);
			vParTestSetLED( 5, 0);
			/*if (counter[0]++ > 50)
			{
				printf("MPU:\nACCEL:\t");
				printf("x=%d\ty=%d\tz=%d\n", mpu9050.accel.x, mpu9050.accel.y, mpu9050.accel.z);
				printf("GYRO: x=%d\ty=%d\tz=%d\n", mpu9050.gyro.x, mpu9050.gyro.y, mpu9050.gyro.z);
				counter[0] = 0;
			}*/
		}
		if (uxBits & HMC_DATA_READY)
		{
			HMC_Read(&hmc_data);
			vParTestSetLED(2, 1);
			vParTestSetLED(3, 0);
			/*if (counter[1]++ > 4)
			{
				printf("HMC:\tDirect: x=%d\ty=%d\tz=%d\n", hmc_data.direct.x, hmc_data.direct.y, hmc_data.direct.z);
				printf("\tAngle: x=%.2f\ty=%.2f\tz=%.2f\n", hmc_data.angle.x, hmc_data.angle.x, hmc_data.angle.x);
				counter[1] = 0;
			}*/
		}
		if (!(uxBits & (MPU_DATA_READY|HMC_DATA_READY)))
		{
			vParTestSetLED(2, 0);
			vParTestSetLED(3, 1);
			vParTestSetLED( 4, 0);
			vParTestSetLED( 5, 1);
			vTaskDelayUntil(&xLastTime, xRate);
			//printf("\n#########Error##########\n\n");
		}
	}
}

#if 0
int fputc(int ch, FILE *f)
{
	
	xSerialPutChar(0, ch, 0);
	return ch;
}

int GetKey(void)
{
	int ch = 0;
	xSerialGetChar(0, &ch, 0);
	return ch;
}
#endif
/*
void Delay(uint16_t us)
{
	while(us--)
		__delay_cycles((long)8);
}*/
