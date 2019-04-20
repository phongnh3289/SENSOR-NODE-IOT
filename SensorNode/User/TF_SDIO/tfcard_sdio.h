#ifndef __SDIO_SDCARD_H
#define __SDIO_SDCARD_H		

#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 

//�û�������			  
//SDIOʱ�Ӽ��㹫ʽ:SDIO_CKʱ��=SDIOCLK/[clkdiv+2];����,SDIOCLK�̶�Ϊ48Mhz
#define SDIO_INIT_CLK        0x76 	//SDIO��ʼ��Ƶ�ʣ����400Kh  
#define SDIO_TRANSFER_CLK    0x00		//SDIO����Ƶ��,��ֵ̫С���ܻᵼ�¶�д�ļ����� 

//SDIO����ģʽ����
#define SD_WORK_MODE  0     // 0:��ѯģʽ   1:DMAģʽ
		  
//SD����Ϣ
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  long long CardCapacity;  	//SD������ ��λΪ���ֽڡ�   ����SDCardInfo.CardCapacity>>20����20λ�õ����������ĵ�λΪ M  
  u32 CardBlockSize; 		    //SD�����С	
  u16 Rlative_ADD;					//����Ե�ַ
  u8 CardType;				      //������
} SD_CardInfo;	

extern SD_CardInfo SDCardInfo;//SD����Ϣ	sd����ʼ��֮����丳ֵ		 

//��غ�������

//SD����ʼ��
SD_Error SD_Init(void);

/****************************************************************************
* ��    ��: u8 SD_ReadSDisk(u8*buf,u32 sector,u8 cnt)
* ��    �ܣ���SD��
* ��ڲ�����buf:�����ݻ�����
            sector:������ַ
            cnt:��������
* ���ز�����0,����;����,���� 
* ˵    ����       
****************************************************************************/	
u8 SD_ReadSDisk(u8*buf,u32 sector,u8 cnt); 	//��SD��,fatfs����

/****************************************************************************
* ��    ��: u8 SD_WriteSDisk(u8*buf,u32 sector,u8 cnt)
* ��    �ܣ�дSD��
* ��ڲ�����buf:д���ݻ�����
            sector:������ַ
            cnt:��������
* ���ز�����0,����;����,���� 
* ˵    ����       
****************************************************************************/	
u8 SD_WriteSDisk(u8*buf,u32 sector,u8 cnt);	//дSD��,fatfs����

#endif 




