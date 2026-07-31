/******************************************************************************
* Copyright (c) 2026 lin
* SPDX-License-Identifier: MIT
******************************************************************************/

#include "SoftwareUart.h"

/*************************************************
变量定义
*************************************************/
u8 g_u8SoftUartSend[def_SoftUartSendNum] = {0};
u8 g_u8SoftUartRec[def_SoftUartRecNum] = {0};

SOFTWAREUARTFLAG_TypeDef g_sSoftwareUartFlag = {0};

u16 g_u16SendToSendTime = 0;
u8 g_u8SoftUartSendCnt = 0;
u8 g_u8SoftUartRecCnt = 0;
		
/******************************************************************
函数名称：						F_SoftwareUartInit
功能描述：						软件串口初始化
调用本函数的时钟周期：           无
输入参数：						无
输出参数：						无
其他说明：						无
*******************************************************************/
void F_SoftwareUartInit(void)
{
	//GPIO初始化
}

/******************************************************************
函数名称：						F_SoftwareUartSend
功能描述：						软件串口发送
调用本函数的时钟周期：           125us	
输入参数：						无
输出参数：						无
其他说明：						无
*******************************************************************/
void F_SoftwareUartSend(void)
{
	SOFTWAREUART_TypeDef l_sUartBuf = {0};
	static u8 s_u8Cnt = 0;
	u16 l_u16BitShift = 0x0001;
	static u8 s_u8BaudrateShift = 0;
	static u8 s_u8ParityCnt = 0;
	
	if(1 == g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartSend){
		s_u8BaudrateShift >>= 1;
		if(0 == s_u8BaudrateShift){
			s_u8BaudrateShift = def_Baudrate;
			
			l_u16BitShift <<= s_u8Cnt;
			
			l_sUartBuf.BIT.g_bStart = 0;
			l_sUartBuf.BIT.g_u8Data = g_u8SoftUartSend[s_u8SoftUartSendCnt];
		#if SoftUartParity
			#if (SoftUartParity == SoftUartOdd)
				l_sUartBuf.BIT.g_bParity = s_u8ParityCnt>0?0:1;
			#else 
				l_sUartBuf.BIT.g_bParity = s_u8ParityCnt>0?1:0;
			#endif							
		#endif 	
			l_sUartBuf.BIT.g_bStop = 1;
		#if SoftUartDoubleStop	
			l_sUartBuf.BIT.g_bDoubleStop = 1;
		#endif 	  
			if(l_sUartBuf.g_u16DataFrame&l_u16BitShift){
				def_SetUartSendPin(1);
				#if SoftUartParity 
					if((s_u8Cnt >= 1)&&(s_u8Cnt <= def_SoftUartData)){
						s_u8ParityCnt ^= 0x01;							
					}
				#endif 					
			}else{
				def_SetUartSendPin(0);
			}
			if(++s_u8Cnt >= def_SoftUartDataFrame){
				s_u8Cnt = 0;
				s_u8ParityCnt = 0;
				if(++g_u8SoftUartSendCnt >= def_SoftUartSendNum){
					g_u8SoftUartSendCnt = 0;
					g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartSend = 0;
					g_u16SendToSendTime = def_SendToSendTime;
				}
			}
		}
	}else{
		if(g_u16SendToSendTime > 0){
			g_u16SendToSendTime--;
		}
	}
}

/******************************************************************
函数名称：						F_SoftwareUartRec
功能描述：						软件串口接收
调用本函数的时钟周期：           125us		
输入参数：						无
输出参数：						无
其他说明：						无
*******************************************************************/
void F_SoftwareUartRec(void)
{
	static SOFTWAREUART_TypeDef s_sUartBuf = {0};
	static u8 s_u8Cnt = 0;
	static u16 s_u16BitShift = 0x0001;
	static u8 s_u8BaudrateShift = def_Baudrate;
	static u8 s_u8HighCnt = 0;
	static u16 s_u16RecErrTime = 0;
	static u8 s_u8ParityCnt = 0;
	
	if(s_u16RecErrTime > 0){
		s_u16RecErrTime--;
		if(0 == s_u16RecErrTime){
			g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec = 1;
		}
	}
	
	if(0 == g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec){
		
		if(0 == g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRecStart){
			if(0 == def_GetUartRecPin){
				g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRecStart = 0;
				if(0 == def_GetUartRecPin){
					g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRecStart = 1;	
				}
			}
		}
		
		if(1 == g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRecStart){
			if(s_u8BaudrateShift&def_RecSample){
				if(def_GetUartRecPin){
					s_u8HighCnt++;
				}
			}
			s_u8BaudrateShift >>= 1;
			if(0 == s_u8BaudrateShift){
				s_u8BaudrateShift = def_Baudrate;
			
				if(s_u8HighCnt >= def_RecSampleNum){
					s_sUartBuf.g_u16DataFrame |= s_u16BitShift;
				#if SoftUartParity
					if((s_u8Cnt >= 1)&&(s_u8Cnt <= def_SoftUartData)){
						s_u8ParityCnt ^= 0x01;							
					}
				#endif 		
				}
				s_u16BitShift <<= 1;
				s_u8HighCnt = 0;
				if(++s_u8Cnt >= def_SoftUartDataFrame){
					s_u8Cnt = 0;
					g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRecStart = 0;
					s_u16RecErrTime = def_RecErrTime;
					s_u16BitShift = 0x0001;					
					if(1 == s_sUartBuf.BIT.g_bStart){
						g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec = 1;
				#if SoftUartParity
				
				#if (SoftUartParity == SoftUartOdd)
					}else if(0 == (s_sUartBuf.BIT.g_bParity+s_u8ParityCnt)){
						g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec = 1;
				#else
					}else if(1 == (s_sUartBuf.BIT.g_bParity+s_u8ParityCnt)){
						g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec = 1;
				#endif
				
				#endif 						
					}else if(0 == s_sUartBuf.BIT.g_bStop){
						g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec = 1;
				#if SoftUartDoubleStop
					}else if(0 == s_sUartBuf.BIT.g_bDoubleStop){
						g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec = 1;
				#endif		
					}else{
						g_u8SoftUartRec[g_u8SoftUartRecCnt] = s_sUartBuf.BIT.g_u8Data;
						g_u8SoftUartRecCnt++;
						if(g_u8SoftUartRecCnt > 2){
							if((g_u8SoftUartRecCnt >= g_u8SoftUartRec[2])||(g_u8SoftUartRecCnt >= def_SoftUartRecNum)){
								g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec = 1;
							}
						}
					}
				s_sUartBuf.g_u16DataFrame = 0;	
				#if SoftUartParity
					s_u8ParityCnt = 0;
				#endif 						
				}
			}
		}
	}else{

	}
}

/******************************************************************
函数名称：						F_SoftwareUartRecOpt
功能描述：						软件串口接收处理
调用本函数的时钟周期：           无	
输入参数：						无
输出参数：						无
其他说明：						无
*******************************************************************/
void F_SoftwareUartSendOpt(void)
{
	u16 l_u16Buf = 0;
	
	if(g_u16SendToSendTime > 0){

	}else if(0 == g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartSend){
		
		g_u8SoftUartSend[0] = def_SoftUartHead;//数据头
		g_u8SoftUartSend[1] = def_SoftUartAdress;//设备地址
		
		g_u8SoftUartSend[2] = def_SoftUartSendLength0;//数据长度
		g_u8SoftUartSend[3] = def_SoftUartSendCmd0;//命令字
		
		g_u8SoftUartSend[4] = 0x00;//地址高位
		g_u8SoftUartSend[5] = 0x00;//地址低位
		
		g_u8SoftUartSend[6] = 0x00;//数据高位
		g_u8SoftUartSend[7] = 0x00;//数据低位

		l_u16Buf = F_CrcCal(g_u8SoftUartSend,8);
		
		g_u8SoftUartSend[8] = l_u16Buf>>8;
		g_u8SoftUartSend[9] = l_u16Buf&0xff;		
		
		g_u8SoftUartSendCnt = 0;
		g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartSend = 1;
	}
}
/******************************************************************
函数名称：						F_SoftwareUartRecOpt
功能描述：						软件串口接收处理
调用本函数的时钟周期：           无		
输入参数：						无
输出参数：						无
其他说明：						无
*******************************************************************/
void F_SoftwareUartRecOpt(void)
{
	u16 l_u16Buf = 0;
	
	if(1 == g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec){		
		if(g_u8SoftUartRecCnt > 5){
			l_u16Buf = g_u8SoftUartRec[g_u8SoftUartRecCnt-2];
			l_u16Buf <<= 8;
			l_u16Buf |= g_u8SoftUartRec[g_u8SoftUartRecCnt-1];
			if(l_u16Buf == F_CrcCal(g_u8SoftUartRec,g_u8SoftUartRecCnt-2)){
				g_u8SoftUartRec[0] = 0;
				if(def_SoftUartHead == g_u8SoftUartRec[0]){
					if(def_SoftUartAdress == g_u8SoftUartRec[1]){
						if(g_u8SoftUartRecCnt == g_u8SoftUartRec[2]){
							switch(g_u8SoftUartRec[3]){
								case 0x00:
								
								break;
								default:
								
								break;					
							}
						}
					}
				}
			}
		}

		memset(&g_u8SoftUartRec[0], 0, def_SoftUartRecNum);
		g_u8SoftUartRecCnt = 0;
		g_sSoftwareUartFlag.s_u8bit.g_bSoftwareUartRec = 0;
	}
}

/******************************************************************
函数名称：						F_CrcCal
功能描述：						CRC16校验
调用本函数的时钟周期：           无		
输入参数：						参数1：数据，参数2：数据长度
输出参数：						返回值：CRC16校验值
其他说明：						无
*******************************************************************/
u16 F_CrcCal(u8 *l_pu8Data, u8 l_u8Length)
{
	u8 l_u8Cnt = 0;
	u16 g_u16CrcValue=0xffff;

	while(l_u8Length--) {
		g_u16CrcValue^=*l_pu8Data++;
		for(l_u8Cnt=0;l_u8Cnt<8;l_u8Cnt++) {
			if(g_u16CrcValue&0x0001)
			g_u16CrcValue=(g_u16CrcValue>>1)^0xa001;
			else
			g_u16CrcValue=g_u16CrcValue>>1;
		}
	}

	return g_u16CrcValue;
}

