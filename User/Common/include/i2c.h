#ifndef __I2C_H_
#define __I2C_H_

#define I2C_FLAG_INIT		0x01

#define I2C_SDA_IN	(P6DIR	&= ~BIT2)
#define I2C_SDA_OUT	(P6DIR	|= BIT2)
#define I2C_SDA_0	(P6OUT	&= ~BIT2)
#define I2C_SDA_1	(P6OUT	|= BIT2)
#define I2C_SDA		(P6IN	& BIT2)
#define I2C_SCL_IN	(P6DIR	&= ~BIT3)
#define I2C_SCL_OUT	(P6DIR	|= BIT3)
#define I2C_SCL_0	(P6OUT	&= ~BIT3)
#define I2C_SCL_1	(P6OUT	|= BIT3)

void I2C_Init(void);
uint8_t I2C_Read(uint8_t devaddr, uint8_t dstaddr, uint8_t *data, uint8_t datelen);
uint8_t I2C_Write(uint8_t devaddr, uint8_t dstaddr, uint8_t *data, uint8_t datelen);

#endif
