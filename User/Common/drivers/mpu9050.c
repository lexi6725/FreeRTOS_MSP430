#include "FreeRTOS.h"
#include "event_groups.h"
#include "main.h"
#include "mpu9050.h"
#include "i2c.h"
extern EventGroupHandle_t xEventGroup;

void Init_MPU9050(void)
{
	uint8_t tmp;

	I2C_Init();

	// Init MPU Interrupt Pin (P1.6)
	P1SEL	&= ~BIT6;
	P1DIR	&= ~BIT6;
	P1IES	|= BIT6; 	// Rising edge
	P1IE	|= BIT6;


	tmp = 0x00;
	I2C_Write(MPUSLVADDR, PWR_MGMT_1, &tmp, 1);
	
	tmp = 0x07;
	I2C_Write(MPUSLVADDR, SMPLRT_DIV, &tmp, 1);
	
	tmp = 0x06;
	I2C_Write(MPUSLVADDR, MPU_CONFIG, &tmp, 1);
	
	tmp = 0x18;
	I2C_Write(MPUSLVADDR, GYRO_CONFIG, &tmp, 1);
	
	tmp = 0x01;
	I2C_Write(MPUSLVADDR, ACCEL_CONFIG, &tmp, 1);

	tmp = 0x01;
	I2C_Write(MPUSLVADDR, INT_ENABLE, &tmp, 1);
}

void MPU9050_ISR(void)
{
	BaseType_t xResult, xHigherPriorityTaskWoken;

	xResult = xEventGroupSetBitsFromISR(xEventGroup, MPU_DATA_READY, &xHigherPriorityTaskWoken);

	if (xResult != pdFAIL)
	{
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

uint8_t MPU9050_Read(mpu9050_t *mpu9050)
{
	BaseType_t uxBits = 0;
	const TickType_t xTickToWait = 10;

	uxBits = xEventGroupWaitBits(xEventGroup, MPU_DATA_READY, pdTRUE, pdFALSE, xTickToWait);

	if (uxBits & MPU_DATA_READY)
	{
		I2C_Read(MPUSLVADDR, ACCEL_XOUT_H, (uint8_t *)&mpu9050->accel.x, sizeof(triaxial_t));
		I2C_Read(MPUSLVADDR, GYRO_XOUT_H, (uint8_t *)&mpu9050->gyro.x, sizeof(triaxial_t));

		if (mpu9050->accel.x != 0)
			return pdTRUE;
		else
			return pdFALSE;
	}
	return pdFALSE;
}


