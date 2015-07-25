#ifndef __PWM_H_
#define __PWM_H_

#define Rotate_Left		(0x00)
#define Rotate_Right	(0x01)
#define Rotate_All		(0x02)
#define IsRotate(x)		(((x)==Rotate_Left) || \
						 ((x)==Rotate_Right) || \
						 ((x)==Rotate_All))

#define DirMask			(0x33)		/* P6[0,1,4,5] */
#define DirFront		(0x00)
#define DirBack			(0x11)
#define DirLeft			(0x10)
#define DirRight		(0x01)
#define DirStop			(0x22)
//#define DirLeftFront	(DirLeft|DirFront)
//#define DirRightFront	(DirRight|DirFront)
//#define DirLeftBack		(DirLeft|DirBack)
//#define DirRightBack	(DirRight|DirBack)
#define IsDirs(x)	(((x)==DirLeft) || \
					 ((x)==DirRight) || \
					 ((x)==DirFront) || \
					 ((x)==DirStop) || \
					 ((x)==DirBack))

#define CONFIG_DEFAULT_PWM_RATE	30
#define PWM_MAX_RATE			50

#define IsPwmRate(x)		((x) <= PWM_MAX_RATE)

#define DirPortOut()		P6SEL &= 0x0F ; P6DIR	|= 0x0F

#define RotDirFront			0x01
#define RotDirBack			0x02
#define RotDirStop			0x03
#define RotCtrLeft			0x00
#define RotCtrRight			0x03

#define CMD_ADD_RATE		0x01
#define CMD_RDU_RATE		0x02
#define CMD_DIR_FRONT		0x03
#define CMD_DIR_BACK		0x04
#define CMD_DIR_LEFT		0x05
#define CMD_DIR_RIGHT		0x06
#define CMD_STOP			0x07

/* KeyStatus Flag */
#define KEY_BRK				(0x01<<0)
#define KEY_SPD				(0x01<<1)
#define KEY_LEFT			(0x01<<2)
#define KEY_UP				(0x01<<3)
#define KEY_DOWN			(0x01<<4)
#define KEY_Fn				(0x01<<5)
#define KEY_SEL				(0x01<<6)
#define KEY_RIGHT			(0x01<<7)

#define KEY_SPD_ADD			(KEY_Fn|KEY_UP)
#define KEY_SPD_RDC			(KEY_Fn|KEY_DOWN)


void Init_PWM(void);
void ChangeRate(uint8_t rate);
void ControlProcess(uint8_t cmd);

#endif
