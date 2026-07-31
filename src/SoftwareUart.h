/******************************************************************************
* Copyright (c) 2026 lin
* SPDX-License-Identifier: MIT
******************************************************************************/
 
#ifndef _SOFTWAREUART_H_
#define _SOFTWAREUART_H_
/*************************************************
宏定义
*************************************************/
#define def_SoftUartStart			    1//起始位

#define def_SoftUartData				8//数据位

#define	SoftUartNoneParity				0
#define	SoftUartOdd						1
#define	SoftUartEven					2

#define SoftUartParity					SoftUartNoneParity//奇偶校验位

#define def_SoftUartStop				1//停止位

#define SoftUartDoubleStop				0//2个停止位

#define def_SoftUartDataFrame			(def_SoftUartStart+def_SoftUartData+def_SoftUartParity+def_SoftUartStop+def_SoftUartDoubleStop)

typedef union{
     u16 g_u16DataFrame;
	 struct{
		  u16 g_bStart : def_SoftUartStart;
		  u16 g_u8Data : def_SoftUartData;
		#if SoftUartParity
			#define def_SoftUartParity				1
			u16 g_bParity : def_SoftUartParity;
		#else
			#define def_SoftUartParity				0
		#endif 			 
		  u16 g_bStop : def_SoftUartStop;
		#if SoftUartDoubleStop
			#define def_SoftUartDoubleStop			1
			u16 g_bDoubleStop : def_SoftUartDoubleStop;
		#else 
			#define def_SoftUartDoubleStop			0
		#endif 			 
		}BIT;
}SOFTWAREUART_TypeDef;

#define def_Baudrate					(1<<7)//波特率1000000/125/8bps

#define	def_RecSample					0x3c//接收采样电平区间
#define	def_RecSampleNum				3//接收采样电平个数

#define def_SendToSendTime        		300//发送帧最小间隔时间

#define	def_RecErrTime					300//接收字节超时时间

#define def_SoftUartSendNum				10//最大发送缓存									
#define def_SoftUartRecNum				10//最大接收缓存

#define def_SetUartSendPin(x)			P00 = x//发送脚
#define def_GetUartRecPin				P01//接收脚

typedef union{
     unsigned char g_u8Data;
	 struct{
		  unsigned char g_bSoftwareUartSend : 1;
		  unsigned char g_bSoftwareUartRec : 1;
		  unsigned char g_bSoftwareUartRecStart : 1;
		}s_u8bit;
}SOFTWAREUARTFLAG_TypeDef;

/*************************************************
外部声明
*************************************************/
extern void F_SoftwareUartSend(void);
extern void F_SoftwareUartRec(void);
extern void F_SoftwareUartSendOpt(void);
extern void F_SoftwareUartSendOpt(void);
extern u16 F_CrcCal(u8 *l_pu8Data, u8 l_u8Length);

#endif