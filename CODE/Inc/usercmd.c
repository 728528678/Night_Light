/*----------------------------------------------------------------------------/
/  Init  -  usercmd 						                 (C)Lin, 2018
/-----------------------------------------------------------------------------/
/ 本程序只供学习使用，未经作者许可，不得用于其它任何用途
/ 测试验证环境Init Night Light V2.0 开发板
/ user cmd
/ Copyright (C) 2019, Lin, all right reserved.
/
/ * 修改日期：2019-01-24
/ * 版本：V1.0
/ * Copyright(C) InitDev 2019-2029
/
/-----------------------------------------------------------------------------/
修改记录
/
/---------------------------------------------------------------------------*/

#include "usercmd.h"
#include <string.h>
#include "main.h"
																		 /* ------------------------------ */
const char user_init_txt1[] = "/* Init Electronic Design Studio  */  \r\n/* Night Light V2.0               */  \r\n";
const char user_init_txt2[] = "/* Copyright(C) InitDev 2019-2029 */  \r\n/* Input /help get all user cmd   */  \r\n";
const char user_help_txt1[] = " /help  命令显示帮助信息\r\n /color 命令查看或改变三次触摸的颜色\r\n /save  命令保存操作\r\n";

extern uint32_t WriteFlashData;
extern uint32_t ReadFlashData ;
extern uint32_t FlashAddr[3];
extern uint32_t sys_color[3];                             
extern uint8_t ReceiveBuff[BUFFERSIZE];
extern uint8_t Work_Mode;
extern uint32_t color_temp;
extern uint8_t Rx_len;
extern void WriteFlash(uint8_t addrnum,uint32_t data);
extern uint32_t ReadFlash(uint8_t addrnum);

uint8_t USER_CMD_FLAG = 0;
uint8_t Chose_Color	= 0;
uint8_t Save_Flag=0;
uint32_t user_color_temp=0;
/* USER CMD Init */
void User_CMD_Init(void)
{
	/* show information about user cmd */
	printf(user_init_txt1);
	printf(user_init_txt2);
	
}
/******* CMD TABLE *******/
/*	/help	show help information
	/color	get 3 touch level color,and you can exchange the color
	/save	save the operation
*/
/* USER CMD string deal  */
uint8_t CMD_String_deal(uint8_t* str)
{
	if(strcasecmp((const char*)str,"/help")==0)
	{
		printf(user_help_txt1);
	}
	else if(strcasecmp((const char*)str,"/color")==0)
	{
		printf("\r\n /color \r\n");
		printf(" color1: 0x%06X\r\n",BRG_to_RGB(sys_color[0]));//打印当前颜色
		printf(" color2: 0x%06X\r\n",BRG_to_RGB(sys_color[1]));
		printf(" color3: 0x%06X\r\n",BRG_to_RGB(sys_color[2]));
		printf(" 输入1,2,3改变对应颜色\r\n");
		USER_CMD_FLAG = 1;
		return 1;
	}
	else if(strcasecmp((const char*)str,"/save")==0)
	{
		if(Save_Flag == 1)
		{
			Save_Flag = 0;
			printf("\r\n /save \r\n");
			WriteFlash(0,sys_color[0]);
			WriteFlash(1,sys_color[1]);
			WriteFlash(2,sys_color[2]);
			printf(" 数据保存成功\r\n");
			printf(" color1: 0x%06X\r\n",BRG_to_RGB(ReadFlash(0)));//打印当前颜色
			printf(" color2: 0x%06X\r\n",BRG_to_RGB(ReadFlash(1)));
			printf(" color3: 0x%06X\r\n",BRG_to_RGB(ReadFlash(2)));
			USER_CMD_FLAG = 0;
			return 1;
		}
		else
		{
			printf("\r\n 当前无数据修改 \r\n");
		}
	}
	else if((USER_CMD_FLAG == 1)&&((*str > '0')&&(*str < '4')))//修改颜色
	{
		printf("\r\n 您要改变的是 color%x\r\n",*str-0x30);
		printf(" 请输入颜色RGB值,以0x开头.例如0x303030\r\n\r\n");
		printf(" 调整好后输入 /save 命令来保存当前数据\r\n\r\n");
		Chose_Color = *str-0x30;
		USER_CMD_FLAG = 2;
		return 1;
	}
	else if((USER_CMD_FLAG != 0)&&(Chose_Color >= 1)&&(Chose_Color <= 3))//颜色切换模式color1.2.3
	{
		color_temp = HexStr_to_u32(str);
		if(color_temp ==0xff000000)
		{
			Show_error_data();
			return 0;
		}
		else
		{
			Save_Flag = 1;//需要保存
			Work_Mode = Mode_Debug;//亮灯
			sys_color[Chose_Color-1] = color_temp;
			printf(" 当前color%x:0x%06X,可继续输入RGB参数改变颜色\r\n\r\n",Chose_Color,user_color_temp);
			printf(" 或输入1,2,3改变对应color的颜色\r\n");
			USER_CMD_FLAG = 1;
			return 1;
		}
	}
	else
	{
		Show_error_data();
		return 0;
	}
	return 0;
}

/* 下载一次程序，之前保存的数据被覆盖，提醒用户 */
void Show_Firstdown_inf(void)
{
	printf("\r\n");
	printf(" __________________________________\r\n");
	printf("|                                  |\r\n");
	printf("|              WARNING             |\r\n");
	printf("|__________________________________|\r\n");
	printf("\r\n检测到未配置用户color或数据丢失，可能需要重新配置color\r\n");
}

void Show_error_data(void)
{
	printf("接收到数据[ ");
	for(int i=0;i<Rx_len;i++)
	{
	/*        打印接收到的数据 */
		printf("%c",ReceiveBuff[i]);
	}
	printf(" ]请检查数据是否正确\r\n");
}

/* Switch light color depand on user color cmd */
void Sw_Light_Color(uint32_t u_color)
{
	colorWipe(&hsw28,u_color,20);
}



/* 大写转换成小写 */
uint8_t CaptoLow(uint8_t cap)
{
	if((cap > 'A')&&(cap < 'Z'))
	{
		return cap+'a'-'A';
	}
	else
	{
		return cap;
	}
}

uint8_t ByteHex_to_u8(uint8_t hex)
{
	uint8_t temp = 0;
	temp = CaptoLow(hex);
	if((temp>='0')&&(temp<='9'))
	{
		return temp-'0';
	}
	else if((temp>='a')&&(temp<='f'))
	{
		return temp-'a'+10;
	}
	return 0xff;
}

//0x开头的十六进制方式字符串转数字，example: "0xf5f3f4"	->	0xf5f3f4=16118772
uint32_t HexStr_to_u32(uint8_t* str)
{
	uint8_t i=0;
	uint32_t num=0;
	/* inspect strings format */
	if(str[0] == '0')
	{
		if(CaptoLow(str[1]) == 'x')
		{
			for(i=2;((str[i]>='0')&&(str[i]<='9'))||((str[i]>='a')&&(str[i]<='f'))||((str[i]>='A')&&(str[i]<='F'));i++)
			{
				num = 16*num + ByteHex_to_u8(str[i]);
			}
			user_color_temp = num;
			num = ((user_color_temp&0xffff00)>>8)|((user_color_temp&0xff)<<16);
			return num;
		}
	}
	return 0xff000000;
}

uint32_t BRG_to_RGB(uint32_t BRG)
{
	uint32_t RGB=0;
	RGB = ((BRG&0x0000ffff)<<8)|((BRG & 0x00ff0000)>>16);
	return RGB;
}

