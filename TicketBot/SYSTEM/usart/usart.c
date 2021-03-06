#include "usart.h"
#include "ltdc.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F7������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/6/23
//�汾��V1.5
//��Ȩ���У�����ؾ���?
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
////////////////////////////////////////////////////////////////////////////////// 	  
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������?   
	USART1->TDR=(u8)ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //���ʹ���˽���?
//����1�жϷ������?
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���?   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ��?
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���?	
u16 USART2_RX_STA=0;

u8 aRxBuffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART1_Handler; //UART���?
UART_HandleTypeDef UART2_Handler; //UART���
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�  	  
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
		
//��ʼ��IO ����1 
//bound:������

void uart_init(u32 bound)
{	
	//UART ��ʼ������
	UART1_Handler.Instance=USART3;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);
  
}

void usart2_init(u32 bound)
{	
	//UART ��ʼ������
	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //������
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()��ʹ��UART1
}
//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����?
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��?

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART3)//����Ǵ���?1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART3_CLK_ENABLE();			//ʹ��USART1ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_10;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������?
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF7_USART3;	//����ΪUSART1
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin=GPIO_PIN_11;			//PA10
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PA10
		
#if EN_USART1_RX
		
		HAL_NVIC_EnableIRQ(USART3_IRQn);				//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART3_IRQn,0,0);			//��ռ���ȼ�3�������ȼ�3
#endif	
	}
		if(huart->Instance==USART2)
	{
		  //GPIO�˿�����
		GPIO_InitTypeDef GPIO_Initure;
	
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOBʱ��
		__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART2ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_3;			//PB10
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART2
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PB10

		GPIO_Initure.Pin=GPIO_PIN_2;			//PB11
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PB11
	
		// __HAL_UART_DISABLE_IT(huart,UART_IT_TC);
	//���������ж�
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//ʹ��USART2�ж�
		HAL_NVIC_SetPriority(USART2_IRQn,2,2);			//��ռ���ȼ�2�������ȼ�3
				__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);		
		 TIM7_Int_Init(1000-1,9000-1);		//100ms�ж�
		USART2_RX_STA=0;		//����
		TIM7->CR1&=~(1<<0);        //�رն�ʱ��7
	}

}

//����3,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
		while((USART2->ISR&0X40)==0);			//ѭ������,ֱ���������   
		USART2->TDR=USART2_TX_BUF[j];  
	} 
}

void TIM_SetTIM3Compare4(u32 compare);
void TIM_SetTIM3Compare4(u32 compare);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    u8 len;	
	u16 times=0;
	char printxx[10];
Show_Str(30,50,200,16,"enter callback",16,0);	 		
	if(huart->Instance==USART3)//如果是串�?1
	// {
// 		sprintf(printxx,"%x",USART_RX_STA);
// 		Show_Str(30,70,200,16,printxx,16,0);	
// 		if((USART_RX_STA&0x8000)==0)//接收未完�?
// 		{
// 			//Show_Str(30,80,200,16,"notfinish",16,0);	
// 			if(USART_RX_STA&0x4000)//接收到了0x0d
				
// 			{Show_Str(30,150,200,16,"0x0d",16,0);	
// 				if(aRxBuffer[0]!=0x0a){USART_RX_STA=0;Show_Str(30,90,200,16,"rcverr",16,0);	}//接收错误,重新开�?
// 				else {USART_RX_STA|=0x8000;Show_Str(30,100,200,16,"rcv",16,0);	}	//接收完成�? 

// 		//////////////////////////////////
// {
//        if(USART_RX_STA&0x8000)
		{					   
			/////////////////////
			
			Show_Str(30,110,200,16,"enter function ",16,0);	
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			if(aRxBuffer[0]=='0'){//Í£³µ
				Show_Str(30,130,200,16,"enter function 0",16,0);
			TIM_SetTIM3Compare3(150);	//ÐÞ¸Ä±È½ÏÖµ£¬ÐÞ¸ÄÕ¼¿Õ±È
			TIM_SetTIM3Compare4(150);	
			}
		else if(aRxBuffer[0]=='1'){//Ç°½ø
			Show_Str(30,150,200,16,"enter function 1",16,0);
			TIM_SetTIM3Compare3(155);	//ÐÞ¸Ä±È½ÏÖµ£¬ÐÞ¸ÄÕ¼¿Õ±È
			TIM_SetTIM3Compare4(150);
			}
		else if(aRxBuffer[0]=='2'){//Ç°×ó
			Show_Str(30,170,200,16,"enter function 2",16,0);
			TIM_SetTIM3Compare3(155);	//ÐÞ¸Ä±È½ÏÖµ£¬ÐÞ¸ÄÕ¼¿Õ±È
			TIM_SetTIM3Compare4(190);
			}
		else if(aRxBuffer[0]=='3'){//Ç°ÓÒ
			Show_Str(30,190,200,16,"enter function 3",16,0);
			TIM_SetTIM3Compare3(155);	//ÐÞ¸Ä±È½ÏÖµ£¬ÐÞ¸ÄÕ¼¿Õ±È
			TIM_SetTIM3Compare4(110);
			}
		else{//ºóÍË
			Show_Str(30,110,200,16,"enter function elsel",16,0);
			TIM_SetTIM3Compare3(150);	//ÐÞ¸Ä±È½ÏÖµ£¬ÐÞ¸ÄÕ¼¿Õ±È
			TIM_SetTIM3Compare4(150);
				}
		
			HAL_UART_Transmit(&UART1_Handler,(uint8_t*)(aRxBuffer),10,1000);	//发送接收到的数�?
			//while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//等待发送结�?
			USART_RX_STA=0;
			///////////////////////
		}
// 	}
// //////////////////////////////////////////////
// 			}
// 			else //还没收到0X0D
// 			{	
// 				if(aRxBuffer[0]==0x0d)USART_RX_STA|=0x4000;
// 				else
// 				{
// 					USART_RX_BUF[USART_RX_STA&0X3FFF]=aRxBuffer[0] ;
// 					USART_RX_STA++;
// 					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接�?	  
// 				}		 
// 			}
// 		}

	// }
}
void USART2_IRQHandler(void)
{
	u8 res;	      //HAL_UART_IRQHandler(&UART2_Handler);
	u8 printbuf[40];
			// sprintf(printbuf,"USART2_IRQHandler\r\n");//串口打印JPEG文件大小
			// HAL_UART_Transmit(&UART1_Handler,(uint8_t*)(printbuf),40,1000);
			// 	while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);
			// clrStr(printbuf,40); 
	if(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE)!=RESET)//���յ�����
	{	 
//		HAL_UART_Receive(&UART3_Handler,&res,1,1000);
		res=USART2->RDR; 			 
		if((USART2_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{ 
			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//�����Խ�������
			{
//				__HAL_TIM_SetCounter(&TIM7_Handler,0);	
				TIM7->CNT=0;         				//���������	
				if(USART2_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
				{
//					__HAL_RCC_TIM7_CLK_ENABLE();            //ʹ��TIM7ʱ��
					TIM7->CR1|=1<<0;     			//ʹ�ܶ�ʱ��7
				}
				USART2_RX_BUF[USART2_RX_STA++]=res;	//��¼���յ���ֵ	 
			}else 
			{
				USART2_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
				__HAL_UART_CLEAR_IT(&UART2_Handler,UART_FLAG_RXNE);
			} 
		}
	} else{
		CLEAR_BIT(UART2_Handler.Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
	}
			// 	sprintf(printbuf,"USART2_IRQHandler\r\n");//串口打印JPEG文件大小
			// HAL_UART_Transmit(&UART1_Handler,(uint8_t*)(printbuf),40,1000);
			// 	while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);
			// clrStr(printbuf,40); 
	 				 											 
}   
 



//����1�жϷ������?
void USART3_IRQHandler(void)                	
{ 
	u32 timeout=0;
    u32 maxDelay=0x1FFFF;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函�?
	
	timeout=0;
    while (HAL_UART_GetState(&UART1_Handler)!=HAL_UART_STATE_READY)//等待就绪
	{
        timeout++;////超时处理
        if(timeout>maxDelay) break;		
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART1_Handler,(u8 *)aRxBuffer, RXBUFFERSIZE)!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount�?1
	{
        timeout++; //超时处理
        if(timeout>maxDelay) break;	
	}
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
} 
// void USART3_IRQHandler(void)                	
// { 
// 	u8 Res;
// #if SYSTEM_SUPPORT_OS	 	//ʹ��OS
// 	OSIntEnter();    
// #endif
// 	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
// 	{
//         HAL_UART_Receive(&UART1_Handler,&Res,1,1000); 
// 		if((USART_RX_STA&0x8000)==0)//����δ���?
// 		{
// 			if(USART_RX_STA&0x4000)//���յ���0x0d
// 			{
// 				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
// 				else USART_RX_STA|=0x8000;	//���������? 
// 			}
// 			else //��û�յ�0X0D
// 			{	
// 				if(Res==0x0d)USART_RX_STA|=0x4000;
// 				else
// 				{
// 					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
// 					USART_RX_STA++;
// 					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
// 				}		 
// 			}
// 		}   		 
// 	}
// 	HAL_UART_IRQHandler(&UART1_Handler);	
// #if SYSTEM_SUPPORT_OS	 	//ʹ��OS
// 	OSIntExit();  											 
// #endif
// } 
#endif	


/****************************************************************************************/
/****************************************************************************************/
/*************************�������ͨ���ڻص������б�д�жϿ����߼�?*********************/
/****************************************************************************************
***************************************************************************************************
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//����Ǵ���?1
	{
		if((USART_RX_STA&0x8000)==0)//����δ���?
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(aRxBuffer[0]!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//���������? 
			}
			else //��û�յ�0X0D
			{	
				if(aRxBuffer[0]==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=aRxBuffer[0] ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}

	}
}
 
//����1�жϷ������?
void USART1_IRQHandler(void)                	
{ 
	u32 timeout=0;
    u32 maxDelay=0x1FFFF;
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART1_Handler);	//����HAL���жϴ������ú���
	
	timeout=0;
    while (HAL_UART_GetState(&UART1_Handler)!=HAL_UART_STATE_READY)//�ȴ�����
	{
        timeout++;////��ʱ����
        if(timeout>maxDelay) break;		
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART1_Handler,(u8 *)aRxBuffer, RXBUFFERSIZE)!=HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
        timeout++; //��ʱ����
        if(timeout>maxDelay) break;	
	}
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntExit();  											 
#endif
} 
#endif	
*/



 




