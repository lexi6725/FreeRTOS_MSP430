#include "FreeRTOS.h"
#include "i2c.h"
#include "main.h"
static uint8_t I2C_Status = 0;

void I2C_Init(void)
{
	if (!(I2C_Status & I2C_FLAG_INIT))
	{
		I2C_SDA_IN;
		I2C_SCL_IN;
		I2C_SDA_0;
		I2C_SCL_0;
		I2C_Status |= I2C_FLAG_INIT;
	}
}

void I2C_Start(void)
{
	I2C_SDA_IN;
	I2C_SCL_IN;
	Delay(1);
	I2C_SDA_OUT;
	Delay(1);
	I2C_SCL_OUT;
}

void I2C_Stop(void)
{
	I2C_SDA_OUT;
	I2C_SCL_IN;
	Delay(1);
	I2C_SDA_IN;
	Delay(1);
}

void I2C_WriteByte(uint8_t data)
{
	uint8_t index;
	uint8_t timeout = 0x20;

	//I2C_SDA_OUT;
	for (index = 0; index < 8; index++)
	{
		if (data & 0x80)
			I2C_SDA_IN;
		else
			I2C_SDA_OUT;

		I2C_SCL_IN;
		data<<=1;
		I2C_SCL_OUT;
	}

	I2C_SDA_IN;
	I2C_SCL_IN;
	while(timeout-->0)
	{
		if (!(I2C_SDA))
			break;
	}
	I2C_SCL_OUT;
}

uint8_t I2C_ReadByte(uint8_t ack)
{
	uint8_t data = 0;
	uint8_t index;

	I2C_SDA_IN;

	for (index = 0; index < 8; index++)
	{
		data <<= 0x01;
		I2C_SCL_IN;
		if (I2C_SDA)
			data |= 0x01;
		I2C_SCL_OUT;
	}

	if (ack == 0)
		I2C_SDA_OUT;
		
	I2C_SCL_IN;
	Delay(1);
	I2C_SCL_OUT;

	return data;
}

uint8_t I2C_Read(uint8_t devaddr, uint8_t dstaddr, uint8_t *data, uint8_t datelen)
{
	uint8_t index;
	
	I2C_Start();
	I2C_WriteByte(devaddr);
	I2C_WriteByte(dstaddr);

	I2C_Start();
	I2C_WriteByte(devaddr|0x01);
	for (index = 0; index < datelen-1; ++index)
	{
		data[index] = I2C_ReadByte(0);
	}

	data[index] = I2C_ReadByte(1);

	I2C_Stop();

	return 0;
}

uint8_t I2C_Write(uint8_t devaddr, uint8_t dstaddr, uint8_t *data, uint8_t datelen)
{
	uint8_t index;

	I2C_Start();
	I2C_WriteByte(devaddr);
	I2C_WriteByte(dstaddr);

	for (index = 0; index < datelen; ++index)
	{
		I2C_WriteByte(data[index]);
	}

	I2C_Stop();

	return 0;
}

