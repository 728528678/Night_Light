#ifndef __USERCMD_H__
#define __USERCMD_H__

#include "main.h"
#include "usart.h"
#include "WS28.h"



typedef enum
{
	mode	= 0x00U,
	color	= 0x01U,
	num		= 0x02U
}USER_CMD;


uint32_t BRG_to_RGB(uint32_t BRG);
void Show_Firstdown_inf(void);
extern void User_CMD_Init(void);
uint8_t CMD_String_deal(uint8_t* str);
void Show_error_data(void);
extern void Sw_Light_Color(uint32_t u_color);
extern uint32_t HexStr_to_u32(uint8_t* str);
#endif /* __USERCMD_H__ */
