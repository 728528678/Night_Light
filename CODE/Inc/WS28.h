/*
WS28 - Library for the WS281X series of RGB LED device IC.

Copyright 2018 Waiman

Supported devices:
WS2811 WS2812
*/

#ifndef __WS28_H__
#define __WS28_H__

#include "main.h"
#include <stdio.h>
#include <string.h>

#include "dma.h"
#include "spi.h"

#define PIXELS_LEN		(8)		//
#define BUFF_LEN		(PIXELS_LEN*12)
#define LED_ARR			(BUFF_LEN+1)

#define BIT00			(0x88)
#define BIT01			(0x8E)
#define BIT10			(0xE8)
#define BIT11			(0xEE)

//														0x00 B R G
#define BLACK			((uint32_t)(0x00000000U))
#define RED				((uint32_t)(0x0000FF00U))
#define GREEN			((uint32_t)(0x00FF0000U))
#define BLUE			((uint32_t)(0x000000FFU))
#define YELLOW			((uint32_t)(0x00FFFF00U))
#define TURQUOISE		((uint32_t)(0x00FF00FFU))
#define PURPLE			((uint32_t)(0x0000FFffU))
#define WHITE			((uint32_t)(0x00FFFFFFU))

typedef enum
{
    BUSY = 0x00U,
    READY= 0x01U
} BSP_UpdateEnumDef;


typedef enum  
{
	CW,
	CCW
}DIRECTION_HandleTypeDef;



typedef struct
{
    uint8_t *   		BuffPoint;			/*!< 缓存指针        */
    uint8_t *   		SendPoint;			/*!< 发送指针        */
    uint8_t 				Brightness;			/*!< 亮度参数        */
    uint16_t        PixelLen;			  /*!< 像素数量        */
    uint16_t        BuffLen;			  /*!< 缓存长度        */
    __IO BSP_UpdateEnumDef    Status;		/*!< 状态            */
} SW28_HandleTypeDef;

typedef enum
{
    UP = 0x00,
    DOWN= 0x01,
		WAIT = 0x02
} UpOrDwon;

typedef struct
{
    uint8_t    		Red;		
    uint8_t    		Green;	
    uint8_t 			Blue;		
    float      		RedStep;		
		float      		GreenStep;
		float       	BlueStep;
    uint32_t      Color;
		uint8_t		    Cnt;	
		uint8_t				WaitCnt;
		UpOrDwon  		Status;
		
} Breathe_HandleTypeDef;

extern SW28_HandleTypeDef	hsw28;
extern Breathe_HandleTypeDef bre28;
//**************************************************************************************
extern  void Beathe_Init(Breathe_HandleTypeDef *breObj, uint32_t color);
extern  void Breathe_Show(void);
extern 	void Sw28_Init(SW28_HandleTypeDef *swObj,uint8_t Len);
extern 	void Sw28_SetPixelsColor(SW28_HandleTypeDef *swObj,uint8_t pix,uint32_t color);
extern 	void Sw28_AllBlock(SW28_HandleTypeDef *swObj);

extern 	void Sw28_SetAllColor(SW28_HandleTypeDef *swObj,uint32_t color);
extern 	void Sw28_Show(SW28_HandleTypeDef *swObj);
extern 	uint32_t Sw28_Color(uint8_t r, uint8_t g, uint8_t b);
extern 	uint32_t Sw28_GetPixelColor(SW28_HandleTypeDef *swObj, uint16_t pix);
extern 	uint32_t Sw28_ScaleColor(uint8_t scale, uint32_t color);
extern 	void Sw28_SetBrightness(SW28_HandleTypeDef *swObj, uint8_t b);
extern 	void Sw28_Color2RGB(uint32_t color, uint8_t* r, uint8_t* g, uint8_t* b);

extern 	uint32_t Wheel(uint8_t WheelPos);

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//					效果函数
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

extern 	void rainbow(SW28_HandleTypeDef *swObj, uint16_t wait);
extern 	void rainbowCycle(SW28_HandleTypeDef *swObj, uint16_t wait);
extern 	void theaterChase(SW28_HandleTypeDef *swObj, uint32_t c, uint16_t wait);
extern 	void theaterChaseRainbow(SW28_HandleTypeDef *swObj, uint16_t wait);
extern 	void colorWipe(SW28_HandleTypeDef *swObj, uint32_t c, uint16_t wait);

extern 	void blink_all(SW28_HandleTypeDef *swObj, uint32_t color, uint8_t times, uint16_t delay_time);
#endif
