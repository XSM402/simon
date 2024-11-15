/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN Private defines */
#define CAN_RxExtId 0x1800D8D0
#define CAN_TxExtId 0x1800D0D8

typedef struct
{
	uint32_t DesDEVICE_ID   	    :8;             //源设备ID
	uint32_t Property		        :8;		        //指令属性值定义
	uint32_t SrcDEVICE_ID   	    :8;             //目标设备ID	
	uint32_t Priority				:4;			    //优先级
	uint32_t Permit                 :1;
}EXT_ID_Typedef;								    //定义29位扩展ID

typedef union
{
	int8_t			chars[8];			            //8个char
	int16_t			shorts[4];			            //4个short
	int32_t			ints[2];			            //2个int
	int64_t			longs[1];			            //1个Long
	uint8_t			uchars[8];			            //8个无符号char
	uint16_t		ushorts[4];			            //4个无符号short
	uint32_t		uints[2];			            //2个无符号int
	uint64_t		ulongs[1];			            //1个无符号Long
	float           floats[2];
}CAN_Data;	

typedef union
{
	uint32_t	all;
	uint32_t	StdID		:11;		            //标准ID
	EXT_ID_Typedef	  ExtID;		                //扩展ID
}ID;	

typedef struct
{
	ID 			id;						            //ID
	char		isRemote;			                //是否是远程帧
	char 		length;				                //数据长度
	CAN_Data	data;				                //数据
}Frame;

typedef struct
{
	uint16_t	prop;				                //属性名称
	void(*fun)(Frame* frm);					        //此属性对应的处理函数
}FunDict;

/* USER CODE END Private defines */

void MX_CAN1_Init(void);

/* USER CODE BEGIN Prototypes */
void CAN_Filter_Init(void);   //过滤器配置函数 
extern CAN_TxHeaderTypeDef	TxHeader;      //发送
extern CAN_RxHeaderTypeDef	RxHeader;      //接收
extern float SpeedMeasure;
extern float PositionMeasure;

extern uint8_t	RxData[8];   //数据接收数组
uint8_t CAN_Send_Msg(uint32_t id,uint8_t* msg,uint8_t len); //数据发送函数
void comm_can_set_rpm(uint8_t controller_id, float rpm) ;
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

