//********************************************************************
//  STM32_Toppers: HAL_Extention_Functions  (Rev.03)
//																Sumida
//********************************************************************
#include "kernel_cfg.h"             // Toppers Kernel Config Header
#include "hal.h"					// Toppers Header
#include "hal_extention.h"			// Toppers HAL_Extention Header

#ifndef non_terminal_sio
	#define  string_end '\r'
#else
	#define  string_end '\0'
#endif

//**************************************
//  SIO_Function (UART2)
//**************************************
// **** Open SIO Port ****
void sio_open(ID portid) {
   ER ercd;
   ercd = serial_opn_por(portid);
   if (ercd < 0 && MERCD(ercd) != E_OBJ) {        // Port Shareing with Syslog is not Error
       syslog_1(LOG_NOTICE,"SIO Open Err (ercd=%d); Prog Terminate",ercd);
       ext_tsk();
   }else {
       serial_ctl_por(portid,(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)); // Set Controls
   }
}
// **** Write String ****
void sio_write_string(ID portid, char* w_pnt) {
   for (; *w_pnt!='\0'; w_pnt++) {
      serial_wri_dat(portid, w_pnt,1);
   }
}
// **** Write Character ****
void sio_write_char(ID portid, char* w_pnt) {
   serial_wri_dat(portid, w_pnt,1);
}
// **** Read String ****
void sio_read_string(ID portid, char* r_pnt) {
   for (;; r_pnt++) {
      serial_rea_dat(portid, r_pnt, 1);
	  
      if (*r_pnt == string_end){                  // If Enter_Key, Put \0 to end of String
          *r_pnt = '\0';
          break;
	  }
	  #ifndef non_terminal_sio
      if (*r_pnt == '\b'){                        // If BackSpace, Rewind the Pointer
          r_pnt=r_pnt -2;
      }else {                                     // EchoBack Input Char to Terminal
          serial_wri_dat(portid, r_pnt, 1);
      }
	  #endif
   }
   #ifndef non_terminal_sio
   sio_write_string(TASK_PORTID,"\n\r");          // New Line
   #endif
}
// **** Read Character ****
void sio_read_char(ID portid, char* r_char){
   serial_rea_dat(portid,r_char,1);
   serial_wri_dat(portid,r_char,1);               // EchoBack Input Char to Terminal
}

//**************************************
//  _sbrk Function ..for 問題回避 (角田)
//**************************************
char * stack_ptr asm("sp");
caddr_t _sbrk_r (struct _reent *r, int incr){
  extern char   end asm ("end");
  static char * heap_end;
  char * prev_heap_end;
  if (heap_end == NULL){
     heap_end = & end;
  }
  prev_heap_end = heap_end;
  if (heap_end + incr > stack_ptr){
     errno = ENOMEM;
     return (caddr_t) -1;
  }
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

