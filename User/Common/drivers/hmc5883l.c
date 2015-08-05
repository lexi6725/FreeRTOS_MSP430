#include "FreeRTOS.h"
#include "i2c.h"
#include "hmc5883l.h"
#include "event_groups.h"
#include "main.h"
#include <math.h>
extern EventGroupHandle_t xEventGroup;

void HMC_Init(void)
{
	HMC_CFG_t hmc_cfg;
	
	I2C_Init();

	hmc_cfg.cfg_A= (SAMPLE_AVG_4|DT_RATE_30);
	hmc_cfg.cfg_B= GAIN_1_3;
	hmc_cfg.cfg_Mode= MODE_MUL;
	I2C_Write(HMC_SLV_ADDR, CFG_A, (uint8_t *)&hmc_cfg, sizeof(hmc_cfg));

	// Init HMC Interrupt Pin (P1.5)
	P1SEL	&= ~BIT5;
	P1DIR	&= ~BIT5;
	P1IES	&= ~BIT5; 	// Rising edge
	P1IE	|= BIT5;
}

uint8_t HMC_Detect(void)
{
	uint8_t detect[6];
	
	I2C_Init();

	I2C_Read(HMC_SLV_ADDR, HMC_STATUS, detect, 4);

	if (detect[1] == DETECT_A_VALUE && detect[2] == DETECT_B_VALUE && detect[3] == DETECT_C_VALUE)
	{
		return pdTRUE;
	}

	return pdFALSE;
}
uint8_t HMC_GetStatus(void)
{
	uint8_t status;

	I2C_Read(HMC_SLV_ADDR, HMC_STATUS, &status, 1);

	return status;
}

void HMC_ISR(void)
{
	BaseType_t xResult, xHigherPriorityTaskWoken;

	xResult = xEventGroupSetBitsFromISR(xEventGroup, HMC_DATA_READY, &xHigherPriorityTaskWoken);

}

uint8_t HMC_Read(HMC_Data_t *hmc_data)
{
	uint8_t DataBuf[6];
	BaseType_t uxBits = 0;
	const TickType_t xTickToWait = 100;
	float tmp, r;

	uxBits = xEventGroupWaitBits(xEventGroup, HMC_DATA_READY, pdTRUE, pdFALSE, xTickToWait);

	if (uxBits & HMC_DATA_READY)
	{
		I2C_Read(HMC_SLV_ADDR, DOUT_X_MSB, DataBuf, 6);

		hmc_data->direct.x = (DataBuf[0] << 8 | DataBuf[1]);
		hmc_data->direct.y = (DataBuf[2] << 8 | DataBuf[3]);
		hmc_data->direct.z = (DataBuf[4] << 8 | DataBuf[5]);

		r = sqrt((double)hmc_data->direct.x*(double)hmc_data->direct.x
				+(double)hmc_data->direct.y*(double)hmc_data->direct.y
				+(double)hmc_data->direct.z*(double)hmc_data->direct.z);

		tmp = (double)hmc_data->direct.x/r;
		tmp = acos(tmp);
		hmc_data->angle.x = tmp*180.0/3.1415926;
		
		tmp = (double)hmc_data->direct.y/r;
		tmp = acos(tmp);
		hmc_data->angle.y = tmp*180.0/3.1415926;
		
		tmp = (double)hmc_data->direct.z/r;
		tmp = acos(tmp);
		hmc_data->angle.z = tmp*180.0/3.1415926;

		return pdTRUE;
	}

	return pdFALSE;

}


