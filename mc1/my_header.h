//**********************************************************************
//  MY_HEADER for MailBox
//																  Sumida
//**********************************************************************

typedef struct {							// MailBox Packet
	T_MSG	pk_msg;
	unsigned int  fromID;
	unsigned int  toID;
	char 	message[50];
}MBX_PKT;


