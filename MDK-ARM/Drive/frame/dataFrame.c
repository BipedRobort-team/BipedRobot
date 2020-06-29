/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      dataFrame.c                                                                                                 ***/
/***Author:                                                                                                                    ***/
/***Version:       1.0                                                                                                         ***/
/***Date:                                                                                                                      ***/ 
/***Description:   ������֡                                                                                                    ***/
/**                                                                                                                            ***/
/**                                                                                                                            ***/
/***Others:                                                                                                                    ***/
/***Function List:                                                                                                             ***/
/**    1.                                                                                                                      ***/
/**    2.                                                                                                                      ***/
/***History:                                                                                                                   ***/
/**    1.Date:   2020/06/24                                                                                                    ***/
/**      Author: Caodandan                                                                                                     ***/
/**      Modification:                                                                                                         ***/
/**    2.                                                                                                                      ***/
/*********************************************************************************************************************************/
#include "dataFrame.h"


/* External variables -----------------------------------------------------------------------------------------------------------*/
volatile u8 *Motor_Buffer;
u8 *Motor_Mid_Buffer;
u8 *sendBuffer;
volatile u8 *IMU_Buffer;
volatile u8 *MCU_Buffer;
volatile u8 *Left_Buffer;
volatile u8 *Right_Buffer;
//���豸���ݴ�����ɱ�־
u8 IMU_Ready_Flag = 0;
u8 MCU_Ready_Flag = 0;
u8 DXL_Ready_Flag = 0;
u8 Left_Ready_Flag = 0;
u8 Right_Ready_Flag = 0;
//volatile u8 *sendBuffer;


/* Private function prototypes --------------------------------------------------------------------------------------------------*/
/** @addtogroup dataFrame_Private_Functions
  * @{
  */
void Data_Send(void);
void Packet_Frame(void);
void MCU_Frame(void);
void Left_Frame(void);
void Right_Frame(void);
void IMU_Frame(void);
void Motor_Frame(void);



/*********************************************************************************************************************************/
/**Function Name:         Data_Send                                                                                            **/
/**Function Description:  ��������                                                                                               **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Data_Send(void)
{
	
	Motor_Frame();
//	IMU_Frame();
//	MCU_Frame();
//	Left_Frame();
//	Right_Frame();
	Packet_Frame();

}
/*********************************************************************************************************************************/
/**Function Name:         MCU_Frame                                                                                             **/
/**Function Description:  MCU����������֡����                                                                                    **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void MCU_Frame(void)
{
	MCU_Ready_Flag = 1;
}
/*********************************************************************************************************************************/
/**Function Name:         Left_Frame                                                                                            **/
/**Function Description:  ѹ����������ŷ���������֡����                                                                          **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Left_Frame(void)
{
	Left_Ready_Flag  = 1;
}
/*********************************************************************************************************************************/
/**Function Name:         Motor_Frame                                                                                           **/
/**Function Description:  ѹ���������ҽŷ���������֡����                                                                          **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Right_Frame(void)
{
	Right_Ready_Flag = 1;
}
/*********************************************************************************************************************************/
/**Function Name:         Motor_Frame                                                                                           **/
/**Function Description:  �������������֡����                                                                                   **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Motor_Frame(void)
{
	int i = 0;
	Motor_Buffer[0] = 0x05;
	Motor_Buffer[1] = 0x00;
	Motor_Buffer[2] = 0x00;
	
	if(UART1RxEndFlag && Motor_Read_Flag)//���յ���������������ڴ�����ǰ��λ���Ѿ����Ͷ����ָ��
	{
		switch(Motor_Read_Cmd)
		{
			case 0x02://read
//				Motor_Buffer = (u8 *)malloc(UART1RxLen*sizeof(u8));
//				memset(Motor_Buffer,0,UART1RxLen);
				Motor_Length_Sign += UART1RxLen;
				for(i = 3;i<Motor_Length_Sign;i++)
				{
					Motor_Buffer[i] = DXL_aRxBuffer[i]; //����volatile
				}
					/* Progress the reveiving data */
	//			DXL_SendData(DXL_aTxBuffer,UART1RxLen);
//				USB2UART_SendData(DXL_aTxBuffer,UART1RxLen);
				DXL_Ready_Flag = 1;
				UART1RxEndFlag = 0;
				UART1RxLen = 0;
				return;
				break;
			case 0x82://Sync Read
				Sync_Read_Len--;
			//�˴�����malloc������������
				for(i = 0;i<UART1RxLen;i++)
				{
					Motor_Buffer[i+Motor_Length_Sign] = DXL_aRxBuffer[i];
				}
				Motor_Length_Sign += UART1RxLen;
				UART1RxEndFlag = 0;
				UART1RxLen = 0;
			//ƴ������
				if(Sync_Read_Len == 0)
				{
//					Motor_Mid_Buffer = (u8 *)malloc(Motor_Length_Sign*sizeof(u8));
//					memset(Motor_Mid_Buffer,0,Motor_Length_Sign);
//					for(i = 0;i<Motor_Length_Sign;i++)//������CRCУ����
//					{
//						Motor_Mid_Buffer[i] = Motor_Buffer[i];
//					}
					//�˴�Ϊƴ֡����
					USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);//�˴��������Ż�����IMUƴ����֡
//					USB2UART_SendData(&Motor_Length_Sign,1);
					DXL_Ready_Flag = 1;
					return;
				}
				break;
			case 0x92://Bulk Read
				Bulk_Read_Len--;
				for(i = 0;i<UART1RxLen;i++)
				{
					Motor_Buffer[i+Motor_Length_Sign] = DXL_aRxBuffer[i];
				}
				Motor_Length_Sign += UART1RxLen;
				UART1RxEndFlag = 0;
				UART1RxLen = 0;
			//ƴ������,�����ݴ���һ������У�������ƴ����ɴ���
			if(Bulk_Read_Len == 0)
			{
				USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);//�˴��������Ż�����IMUƴ����֡
				DXL_Ready_Flag = 1;
				return;
			}
				break;
			default:
				//����λ�������豸���������
				return;
		}
	}
}

/*********************************************************************************************************************************/
/**Function Name:         IMU_Frame                                                                                             **/
/**Function Description:  �������������֡����                                                                                   **/
/**Inputs:                UINT8 blockNum                                                                                        **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void IMU_Frame(void)
{
	if(IMU_Read_Flag)
	{
		
		//�ӿں���ȡIMU����ֵ
//		IMU_Read_Flag = 0;
		
		IMU_Ready_Flag = 1;
	}
}


/*********************************************************************************************************************************/
/**Function Name:         Packet_Frame                                                                                           **/
/**Function Description:  ��������������֡����                                                                                    **/
/**Inputs:                UINT8 blockNum                                                                                        **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Packet_Frame(void)
{
	u8 sendBufferLen;
	u8 i = 5,j = 0;
	u16 CRC16=0;
	u8 CRC_L=0,CRC_H=0;
	
//	sendBuffer = (u8 *)malloc(30*sizeof(u8));
//	memset(sendBuffer,0,30);
//	sendBuffer[0] = 0xFF;
//	sendBuffer[1] = 0xFD;
//	sendBuffer[2] = 0x00;
//	sendBuffer[3] = 0x00;
//	sendBuffer[4] = 0x55;
	if((MCU_Ready_Flag)||(Left_Ready_Flag)||(Right_Ready_Flag)||(IMU_Ready_Flag)||(DXL_Ready_Flag))
	{
	//	if(MCU_Read_Flag)
	//	{
	//		for(j = 0;j < 4;j++)
	//		{
	//			sendBuffer[i] = MCU_Buffer[j];
	//			i++;
	//		}
	//		memset(MCU_Buffer,0,4);
	//		MCU_Read_Flag = 0;
//		MCU_Ready_Flag = 0;
	//	
	//	}
	//	if(Left_Read_Flag)
	//	{
	//		for(j = 0;j < 5;j++)
	//		{
	//			sendBuffer[i] = Left_Buffer[j];
	//			i++;
	//		}
	//		memset(Left_Buffer,0,4);
	//		Left_Read_Flag = 0;
//	Left_Ready_Flag = 0;
	//	}
	//	if(Right_Read_Flag)
	//	{
	//		for(j = 0;j < 5;j++)
	//		{
	//			sendBuffer[i] = Right_Buffer[j];
	//			i++;
	//		}
	//		memset(Right_Buffer,0,4);
	//		Right_Read_Flag = 0;
//	Right_Ready_Flag = 0;
	//	}
//		if(IMU_Read_Flag)
//		{
//			for(j = 0;j < 65;j++)//�˴����ȴ�����ΪIMU����
//			{
//				sendBuffer[i] = IMU_Buffer[j];
//				i++;
//			}
//			
//			memset(IMU_Buffer,0,65);//���ȴ�����
//			IMU_Read_Flag = 0;
//			IMU_Ready_Flag = 0;
//		
//		}
		if(DXL_Ready_Flag)
		{
//			USB2UART_SendData(&Motor_Length_Sign,1);
//			USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);
//			Motor_Mid_Buffer = (u8 *)malloc(Motor_Length_Sign*sizeof(u8));
//			memset(Motor_Mid_Buffer,0,Motor_Length_Sign);
//			for(i = 0;i<Motor_Length_Sign;i++)//������CRCУ����
//			{
//				Motor_Mid_Buffer[i] = Motor_Buffer[i];
//			}
//			USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);
//			for(j = 0;j < Motor_Length_Sign;j++)//�˴�Ӧ���Գ��ȱ߽�
//			{
//				sendBuffer[i] = Motor_Mid_Buffer[j];
//				i++;
//				
//			}
//			
//			DXL_SendData(&i,1);
//			DXL_SendData(Motor_Mid_Buffer,Motor_Length_Sign);
//			free(Motor_Mid_Buffer);
//			USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);
//			DXL_SendData(sendBuffer,Motor_Length_Sign);
//			free(Motor_Mid_Buffer);
//			memset(Motor_Buffer,0,Motor_Length_Sign);
			Motor_Length_Sign = 3;
			Motor_Read_Cmd = 0;
			Motor_Read_Flag = 0;
			DXL_Ready_Flag = 0;
			free(sendBuffer);
	//		UART1RxEndFlag = 0;
	//		UART1RxLen = 0;
//		
		}
//		//ȡ���鳤����CRCУ��
//		CRC16 = update_crc(0, sendBuffer, i-1);
//		CRC_L = CRC16 & 0xff;
//		CRC_H = CRC16 >> 8;
//			
//		sendBuffer[i] =  CRC_L;
//		i++;
//		sendBuffer[i] =  CRC_H;
//		sendBufferLen = i-1;
//		sendBuffer[2] = sendBufferLen; // �˴�Ĭ�ϳ��Ȳ�����255
//		USB2UART_SendData(sendBuffer,sendBufferLen);
//		memset(sendBuffer,0,UART1RxLen);
}
	
}
