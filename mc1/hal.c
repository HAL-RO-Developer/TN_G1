//******************************************************************************
// 課題04_MailBox  Sample
//                                            		                      Sumida
//******************************************************************************
//***** Include Header Files ***************************************************
#include "kernel_cfg.h"	// Toppers Kernel Config Header
#include "hal.h"		   // Toppers Header
#include "hal_extention.h" // Toppers HAL_Extention Header
#include "my_header.h"	 // My Header

//#define relmpf_debug			// if on, never issue rel_mpf

#define GET 1
#define RLS 2
#define CLN 3
void mpfmanage(int, MBX_PKT *); // mpf_manager

//***** TSK1 ****************************************************************
void task1(intptr_t exinf)
{
	MBX_PKT *packet; // MBX Packet Address
	//char to_whom[10];

	while (1)
	{

		// コンソールからの送信メッセージ
		get_mpf(MPF, (void **)&packet); // MBX Packet獲得
		mpfmanage(GET, packet);
		packet->fromID = TSK1;

		sio_read_string(TASK_PORTID, packet->message);

		// TSK2へ
		packet->toID = TSK2;
		snd_mbx(MBX, (T_MSG *)packet); // MBX送信
	}
}

//***** TSK2 ****************************************************************
void task2(intptr_t exinf)
{
	MBX_PKT *packet; // MBX Packet Address

	while (1)
	{
		rcv_mbx(MBX, (T_MSG **)&packet); // MBX受信

		sio_write_string(TASK_PORTID, packet->message);

		rel_mpf(MPF, packet); // MBX Packet解放
		mpfmanage(RLS, packet);
	}
}

//***** MPF管理 (共通関数) ****************************************************
void mpfmanage(int ope, MBX_PKT *packet)
{
	static MBX_PKT *addr[10] = {
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	}; // Packet Manage Table
	int i;
	char adr[10];
	if (ope == GET)
	{
		itoa((int)packet, adr, 16);
		addr[i] = packet;
	}
	else if (ope == RLS)
	{
		for (i = 0; addr[i] != packet; i++)
		{
		}
		itoa((int)packet, adr, 16);

		addr[i] = 0;
	}
	else if (ope == CLN)
	{
		for (i = 0; i < 10; i++)
		{
			if (addr[i] != 0)
			{
				itoa((int)addr[i], adr, 16);
				rel_mpf(MPF, addr[i]);
			}
		}
	}
}
