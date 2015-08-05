#ifndef __HMC5883L_H_
#define __HMC5883L_H_
#include "FreeRTOS.h"

#define HMC_SLV_ADDR	0x3C

#define CFG_A			0x00
#define CFG_B			0x01
#define CFG_MODE		0x02
#define DOUT_X_MSB		0x03
#define DOUT_X_LSB		0x04
#define DOUT_Y_MSB		0x05
#define DOUT_Y_LSB		0x06
#define DOUT_Z_MSB		0x07
#define DOUT_Z_LSB		0x08
#define HMC_STATUS		0x09
#define HMC_DETECT_A	0x0A
#define HMC_DETECT_B	0x0B
#define HMC_DETECT_C	0x0C

// Configure Register A Bit
#define SAMPLE_AVG_1	0x00
#define SAMPLE_AVG_2	0x20
#define SAMPLE_AVG_4	0x40
#define SAMPLE_AVG_8	0x60

#define DT_RATE_0_75	0x00
#define DT_RATE_1_5		0x04
#define DT_RATE_3		0x08
#define DT_RATE_7_5		0x0C
#define DT_RATE_15		0x10
#define DT_RATE_30		0x14
#define DT_RATE_75		0x18

#define GAIN_0_88		0x00
#define GAIN_1_3		0x20
#define GAIN_1_9		0x40
#define GAIN_2_5		0x60
#define GAIN_4_0		0x80
#define GAIN_4_7		0xA0
#define GAIN_5_6		0xC0
#define GAIN_8_1		0xE0

#define MODE_MUL		0x00
#define MODE_SINGLE		0x01
#define MODE_IDLE		0x02

#define STATE_RDY		0x01
#define STATE_LOCK		0x02

#define DETECT_A_VALUE	0x48
#define DETECT_B_VALUE	0x34
#define DETECT_C_VALUE	0x33

typedef struct
{
	uint8_t cfg_A;
	uint8_t cfg_B;
	uint8_t cfg_Mode;
} HMC_CFG_t;

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
} direct_t;

typedef struct
{
	float x;
	float y;
	float z;
} angle_t;

typedef struct
{
	direct_t direct;
	angle_t angle;
}HMC_Data_t;


void HMC_Init(void);
uint8_t HMC_Detect(void);
uint8_t HMC_GetStatus(void);
uint8_t HMC_Read(HMC_Data_t *hmc_data);
void HMC_ISR(void);

#endif
