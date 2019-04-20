#include "led.h" 

/*********************************************************************************
************************�������� STM32F407���Ŀ�����******************************
**********************************************************************************
* �ļ�����: led.c                                                                *
* �ļ�������LED��ʼ��                                                            *
* �������ڣ�2015.03.03                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ����LED��ӦIO�ڳ�ʼ��                                                    * 
**********************************************************************************
*********************************************************************************/

//LED��ӦIO��ʼ��
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOGʱ��

  //PG13��PG14��PG15��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//LED0��LED1��LED2��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //����
  GPIO_Init(GPIOG, &GPIO_InitStructure);                         //��ʼ��GPIO
	
	GPIO_SetBits(GPIOG, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);  //GPIOG13,G14,G15���øߣ�����

}





