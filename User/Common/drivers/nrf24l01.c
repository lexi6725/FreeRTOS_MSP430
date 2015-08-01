#include "FreeRTOS.h"
#include "Task.h"
#include "event_groups.h"
#include "portmacro.h"
#include "nrf24l01.h"

static uint8_t nRF_Address[nRF_ADDR_WIDTH] = {0x59, 0x12, 0x67, 0x67, 0x25};
static uint8_t nRF_Init_Flag = 0;
EventGroupHandle_t xEventGroup;
#define HW_SPI

void nRF_Init(void)
{
	P4SEL	&= ~(BIT4 + BIT5);
	P4DIR	|= (BIT4 + BIT5);
	
#if defined(HW_SPI)
	P5DIR	|= (BIT2+BIT3);
	P5DIR	&= ~BIT1;
	P5SEL	|= (BIT1+BIT2+BIT3);
	U1CTL	= CHAR + SYNC + MM + SWRST;
	U1TCTL	= SSEL1 + STC;
	U1BR0	= 0x02;
	U1BR1	= 0x00;
	U1MCTL	= 0x00;
	ME2		|= USPIE1;
	U1CTL	&= ~SWRST;
	//IE2		|= URXIE1 + UTXIE1;
	IFG2 	&= ~(UTXIFG1+URXIFG1);
	vTaskDelay(1);
#else
	P5SEL	&= ~(BIT1+BIT2+BIT3);
	P5DIR	|= (BIT2+BIT3);
	P5DIR	&= ~BIT1;


	nRF_SCK_0;
#endif
	P1DIR	&= ~BIT4;		//P1.4 is IRQ Pin,  Data In
	P1IES	|= BIT4;		// P1.4 High To Low trigger
	P1IE	|= BIT4;

	nRF_CE_0;
	nRF_CSN_1;

	if ((xEventGroup = xEventGroupCreate()) != NULL)
		nRF_Init_Flag = 1;
}

#if !defined(HW_SPI)
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
#endif

uint8_t spi_rw(uint8_t byte)
{
#if defined(HW_SPI)
	uint8_t timeout = 0xFF;
	U1TXBUF = byte;
	
	while(timeout--)
	{
		if (IFG2&URXIFG1)	// wait Recevier Data
			break;
	}

	byte = U1RXBUF;
	IFG2 	&= ~(UTXIFG1+URXIFG1);
#else
	uint8_t bit;

	for (bit = 0; bit < 8; bit++)
	{
		mosi_pin(byte&0x80);
		byte = (byte << 1);
		nRF_SCK_1;
		byte	|= miso_pin();
		nRF_SCK_0;
	}
#endif
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
	if (nRF_Init_Flag == 0)
		nRF_Init();
		
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
	if (nRF_Init_Flag == 0)
		nRF_Init();
		
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
	uint8_t retvalue = 0;
	BaseType_t uxBits;
	const TickType_t xTicksToWait = 3;

	nrf_tx_mode();
	
	nRF_CE_0;

	nrf_rw_reg(FLUSH_TX, 0xFF);
	nrf_write_buf(WR_TX_PLOAD, pbuf, len);

	nRF_CE_1;

	uxBits = xEventGroupWaitBits(xEventGroup, nRF_State_TX_OK|nRF_State_TX_MAX, pdTRUE, pdFALSE, xTicksToWait);

	if (uxBits & nRF_State_TX_OK)
	{
		retvalue = TX_OK;
	}
	else if (uxBits & nRF_State_TX_MAX)
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
	BaseType_t uxBits;
	uint8_t retvalue = 0;
	const TickType_t xTickToWait = 1000;

	uxBits = xEventGroupWaitBits(xEventGroup, nRF_State_RX_OK, pdTRUE, pdFALSE, xTickToWait);

	if (uxBits & nRF_State_RX_OK)
	{
		nRF_CE_0;
		nrf_read_buf(RD_RX_PLOAD, pbuf, len);
		nRF_CE_1;
		retvalue = RX_OK;
	}
	else
		retvalue = TIMEOUT;

	nRF_CE_0;
	nrf_rw_reg(WRITE_REG+STATUS, 0xFF);
	nRF_CE_1;

	return retvalue;
}

void nrf_isr(void)
{
	BaseType_t xResult, xHigherPriorityTaskWoken;
	uint8_t RegValue;

	RegValue = nrf_read_byte(STATUS);
	nrf_rw_reg(WRITE_REG+STATUS, 0xff);

	xHigherPriorityTaskWoken = pdFALSE;

	if (RegValue & TX_OK)
	{
		xResult = xEventGroupSetBitsFromISR(xEventGroup, nRF_State_TX_OK, &xHigherPriorityTaskWoken);
	}
	else if (RegValue & MAX_TX)
	{
		xResult = xEventGroupSetBitsFromISR(xEventGroup, nRF_State_TX_MAX, &xHigherPriorityTaskWoken);
	}
	if (RegValue & RX_OK)
	{
		xResult = xEventGroupSetBitsFromISR(xEventGroup, nRF_State_RX_OK, &xHigherPriorityTaskWoken);
	}

	if (xResult != pdFAIL)
	{
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
	if (P1IFG & BIT4)
		nrf_isr();

	P1IFG = 0;
}