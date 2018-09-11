//**********************************************************************
//  STM32_Toppers: HAL_Extention_Header (Rev.03)
//																  Sumida
//**********************************************************************
//**************************************
// General #include
//**************************************
#include "stm32f4xx_hal.h"
#include "CMSIS\Device\ST\STM32F4xx\Include\stm32f4xx.h"
#include "sil.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define non_terminal_sio		// 通信相手がTerminalでなくProgramの場合Commentを外す事

//**************************************
// SIO_Function(UART2) extern
//**************************************
extern void sio_open(ID);
extern void sio_write_string(ID,char*);
extern void sio_write_char(ID,char*);
extern void sio_read_string(ID,char*);
extern void sio_read_char(ID,char*);

//***************************************
// _sbrk Function extern
//***************************************
extern caddr_t _sbrk_r (struct _reent *r, int incr);

//***************************************
// TIM2_Timer Attribute definition
//***************************************
#define INTNUM_TIM2	(16+28) // for TIM2 (Base16 + TIM2_Int_Code 28)
#define INTPRI_TIM2	-3		// Int Priority
#define INTATR_TIM2	0		// Attributes

//***************************************
// TIM3_Timer Attribute definition
//***************************************
#define INTNUM_TIM3	(16+29) // for TIM3 (Base16 + TIM2_Int_Code 29)
#define INTPRI_TIM3	-3		// Int Priority
#define INTATR_TIM3	0		// Attributes

