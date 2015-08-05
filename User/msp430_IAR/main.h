#ifndef __MAIN_H_
#define __MAIN_H_

#define nRF_State_TX_OK		(0x01<<0)		// 1. 发送数据成功
#define nRF_State_TX_MAX	(0x01<<1)		// 1. 发送数据最大次数
#define nRF_State_RX_OK		(0x01<<2)		// 1. 接收数据成功
#define HMC_DATA_READY		(0x01<<3)
#define MPU_DATA_READY		(0x01<<4)


#define CPU_CLOCK	((double)8000000.0)
#define pd_toMS(x)	((x)*1000.0)	

#define Delay(x)	__delay_cycles((long)(CPU_CLOCK*(double)(x)/1000000.0))

#endif
