#include "FreeRTOS.h"
#include "Task.h"
#include "nrf24l01.h"

uint8_t nRF_Address[nRF_ADDR_WIDTH] = {0x59, 0x42, 0x67, 0x67, 0x92};

void SPI1_Init(void)
{
	P4SEL	&= ~(BIT4 + BIT5);
	P4DIR	|= (BIT4 + BIT5);
	
#if defined(HW_SPI)
	P5SEL	|= (BIT1+BIT2+BIT3);
	U1CTL	= CHAR + SYNC + MM + SWRST;
	U1TCTL	= SSEL1 + STC;
	U1BR0	= 0x02;
	U1BR1	= 0x00;
	U1MCTL	= 0x00;
	ME2		|= USPIE1;
	U1CTL	&= ~SWRST;
	IE2		|= URXIE1 + UTXIE1;
#else
	P5SEL	&= ~(BIT1+BIT2+BIT3);
	P5DIR	|= (BIT2+BIT3);
	P5DIR	&= ~BIT1;

	P1DIR	&= ~BIT4;		//P1.4 is IRQ Pin,  Data In

	nRF_SCK_0;
#endif

	nRF_CE_0;
	nRF_CSN_1;
}

void mosi_pin(uint8_t state)
{
	if (state)
		nRF_MOSI_1;
	else
		nRF_MOSI_0;
}

uint8_t miso_pin(void)
{
	return nRF_MISO_IN();
}

uint8_t spi_rw(uint8_t byte)
{
	uint8_t bit;

	for (bit = 0; bit < 8; bit++)
	{
		mosi_pin(byte&0x80);
		byte = (byte << 1);
		nRF_SCK_1;
		byte	|= miso_pin();
		nRF_SCK_0;
	}
	return byte;
}

uint8_t nrf_rw_reg(uint8_t reg, uint8_t value)
{
	uint8_t status;

	nRF_CSN_0;
	status = spi_rw(reg);
	spi_rw(value);
	nRF_CSN_1;

	return status;
}

uint8_t nrf_read_byte(uint8_t reg)
{
	uint8_t ret_value;

	nRF_CSN_0;
	spi_rw(reg);
	ret_value = spi_rw(0);
	nRF_CSN_1;

	return ret_value;
}

uint8_t nrf_read_buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status, bit;

	nRF_CSN_0;
	status = spi_rw(reg);

	for (bit = 0; bit < len; bit++)
		pBuf[bit] = spi_rw(0);

	nRF_CSN_1;

	return status;
}

uint8_t nrf_write_buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status, bit;

	nRF_CSN_0;
	status = spi_rw(reg);

	for (bit = 0; bit < len; bit++)
		spi_rw(*pBuf++);

	nRF_CSN_1;

	return status;
}

void nrf_rx_mode(void)
{
	nRF_CE_0;	
	nrf_write_buf(WRITE_REG+TX_ADDR, nRF_Address, nRF_ADDR_WIDTH);
	nrf_write_buf(WRITE_REG+RX_ADDR_P0, nRF_Address, nRF_ADDR_WIDTH);
	nrf_rw_reg(WRITE_REG+EN_AA, 0x01);
	nrf_rw_reg(WRITE_REG+EN_RXADDR, 0x01);
	nrf_rw_reg(WRITE_REG+RF_CH, 40);
	nrf_rw_reg(WRITE_REG+RX_PW_P0, nRF_PLOAD_WIDTH);
	nrf_rw_reg(WRITE_REG+RF_SETUP, 0x0F);
	nrf_rw_reg(WRITE_REG+CONFIG, 0x0F);

	nrf_rw_reg(WRITE_REG+STATUS, 0xFF);
	nRF_CE_1;
}

void nrf_tx_mode(void)
{
	/*!< Select the nRF: Chip Select low */
	nRF_CE_0;

	nrf_write_buf(WRITE_REG+TX_ADDR, nRF_Address, nRF_ADDR_WIDTH);
	nrf_write_buf(WRITE_REG+TX_ADDR, nRF_Address, nRF_ADDR_WIDTH);

	nrf_rw_reg(WRITE_REG+EN_AA, 0x01);
	nrf_rw_reg(WRITE_REG+EN_RXADDR, 0x01);
	nrf_rw_reg(WRITE_REG+SETUP_RETR, 0x03);
	nrf_rw_reg(WRITE_REG+RF_CH, 40);
	nrf_rw_reg(WRITE_REG+RF_SETUP, 0x0F);
	nrf_rw_reg(WRITE_REG+RX_PW_P0, nRF_PLOAD_WIDTH);
	nrf_rw_reg(WRITE_REG+CONFIG, 0x0E);
	
	nrf_rw_reg(WRITE_REG+STATUS, 0xFF);
	/*!< Deselect the nRF: Chip Select high */
	nRF_CE_1;
}

uint8_t nrf_start_tx(uint8_t *pbuf, uint8_t len)
{
	uint8_t status;
	uint16_t timeout = 1;
	uint8_t retvalue = 0;

	nrf_tx_mode();
	
	nRF_CE_0;

	nrf_rw_reg(FLUSH_TX, 0xFF);
	nrf_write_buf(WR_TX_PLOAD, pbuf, len);

	nRF_CE_1;

	while((nRF_IRQ()))
	{
		if (timeout-- == 0)
			return TIMEOUT;
		vTaskDelay(1);
	}

	status = nrf_read_byte(STATUS);
	nrf_rw_reg(WRITE_REG+STATUS, 0xFF);

	if (status & TX_OK)
	{
		retvalue = TX_OK;
	}
	else if (status & MAX_TX)
	{
		retvalue = MAX_TX;
	}
	
	nRF_CE_0;
	nrf_rw_reg(FLUSH_TX, 0xff);
	nRF_CE_1;
	
	nrf_rx_mode();
	
	return retvalue;
}

uint8_t nrf_start_rx(uint8_t *pbuf, uint8_t len)
{
	uint8_t status;
	uint16_t timeout = 1;
	uint8_t retvalue = 0;

	while((nRF_IRQ()))
	{
		if (timeout-- == 0)
			return TIMEOUT;
		vTaskDelay(1);
	}

	status = nrf_read_byte(STATUS);
	nrf_rw_reg(WRITE_REG+STATUS, 0xFF);

	if (status & RX_OK)
	{
		nRF_CE_0;
		nrf_read_buf(RD_RX_PLOAD, pbuf, len);
		nrf_rw_reg(FLUSH_RX, 0xFF);
		nRF_CE_1;
		retvalue = RX_OK;
	}
	else
		retvalue = TIMEOUT;

	return retvalue;
}
