//******************************************************************************
//    ToppersASP on STM32F401_Nucleo-64  IO_Sample_Program          : by  Sumida
//
//        IO Sample Program (SYSLOG, SIO, LED, SWT, and Servo_PWM)
//                  Use Default USB Port for SYSLOG,& SIO
//                  Use LED PA5(On_Board) & PB1(Wired)
//******************************************************************************

//***** Include Header Files ***************************************************
#include "kernel_cfg.h"         // Toppers Kernel Config Header
#include "hal.h"				// Toppers Header
#include "hal_extention.h"		// Toppers HAL_Extention Header
#include "header.h"

#define	GET	( 1 )
#define	RLS	( 2 )
#define CLN	( 3 )

#define SRVO_MIN	(  650 )
#define SRVO_MAX	( 2350 )
#define SRVO_RANGE	( SRVO_MAX - SRVO_MIN )
#define SRVO_RATIO	( SRVO_RANGE / 180 )

void init();
void analysis(char* msg);
void led(char no, int interval, int time);
void servo(long deg);
void mpf_manage( int ope, MBX_PKT* packet );

MBX_PKT* 	packet;

void rcv_task(intptr_t exinf)
{
	char		buffer_in[50];
	char 		message[50];
	
	init();
	
	while(1){
		/* --- データ受信 --- */
		sio_read_string(TASK_PORTID, buffer_in);
		strncpy(message, buffer_in, 50);
		
		get_mpf(MPF, (void**)&packet);
		mpf_manage( GET, packet );
		
		packet->fromId = RCV;
		packet->toId = OPE;
		strcpy(packet->message, message);
		snd_mbx(MBX, (T_MSG*)packet);

		dly_tsk(10);
	}
}

void ope_task(intptr_t exinf)
{
	T_RMBX mbx_status;
	
	while(1){
		/* --- MailBox取り出し --- */
		while(1){
			ref_mbx(MBX, &mbx_status);
			if(mbx_status.pk_msg != NULL){
				packet = (MBX_PKT*)mbx_status.pk_msg;
				if(packet->toId == OPE ){
					rcv_mbx(MBX, (T_MSG**)&packet);
					break;
				}
			}
			dly_tsk(3);
		}
		
		analysis(packet->message);

		rel_mpf(MPF, packet);
		mpf_manage( RLS, packet );
	}
}

void analysis(char* msg)
{
	if( strcmp(msg, "L110") == 0 ){
		// LED1点滅　10ms
		led(1, 10, 10);
	}else if( strcmp(msg, "L1500") == 0 ){
		// LED1点滅　500ms
		led(1, 500, 10);
	}else if( strcmp(msg, "L210") == 0 ){
		// LED2点滅　10ms
		led(2, 10, 10);
	}else if( strcmp(msg, "L2500") == 0 ){
		// LED2点滅　500ms
		led(2, 500, 10);
	}else if( strcmp(msg, "S90") == 0 ){
		// Servo 90度回転
		servo(90);
	}else if( strcmp(msg, "S180") == 0 ){
		// Servo 180度回転
		servo(180);
	}
}

void init()
{
	// SIO Init
	sio_open(TASK_PORTID);

	// LED Init
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOA->MODER |= GPIO_MODER_MODER5_0;                     			// IO_Mode=PA5, LED(onBoard_LD2) OUT
	GPIOB->MODER |= GPIO_MODER_MODER1_0;                     			// IO_Mode=PB1, LED(wired) OUT
	GPIOA->ODR   &= ~GPIO_ODR_ODR_5;                	       			// OutData=PA5,LED(onBoard_LD2) OFF
	GPIOB->ODR   &= ~GPIO_ODR_ODR_1;   				           			// OutData=PB1,LED(wired) OFF

	// SERVO Init (FOOT)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER0_0;
}

void led(char no, int interval, int time)
{
	int cnt;
	switch(no){
		case 1:
			for( cnt = 0; cnt < time * 1000; cnt+=interval){
				GPIOA->ODR   |= GPIO_ODR_ODR_5;
				dly_tsk(interval);
				GPIOA->ODR   &= ~GPIO_ODR_ODR_5;
				dly_tsk(interval);
			}
			break;
		case 2:
			for( cnt = 0; cnt < time * 1000; cnt+=interval){
				GPIOB->ODR   |= GPIO_ODR_ODR_1;
				dly_tsk(interval);
				GPIOB->ODR   &= ~GPIO_ODR_ODR_1;
				dly_tsk(interval);
			}
			break;
		default:
			break;
	}

}

void servo(long deg)
{
	const long pulse	= 20 * 1000;
	const long duty		= SRVO_RATIO * deg + SRVO_MIN;

	int i;

	for( i = 0; i < 30; i++ ){
		GPIOA->ODR |= GPIO_ODR_ODR_0;
		sil_dly_nse( duty * 1000 * 2) throw();
		GPIOA->ODR &= ~GPIO_ODR_ODR_0;
		sil_dly_nse( (pulse-duty) * 1000 * 2) throw();
		dly_tsk(1);
	}

}

void mpf_manage( int ope, MBX_PKT* packet )
{
	int i;
	static MBX_PKT* addr[10] = { 0,0,0,0,0,0,0,0,0,0 };
	
	switch( ope ){
		case	GET:
			for( i = 0; addr[i] != 0; i++ );
			addr[i] = packet;
			break;
		case	RLS:
			for( i = 0; addr[i] == packet; i++ );
			addr[i] = 0;
			break;
		case	CLN:
			for( i = 0; i < 10; i++ ){
				if( addr[i]!=NULL){
					rel_mpf( MPF,addr[i] );
					sio_write_string(TASK_PORTID, "Release MPF\n\r");
				}
			}
			break;
		default:
			break;
	}	
	
}

