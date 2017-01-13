#include "sys.h"
#include "delay.h"
#include "circle_array.h"

/**
*�������ڵ�ʵ��(IOģ�⴮��)
* �����ʣ�9600    1-8-N
* TXD : PC13
* RXD : PB14
* ʹ���ⲿ�ж϶�RXD���½��ؽ��д�����ʹ�ö�ʱ��4����9600�����ʽ��ж�ʱ���ݽ��ա�
* Demo����: ����11�����ݣ�Ȼ��ѽ��յ������ݷ��ͳ�ȥ
*/


#define OI_TXD	PGout(5)
#define OI_RXD	PGin(7)

#define BuadRate_9600	100

u8 len = 0;	//���ռ���
u8 USART_buf[128];  //���ջ�����

enum{
	COM_START_BIT,
	COM_D0_BIT,
	COM_D1_BIT,
	COM_D2_BIT,
	COM_D3_BIT,
	COM_D4_BIT,
	COM_D5_BIT,
	COM_D6_BIT,
	COM_D7_BIT,
	COM_STOP_BIT,
};

u8 recvStat = COM_STOP_BIT;
u8 recvData = 0;

void IO_TXD(u8 Data)
{
	u8 i = 0;
	OI_TXD = 0;  
	delay_us(BuadRate_9600);
	for(i = 0; i < 8; i++)
	{
		if(Data&0x01)
			OI_TXD = 1;  
		else
			OI_TXD = 0; 	
		
		delay_us(BuadRate_9600);
		Data = Data>>1;
	}
	OI_TXD = 1;
	delay_us(BuadRate_9600);
}
	
void USART_Send(u8 *buf, u8 len)
{
	u8 t;
	for(t = 0; t < len; t++)
	{
		IO_TXD(buf[t]);
	}
}
	
 void IOConfig(void)
 {
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PB,PC�˿�ʱ�� 
	 
	 //SoftWare Serial TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	 
  GPIO_Init(GPIOG, &GPIO_InitStructure);	  				
  GPIO_SetBits(GPIOG,GPIO_Pin_5); 						
	 
	 
	//SoftWare Serial RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOG, &GPIO_InitStructure);	 

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource7);
	EXTI_InitStruct.EXTI_Line = EXTI_Line7;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling; //�½��ش����ж�
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStruct);


	NVIC_InitStructure.NVIC_IRQChannel= EXTI9_5_IRQn ; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  
	
}
 
void TIM4_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,���������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���			 
}
 
 
 int main(void)
 {
   u8 led_show[] = {0xA1, 0x17, 0xE8, 0xFF, 0x01, 0xB0, 0xB2, 0xC8, 0xAB, 0xC9, 0xFA, 0xB2, 0xFA, 0xB1, 0xA3, 0xD6, 0xCA, 0xB1, 0xA3, 0xC1, 0xBF, 0xAC, 0x51};
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	 delay_init();
	 IOConfig();
  TIM4_Int_Init(107, 71);	 //1M����Ƶ��
	 
	 
	 USART_Send(led_show,sizeof(led_show));
  while(1)
	{
		if (current_queue_size>0)
        {
            u8 len,i,j;
            //delay_ms(800);
            //delay_ms(5);
					for (i=0;i<200;i++)
					{
						for (j=0;j<200;j++)
              ;
					}						

            //VUWByte(0x0d);
            //VUWByte(0x0a);
            for (len=0;len<current_queue_size;len++)
            {
                IO_TXD(read_data());
                delete_data();
            }
					}
				}
			

}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line7) != RESET)
	{
		if(OI_RXD == 0) 
		{
			if(recvStat == COM_STOP_BIT)
			{
				recvStat = COM_START_BIT;
				TIM_Cmd(TIM4, ENABLE);
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
}

void TIM4_IRQHandler(void)
{  
	if(TIM_GetFlagStatus(TIM4, TIM_FLAG_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);	
		 recvStat++;
		if(recvStat == COM_STOP_BIT)
		{
			TIM_Cmd(TIM4, DISABLE);
			add_data(recvData);	
			return;
		}
		if(OI_RXD)
		{
			recvData |= (1 << (recvStat - 1));
		}else{
			recvData &= ~(1 << (recvStat - 1));
		}	
  }		
}