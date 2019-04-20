#ifndef __MALLOC_H
#define __MALLOC_H

#include "common.h"

/*********************************************************************************
*************************MCU���� STM32F407���Ŀ�����******************************
**********************************************************************************
* �ļ�����: malloc.h                                                             *
* �ļ��������ڴ����                                                             *
* �������ڣ�2015.03.13                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ����                                                                     * 
**********************************************************************************
*********************************************************************************/

#ifndef NULL
#define NULL 0
#endif

//�ڴ����Ͷ���
#define INSRAM	0	  //�ڲ��ڴ� 
#define EXSRAM  1	  //�ⲿ�ڴ� 

//�ڲ��ڴ�  �ڴ�����õ��Ĺ������  �û��ɸ�����Ҫ�����޸�
#define INMEM_BLOCK_SIZE			32  	   //�ڴ���СΪ32�ֽ�
#define INMEM_MAX_SIZE			80*1024  	 //�������ڲ��ڴ�80K
#define INMEM_ALLOC_TABLE_SIZE	 INMEM_MAX_SIZE/INMEM_BLOCK_SIZE 	//�ڴ���С

//�ⲿ�ڴ�  �ڴ�����õ��Ĺ������  �û��ɸ�����Ҫ�����޸�
#define EXMEM_BLOCK_SIZE			32  	   //�ڴ���СΪ32�ֽ�
#define EXMEM_MAX_SIZE			800*1024   //�������ⲿ�ڴ�800K
#define EXMEM_ALLOC_TABLE_SIZE	 EXMEM_MAX_SIZE/EXMEM_BLOCK_SIZE 	//�ڴ���С
		 
//�ڴ������йغ�������
void Memory_Init(u8 memx);//�ڴ�����ʼ������ memx:�����ڴ�ѡ�� 0:�ڲ�  1:�ⲿ
u8 Mem_perused(u8 memx);	//����ڴ�ʹ����


void *Mem_malloc(u8 memx,u32 size);			 

void *Remem_malloc(u8 memx,void *old,u32 size); 

void Mem_free(u8 memx,void *use);  	

#endif













