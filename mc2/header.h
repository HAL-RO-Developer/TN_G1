#ifndef __HEADER_H__
#define __HEADER_H__

typedef struct{
	T_MSG	pk_msg;
	unsigned char 	fromId;
	unsigned char	toId;
	char			message[50];
}MBX_PKT;

#endif /* __HEADER_H__ */
