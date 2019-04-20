#include "malloc.h"
/*********************************************************************************
*************************�������� STM32F407Ӧ�ÿ�����*****************************
**********************************************************************************
* �ļ�����: malloc.c                                                             *
* �ļ��������ڴ����                                                             *
* �������ڣ�2015.03.13                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ����                                                                     * 
**********************************************************************************
*********************************************************************************/

//�ڴ��(4�ֽڶ���)
__align(4) u8 inmenbase[INMEM_MAX_SIZE];													        //�ڲ�SRAM�ڴ��
__align(4) u8 exmen2base[EXMEM_MAX_SIZE] __attribute__((at(0X68000000)));	//�ⲿSRAM�ڴ��
//�ڴ�����
u16 inmemmapbase[INMEM_ALLOC_TABLE_SIZE];													        //�ڲ�SRAM�ڴ��MAP
u16 exmemmapbase[EXMEM_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+EXMEM_MAX_SIZE)));	//�ⲿSRAM�ڴ��MAP
//�ڴ�������	   
const u32 memtblsize[2]={INMEM_ALLOC_TABLE_SIZE,EXMEM_ALLOC_TABLE_SIZE};		//�ڴ���С
const u32 memblksize[2]={INMEM_BLOCK_SIZE,EXMEM_BLOCK_SIZE};					//�ڴ�ֿ��С
const u32 memsize[2]={INMEM_MAX_SIZE,EXMEM_MAX_SIZE};							//�ڴ��ܴ�С

u8 	*membase[2]={inmenbase,exmen2base};			//�ڴ�� ����2��������ڴ�
u16 *memmap[2]={inmemmapbase,exmemmapbase}; //�ڴ����״̬��
u8  memrdy[2]={0,0}; 						            //�ڴ�����Ƿ����

/****************************************************************************
* ��    ��: void mymemcpy(void *des,void *src,u32 n)
* ��    �ܣ������ڴ�
* ��ڲ�����*des:Ŀ�ĵ�ַ
            *src:Դ��ַ
            n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
* ���ز�������
* ˵    ���� 		     
****************************************************************************/
void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	  u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  

/****************************************************************************
* ��    ��: void mymemset(void *s,u8 c,u32 count)  
* ��    �ܣ������ڴ�
* ��ڲ�����*s:�ڴ��׵�ַ
            c :Ҫ���õ�ֵ
            count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
* ���ز�������
* ˵    ���� 		     
****************************************************************************/
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	 

//�ڴ�����ʼ��  
//memx:�����ڴ��
void Memory_Init(u8 memx)  
{  
  mymemset(memmap[memx], 0,memtblsize[memx]*2);//�ڴ�״̬����������  
	mymemset(membase[memx], 0,memsize[memx]);	    //�ڴ��������������  
	memrdy[memx]=1;								//�ڴ�����ʼ��OK  
}  

/****************************************************************************
* ��    ��: u8 Mem_perused(u8 memx)   
* ��    �ܣ���ȡ�ڴ�ʹ����
* ��ڲ�����memx:�����ڴ��
* ���ز�����ʹ����(0~100)
* ˵    ���� 		     
****************************************************************************/
u8 Mem_perused(u8 memx)  
{  
    u32 used=0;  
    u32 i;  
    for(i=0;i<memtblsize[memx];i++)  
    {  
        if(memmap[memx][i])used++; 
    } 
    return (used*100)/(memtblsize[memx]);  
}  

/****************************************************************************
* ��    ��: u32 memin_malloc(u8 memx,u32 size)  
* ��    �ܣ��ڴ����(�ڲ�����)
* ��ڲ�����memx:�����ڴ��
            size:Ҫ������ڴ��С(�ֽ�)
* ���ز�����0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ 
* ˵    ���� 		     
****************************************************************************/
u32 memin_malloc(u8 memx,u32 size)  
{  
    signed long offset=0;  
    u16 nmemb;	//��Ҫ���ڴ����  
	  u16 cmemb=0;//�������ڴ����
    u32 i;  
	
    if(!memrdy[memx])Memory_Init(memx);//δ��ʼ��,��ִ�г�ʼ�� 
    if(size==0)return 0XFFFFFFFF;//����Ҫ����

    nmemb=size/memblksize[memx];  	//��ȡ��Ҫ����������ڴ����
    if(size%memblksize[memx])nmemb++;  
    for(offset=memtblsize[memx]-1;offset>=0;offset--)//���������ڴ������  
    {     
		if(!memmap[memx][offset])cmemb++;//�������ڴ��������
		else cmemb=0;								//�����ڴ������
		if(cmemb==nmemb)							//�ҵ�������nmemb�����ڴ��
		{
            for(i=0;i<nmemb;i++)  					//��ע�ڴ��ǿ� 
            {  
               memmap[memx][offset+i]=nmemb;  
            }  
            return (offset*memblksize[memx]);//����ƫ�Ƶ�ַ  
		}
    }  
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��  
}  

/****************************************************************************
* ��    ��: u8 memin_free(u8 memx,u32 offset)   
* ��    �ܣ��ͷ��ڴ�(�ڲ�����) 
* ��ڲ�����offset:�ڴ��ַƫ��
* ���ز�����0,�ͷųɹ�;1,�ͷ�ʧ��;
* ˵    ���� 		     
****************************************************************************/
u8 memin_free(u8 memx,u32 offset)  
{  
    int i;  
    if(!memrdy[memx])//δ��ʼ��,��ִ�г�ʼ��
	{
		Memory_Init(memx);    
        return 1;//δ��ʼ��  
    }  
    if(offset<memsize[memx])//ƫ�����ڴ����. 
    {  
        int index=offset/memblksize[memx];			//ƫ�������ڴ�����  
        int nmemb=memmap[memx][index];	//�ڴ������
        for(i=0;i<nmemb;i++)  						//�ڴ������
        {  
          memmap[memx][index+i]=0;  
        }  
        return 0;  
    }else return 2;//ƫ�Ƴ�����.  SRAMIN
}  

/****************************************************************************
* ��    ��: void Mem_free(u8 memx,void *use)
* ��    �ܣ��ڴ��ͷ� 
* ��ڲ�����memx: �����ڴ�ѡ��  0���ڲ�    1���ⲿ
            use�� ǰ�������ڴ��׵�ַ 
* ���ز�������
* ˵    ����
****************************************************************************/	
void Mem_free(u8 memx,void *use)  
{  
	u32 offset;  
    if(use==NULL)return;//��ַΪ0.  
 	  offset=(u32)use-(u32)membase[memx];  
    memin_free(memx,offset);//�ͷ��ڴ�     
}  

/****************************************************************************
* ��    ��: void *Mem_malloc(u8 memx,u32 size)
* ��    �ܣ��ڴ����
* ��ڲ�����memx: �����ڴ�ѡ��  0���ڲ�    1���ⲿ
*           size����Ҫ�����ڴ�Ĵ�С
* ���ز��������䵽���ڴ��׵�ַ
* ˵    �����������ABC��Ҫ2K���ڴ�  ���������� ABC=Mem_malloc(0,2048);
            ע��ñ���������ָ����������������׸�Ԫ�ص�ַ ��Ϊ�ú������ص��ǵ�ֵַ
****************************************************************************/	
void *Mem_malloc(u8 memx,u32 size)  
{  
    u32 offset;  									      
	  offset=memin_malloc(memx,size);  	   				   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)membase[memx]+offset);  
}  

/****************************************************************************
* ��    ��: void *Remem_malloc(u8 memx,void *old,u32 size);
* ��    �ܣ������ڴ����
* ��ڲ�����memx: �����ڴ�ѡ��  0���ڲ�    1���ⲿ
            old�� ���ڴ��׵�ַ
*           size����Ҫ���·����ڴ�Ĵ�С
* ���ز��������䵽���ڴ��׵�ַ
* ˵    ����
****************************************************************************/	
void *Remem_malloc(u8 memx,void *old,u32 size)  
{  
    u32 offset;  
    offset=memin_malloc(memx,size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((u32)membase[memx]+offset),old,size);	//�������ڴ����ݵ����ڴ�   
      Mem_free(memx,old);  											  		    //�ͷž��ڴ�
      return (void*)((u32)membase[memx]+offset);  				//�������ڴ��׵�ַ
    }  
} 

