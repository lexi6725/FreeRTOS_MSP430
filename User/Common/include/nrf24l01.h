#ifndef __NRF24L01_H__
#define __NRF24L01_H__

#define nRF_ADDR_WIDTH		5
#define nRF_PLOAD_WIDTH		32

/**********************************************************
* NRF24L01�Ĵ�����������
***********************************************************/
#define READ_REG			0x00		//�����üĴ�������5λΪ�Ĵ�����ַ
#define WRITE_REG			0x20		//д���üĴ�������5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD			0x61		//��RX��Ч���ݣ�1~32�ֽ�
#define WR_TX_PLOAD			0xA0		//дTX��Ч���ݣ�1~32�ֽ�
#define	FLUSH_TX			0xE1		//���TX_FIFO�Ĵ���������ģʽ����
#define FLUSH_RX			0xE2		//���RX_FIFO�Ĵ���������ģʽ��ʹ��
#define REUSE_TX_PL			0xE3		//����ʹ����һ�����ݣ�CEΪ�ߣ����ݰ�����������
#define	NOP					0xFF		//�ղ���������������״̬�Ĵ���

/**********************************************************
* NRF24L01�Ĵ�����ַ
***********************************************************/
#define CONFIG				0x00		//���üĴ�����ַ��
										//Bit0:1 ����ģʽ 0����ģʽBit1: 1 PowerUp      0 PowerDown
										//Bit2:1 CRC two byte  0 CRC one byte     Bit3: 1 Enable CRC 0 Disable CRC 
										//Bit4: 1 Enabel IRQ 0 DisableIRQ               Bit5: 1 TX_IRQ
										//Bit6: 1 TX_IRQ						  Bit7: Only '0'
#define	EN_AA				0x01		//ʹ���Զ�Ӧ����Bit[0:5]:��Ӧͨ��0~5
#define EN_RXADDR			0x02		//���յ�ַʹ��,Bit[0:5]:��Ӧͨ��0~5
#define SETUP_AW			0x03		//���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR			0x04		//�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH				0x05		//RFͨ��,bit6:0,����ͨ��Ƶ��;
#define RF_SETUP			0x06		//RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define	STATUS				0x07		//״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
										//bit5:���ݷ�������ж�;bit6:���������ж�;
#define MAX_TX				0x10		//�ﵽ����ʹ����ж�
#define	TX_OK				0x20		//TX��������ж�
#define	RX_OK				0x40		//���յ������ж�

#define OBSERVE_TX			0x08		//���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD					0x09		//�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0			0x0A		//����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1			0x0B		//����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2			0x0C		//����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define	RX_ADDR_P3			0x0D		//����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define	RX_ADDR_P4			0x0E		//����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define	RX_ADDR_P5			0x0F		//����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define	TX_ADDR				0x10		//���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define	RX_PW_P0			0x11		//��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1			0x12		//��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define	RX_PW_P2			0x13		//��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3			0x14		//��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4			0x15		//��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define	RX_PW_P5			0x16		//��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define	FIFO_STATUS			0x17		//FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
										//bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;

#define nRF_CE_1		(P4OUT	|= BIT4)
#define nRF_CE_0		(P4OUT	&= ~BIT4)
#define nRF_CSN_1		(P4OUT	|= BIT5)
#define nRF_CSN_0		(P4OUT	&= ~BIT5)
#define nRF_IRQ()		(P1IN	& BIT4)
#define nRF_SCK_0		(P5OUT	&= ~BIT3)
#define nRF_SCK_1		(P5OUT	|= BIT3)
#define nRF_MOSI_0		(P5OUT	&= ~BIT2)
#define nRF_MOSI_1		(P5OUT	|= BIT2)
#define nRF_MISO_IN()	((P5IN>>1)&0x01)

#define TIMEOUT		0x01

void SPI1_Init(void);
void nrf_tx_mode(void);
void nrf_rx_mode(void);
uint8_t nrf_start_tx(uint8_t *pbuf, uint8_t len);
uint8_t nrf_start_rx(uint8_t *pbuf, uint8_t len);


#endif
