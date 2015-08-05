#include "FreeRTOS.h"
#include "i2c.h"
#include "main.h"
static uint8_t I2C_Status = 0;

void I2C_Init(void)
{
	if (!(I2C_Status & I2C_FLAG_INIT))
	{
		I2C_SDA_IN;
		I2C_SCL_OUT;
		I2C_SDA_0;
		I2C_SCL_1;
		I2C_Status |= I2C_FLAG_INIT;
	}
}

void I2C_Start(void)
{
	I2C_SDA_IN;
	I2C_SCL_1;
	Delay(2);
	I2C_SDA_OUT;
	Delay(2);
	I2C_SCL_0;
}

void I2C_Stop(void)
{
	I2C_SDA_OUT;
	I2C_SCL_1;
	Delay(2);
	I2C_SDA_IN;
	Delay(2);
}

/*
 * 0: ACK   1:NAK
*/
void I2C_SendAck(uint8_t ack)
{
	if (ack)
		I2C_SDA_IN;
	else
		I2C_SDA_OUT;

	I2C_SCL_1;
	Delay(2);
	I2C_SCL_0;
}

uint8_t I2C_RecvAck(void)
{
	uint8_t retValue = 0;

	I2C_SDA_IN;
	I2C_SCL_1;
	Delay(1);
	retValue = I2C_SDA;
	Delay(2);
	I2C_SCL_0;
	
	return retValue;
}

void I2C_WriteByte(uint8_t data)
{
	uint8_t index;

	I2C_SDA_OUT;
	for (index = 0; index < 8; index++)
	{
		if ((data<<index) & 0x80)
			I2C_SDA_IN;
		else
			I2C_SDA_OUT;

		I2C_SCL_1;
		Delay(2);
		I2C_SCL_0;
		Delay(1);
	}
	Delay(5);
}

uint8_t I2C_ReadByte(void)
{
	uint8_t data = 0;
	uint8_t index;

	I2C_SDA_IN;

	for (index = 0; index < 8; index++)
	{
		data <<= 0x01;
		I2C_SCL_1;
		Delay(2);
		if (I2C_SDA)
			data |= 0x01;
		I2C_SCL_0;
		Delay(1);
	}
	Delay(5);

	return data;
}

uint8_t I2C_Read(uint8_t devaddr, uint8_t dstaddr, uint8_t *data, uint8_t datelen)
{
	uint8_t index;
	
	I2C_Start();
	I2C_WriteByte(devaddr);
	I2C_RecvAck();
	I2C_WriteByte(dstaddr);
	I2C_RecvAck();

	I2C_Start();
	I2C_WriteByte(devaddr|0x01);
	I2C_RecvAck();
	for (index = 0; index < datelen; ++index)
	{
		data[index] = I2C_ReadByte();
		if (index == (datelen-1))
			I2C_SendAck(1);
		else
			I2C_SendAck(0);
	}

	I2C_Stop();
	Delay(5);

	return 0;
}

uint8_t I2C_Write(uint8_t devaddr, uint8_t dstaddr, uint8_t *data, uint8_t datelen)
{
	uint8_t index;

	I2C_Start();
	I2C_WriteByte(devaddr);
	I2C_RecvAck();
	I2C_WriteByte(dstaddr);
	I2C_RecvAck();

	for (index = 0; index < datelen; ++index)
	{
		I2C_WriteByte(data[index]);
		I2C_RecvAck();
	}

	I2C_Stop();
	Delay(5);

	return 0;
}

