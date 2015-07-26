#include "FreeRTOS.h"
#include "pwm.h"

static uint8_t PWM_Flag = 0;

void Init_PWM(void)
{
	P6SEL	&= ~DirMask;
	P6DIR	|= DirMask;
	P6OUT	&= ~DirMask;

	P4DIR	|= (BIT1+BIT2);
	P4SEL	|= (BIT1+BIT2);

	TBCCTL1	|= OUTMOD_7;				// PWM OUT Mode: Reset/Set
	TBCCTL2	|= OUTMOD_7;				// PWM OUT Mode: Reset/Set
	
	TBCCR1	= CONFIG_DEFAULT_PWM_RATE%PWM_MAX_RATE;
	TBCCR2	= CONFIG_DEFAULT_PWM_RATE%PWM_MAX_RATE;
	TBCCR0	= PWM_MAX_RATE;						// 50uS = 20KHz
}

void Enable_PWM(void)
{
	TBCTL	|= TASSEL_2+ID_3+TBCLR+MC_1;			// SMCLK/div8 and Up Mode 
	PWM_Flag = 1;
}

void Disable_PWM(void)
{
	TBCTL	= 0|TBCLR;
	PWM_Flag = 0;
}

void ChangeRate(uint8_t rate)
{
	if (!IsPwmRate(rate))
		return;

	TBCCR1	= rate%(PWM_MAX_RATE+1);
	TBCCR2	= rate%(PWM_MAX_RATE+1);
}

uint8_t GetRate(void)
{
	return TBCCR1;
}

void ChangeDirs(uint8_t dirs)
{
	uint8_t dir = P6OUT;
	
	if (!IsDirs(dirs))
		return;

	if ((dirs != DirStop) && (PWM_Flag == 0))
		Enable_PWM();
	
	P6OUT &= ~DirMask;

	switch(dirs)
	{
		case DirFront:
			P6OUT	|= 0x11;
			break;
		case DirBack:
			P6OUT	|= 0x22;
			break;
		case DirLeft:
			P6OUT	|= 0x12;
			break;
		case DirRight:
			P6OUT	|= 0x21;
			break;
		case DirStop:
			Disable_PWM();
			P6OUT	&= ~DirMask;
			break;
		default:
			break;
	}
}

void ControlProcess(uint8_t cmd)
{
	switch(cmd)
	{
		case CMD_ADD_RATE:
			ChangeRate(GetRate()+1);
			break;
		case CMD_RDU_RATE:
			ChangeRate(GetRate()-1);
			break;
		case CMD_DIR_FRONT:
			ChangeDirs(DirFront);
			break;
		case CMD_DIR_BACK:
			ChangeDirs(DirBack);
			break;
		case CMD_DIR_LEFT:
			ChangeDirs(DirLeft);
			break;
		case CMD_DIR_RIGHT:
			ChangeDirs(DirRight);
			break;
		case CMD_STOP:
			ChangeDirs(DirStop);
			break;
		default:
			break;	
	}
}
