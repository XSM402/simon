/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
CAN_TxHeaderTypeDef	TxHeader;      //发送
CAN_RxHeaderTypeDef	RxHeader;      //接收
float SpeedMeasure;
float PositionMeasure;
uint8_t	RxData[8];  //数据接收数组，can的数据帧只有8帧

typedef enum {
	CAN_PACKET_SET_DUTY = 0,
	CAN_PACKET_SET_CURRENT,
	CAN_PACKET_SET_CURRENT_BRAKE,
	CAN_PACKET_SET_RPM,
	CAN_PACKET_SET_POS,
	CAN_PACKET_FILL_RX_BUFFER,
	CAN_PACKET_FILL_RX_BUFFER_LONG,
	CAN_PACKET_PROCESS_RX_BUFFER,
	CAN_PACKET_PROCESS_SHORT_BUFFER,
	CAN_PACKET_STATUS,
	CAN_PACKET_SET_CURRENT_REL,
	CAN_PACKET_SET_CURRENT_BRAKE_REL,
	CAN_PACKET_SET_CURRENT_HANDBRAKE,
	CAN_PACKET_SET_CURRENT_HANDBRAKE_REL,
	CAN_PACKET_GET_STATUS
} CAN_PACKET_ID;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 12;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/*CAN过滤器初始化*/
void CAN_Filter_Init(void)
{
	CAN_FilterTypeDef sFilterConfig;
	
  sFilterConfig.FilterBank = 0;                    /* 过滤器组0 */
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;  /* 屏蔽位模式 */
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; /* 32位。*/
  
  sFilterConfig.FilterIdHigh         = (((uint32_t)CAN_RxExtId<<3)&0xFFFF0000)>>16;				/* 要过滤的ID高位 */
  sFilterConfig.FilterIdLow          = (((uint32_t)CAN_RxExtId<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; /* 要过滤的ID低位 */
  sFilterConfig.FilterMaskIdHigh     = 0xFFFF;			/* 过滤器高16位每位必须匹配 */
  sFilterConfig.FilterMaskIdLow      = 0xFFFF;			/* 过滤器低16位每位必须匹配 */
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;           /* 过滤器被关联到FIFO 0 */
  sFilterConfig.FilterActivation = ENABLE;          /* 使能过滤器 */ 
  sFilterConfig.SlaveStartFilterBank = 14;
	
	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
		/* Filter configuration Error */
    Error_Handler();
  }
	
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }
	
	  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }
	
	TxHeader.ExtId=CAN_TxExtId;        //扩展标识符(29位)
	TxHeader.IDE=CAN_ID_EXT;    //使用标准帧
	TxHeader.RTR=CAN_RTR_DATA;  //数据帧
	TxHeader.DLC=8; 
	TxHeader.TransmitGlobalTime = DISABLE;
}

/*CAN发送数据，入口参数为要发送的数组指针，数据长度，返回0代表发送数据无异常，返回1代表传输异常*/
uint8_t CAN_Send_Msg(uint32_t id,uint8_t* msg,uint8_t len)
{	
  uint8_t i=0;
	uint32_t TxMailbox;
	uint8_t message[8];
 
	TxHeader.ExtId=id;        //扩展标识符(29位)
	TxHeader.IDE=CAN_ID_EXT;    //使用扩展帧
	TxHeader.RTR=CAN_RTR_DATA;  //数据帧
	if(len > 8)
	{
		len = 8;
	}
	TxHeader.DLC=len;    
	
  for(i=0;i<len;i++)
  {
		message[i]=msg[i];
	}
	
  if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, message, &TxMailbox) != HAL_OK)//发送
	{
		return 1;
	}
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3) {}
    return 0;
}

void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t *index) {
	buffer[(*index)++] = number >> 24;
	buffer[(*index)++] = number >> 16;
	buffer[(*index)++] = number >> 8;
	buffer[(*index)++] = number;
}

void comm_can_set_rpm(uint8_t controller_id, float rpm) {
	int32_t send_index = 0;
	uint8_t buffer[4];
	// 把rpm存在buffer里
	buffer_append_int32(buffer, (int32_t)rpm, &send_index);
	// 发送扩展帧数据，0-7位位目标设备id，8-15位为属性值，这里设置为CAN_PACKET_SET_RPM，后面均置零 
	CAN_Send_Msg(controller_id |
			((uint32_t)CAN_PACKET_SET_RPM << 8), buffer, send_index);
}

int32_t buffer_get_int32(const uint8_t *buffer, int32_t *index) {
	int32_t res =	((uint32_t) buffer[*index]) << 24 |
					((uint32_t) buffer[*index + 1]) << 16 |
					((uint32_t) buffer[*index + 2]) << 8 |
					((uint32_t) buffer[*index + 3]);
	*index += 4;
	return res;
}

void ProcessVESCFrame(Frame *Frame_Process)
{
    int32_t get_index = 0;
    EXT_ID_Typedef ExtId;
  
    ExtId = Frame_Process->id.ExtID;

    if(ExtId.Property == CAN_PACKET_STATUS)
    {
        if(ExtId.DesDEVICE_ID == 117)//轮1电机接收
        {
        	SpeedMeasure = (float)buffer_get_int32(Frame_Process->data.uchars,&get_index);
          PositionMeasure = (float)buffer_get_int32(Frame_Process->data.uchars,&get_index)/10000.0f;
        }
    }
}

/*CAN接收中断函数*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *CanNum)
{
	uint32_t i;
	Frame Frame_rcv;
	CAN_RxHeaderTypeDef hCAN_RxHeader;
  HAL_StatusTypeDef	HAL_RetVal;

	
	Rx_Flag =1;  //接收标志位
	HAL_RetVal = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData);
		if(HAL_RetVal == HAL_OK){
			if(RxHeader.IDE == CAN_ID_EXT)
				{
					Frame_rcv.id.all   = RxHeader.ExtId;
					Frame_rcv.isRemote = RxHeader.RTR == CAN_RTR_DATA ? 0 : 1;
					Frame_rcv.length   = RxHeader.DLC;
					memset(Frame_rcv.data.chars, 0, 8);  //先清空再copy
					memcpy(Frame_rcv.data.chars, RxData, Frame_rcv.length);
					ProcessVESCFrame(&Frame_rcv);
				}
		}
	for(i=0;i<RxHeader.DLC;i++)  RxBuf[i]=RxData[i];  //用RxBuf转存RxData的数据
}


/* USER CODE END 1 */
