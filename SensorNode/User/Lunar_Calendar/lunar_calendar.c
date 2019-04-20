#include "lucalendar.h"
#include "rtc.h"
#include "noli.h"

/*********************************************************************************
*************************�������� STM32F407Ӧ�ÿ�����*****************************
**********************************************************************************
* �ļ�����: lucalendar.c                                                         *
* �ļ�������RTCũ��ʵ��                                                          *
* �������ڣ�2018.03.19                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ������ǿ��������֧��ũ����24�����������������Ĺ�������ʱ����1970��ΪԪ��,*
            ��32bit��ʱ��Ĵ����������е�2100������                              * 
**********************************************************************************
*********************************************************************************/

//�²���������ũ������Ҫʹ�õ�
//�·����ݱ�
u8  const day_code1[9]={0x0,0x1f,0x3b,0x5a,0x78,0x97,0xb5,0xd4,0xf3};
unsigned short const day_code2[3]={0x111,0x130,0x14e};
//u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
u8 const *sky[10]=  {"��","��","��","��","��","��","��","��","��","��",};//���
u8 const *earth[12]={"��","��","��","î","��","��","��","δ","��","��","��","��",}; //��֧
u8 const *monthcode[12]={"һ","��","��","��","��","��","��","��","��","ʮ","��","��",};//ũ���·�
u8 const *nongliday[4]={"��","ʮ","إ","��",}; //ũ������  

///////////////////////////////////////////////////////////////////////
//֧�ִ�1900�굽2099���ũ����ѯ
//֧�ִ�2000�굽2050��Ľ�����ѯ
//�Ӻ���,���ڶ�ȡ���ݱ���ũ���µĴ��»�С��,�������Ϊ�󷵻�1,ΪС����0
u8 GetMoonDay(u8 month_p,unsigned short table_addr)
{
	switch (month_p)
	{
		case 1:
			if((year_code[table_addr]&0x08)==0)	return(0);
			else 								return(1); 
		case 2:
			if((year_code[table_addr]&0x04)==0)	return(0);
			else 								return(1);
		case 3:
			if((year_code[table_addr]&0x02)==0)	return(0);
			else 								return(1);
		case 4:
			if((year_code[table_addr]&0x01)==0)	return(0);
			else 								return(1);
		case 5:
			if((year_code[table_addr+1]&0x80)==0)	return(0);
			else 									return(1);
		case 6:
			if((year_code[table_addr+1]&0x40)==0)	return(0);
			else 									return(1);
		case 7:
			if((year_code[table_addr+1]&0x20)==0)	return(0);
			else 									return(1);
		case 8:
			if((year_code[table_addr+1]&0x10)==0)	return(0);
			else 									return(1);
		case 9:
			if((year_code[table_addr+1]&0x08)==0)	return(0);
			else 									return(1);
		case 10:
			if((year_code[table_addr+1]&0x04)==0)	return(0);
			else 									return(1);
		case 11:
			if((year_code[table_addr+1]&0x02)==0)	return(0);
			else 									return(1);
		case 12:
			if((year_code[table_addr+1]&0x01)==0)	return(0);
			else 									return(1);
		case 13:
			if((year_code[table_addr+2]&0x80)==0)	return(0);
			else 									return(1);
	}
	return(0);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////// 
// ��������:GetChinaCalendar
//��������:��ũ��ת��(ֻ����1901-2099��)
// �䡡��:  year        ������
//          month       ������
//          day         ������
//          p           ����ũ�����ڵ�ַ
// �䡡��:  1           �ɹ�
//          0           ʧ��																			 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
u8 GetChinaCalendar(u16  year,u8 month,u8 day,u8 *p)
{ 
	u8 temp1,temp2,temp3,month_p,yearH,yearL;	
	u8 flag_y;
	unsigned short temp4,table_addr;

	yearH=year/100;	yearL=year%100;//��ݵĸߵ������ֽ� 
	if((yearH!=19)&&(yearH!=20))return(0);//���ڲ���19xx ~ 20xx ��Χ��,���˳�
	
	// ��λ���ݱ��ַ  
	if(yearH==20)	table_addr=(yearL+100-1)*3;
	else  			table_addr=(yearL-1)*3;

	// ȡ���괺�����ڵĹ����·�  
	temp1=year_code[table_addr+2]&0x60;	
	temp1>>=5;

	// ȡ���괺�����ڵĹ�����  
	temp2=year_code[table_addr+2]&31; 

	// ���㵱�괺���뵱��Ԫ��������,����ֻ���ڹ���1�»�2��  
	if(temp1==1) 	temp3=temp2-1; 
	else 			temp3=temp2+31-1; 

	// ���㹫�����뵱��Ԫ��������  
	if (month<10) 	temp4=day_code1[month-1]+day-1;
	else  			temp4=day_code2[month-10]+day-1;
	// ��������´���2�²��Ҹ����2��Ϊ����,������1  
	if ((month>2)&&(yearL%4==0)) 	temp4++;

	// �жϹ������ڴ���ǰ���Ǵ��ں�  
	if (temp4>=temp3)
	{ 						
		temp4-=temp3;
		month=1;
		month_p=1;
							
		flag_y=0;
		if(GetMoonDay(month_p,table_addr)==0)	temp1=29; //С��29��
		else 									temp1=30; //��С30��
		// �����ݱ���ȡ����������·�,��Ϊ0�����������  
		temp2=year_code[table_addr]/16; 	
		while(temp4>=temp1)
		{
			temp4-=temp1;
			month_p++;
			if(month==temp2)
			{
				flag_y=~flag_y;
				if(flag_y==0)month++;
			}
			else month++;
			if(GetMoonDay(month_p,table_addr)==0)	temp1=29;
			else 									temp1=30;
		}
		day=temp4+1;
	}
	// �������ڴ���ǰʹ����������������  
	else
	{ 						
		temp3-=temp4;
		if (yearL==0)
		{
			yearL=100-1;
			yearH=19;
		}
		else yearL--;
		table_addr-=3;
		month=12;
		temp2=year_code[table_addr]/16; 	
		if (temp2==0)	month_p=12; 
		else 			month_p=13; 

		flag_y=0;
		if(GetMoonDay(month_p,table_addr)==0)	temp1=29; 
		else 									temp1=30; 
		while(temp3>temp1)
		{
			temp3-=temp1;
			month_p--;
			if(flag_y==0)		month--;
			if(month==temp2)	flag_y=~flag_y;
			if(GetMoonDay(month_p,table_addr)==0)	temp1=29;
			else 									temp1=30;
		}
		day=temp1-temp3+1;
	}

	*p++=yearH;
	*p++=yearL;
	*p++=month;
	*p=day;	
	return(1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������:GetSkyEarth
// ��������:���빫�����ڵõ�һ��������(ֻ����1901-2099��)
// �䡡��:  year        ������
//          p           �������ڵ�ַ
// �䡡��:  ��																							   
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void GetSkyEarth(u16 year,u8 *p)
{
	u8 x;
	
	if(year>=1984)
	{
		year=year-1984;
		x=year%60;				
	}
	else
	{
		year=1984-year;
		x=60-year%60;
	}
	*p=x;
}
//��ָ���ַ�source����no����target
void StrCopyss(u8 *target,u8 const *source,u8 no)
{
	u8 i;
	for(i=0;i<no;i++)
	{
 		 *target++=*source++;
	}
}

/********************************************************************************
*��������:void GetLunarCalendarStr(u16 year,u8 month,u8 day,u8 *str)
*��������:���빫�����ڵõ�ũ���ַ���	          
*��ڲ���: year        ������
*          month       ������
*          day         ������
*          str         ����ũ�������ַ�����ַ    
*�������: ��	
*˵    ����GetLunarCalendarStr(2015,03,15,str) ����*str="��δ������إ��"
********************************************************************************/
void GetLunarCalendarStr(u16 year,u8 month,u8 day,u8 *str)
{
	u8 NLyear[4];
	u8 SEyear;
	
	StrCopyss(&str[0],(u8 *)"���������³�һ",15);
	if(GetChinaCalendar(year,month,day,(u8 *)NLyear)==0)	return;
	GetSkyEarth(NLyear[0]*100+NLyear[1],&SEyear);
	StrCopyss(&str[0],(u8 *)  sky[SEyear%10],2);	//  ��
	StrCopyss(&str[2],(u8 *)earth[SEyear%12],2);	//  ��	
	
	if(NLyear[2]==1)	StrCopyss(&str[6],(u8 *)"��",2);
	else				StrCopyss(&str[6],(u8 *)monthcode[NLyear[2]-1],2);		
	
	if(NLyear[3]>10) 	StrCopyss(&str[10],(u8 *)nongliday[NLyear[3]/10],2);	
	else				StrCopyss(&str[10],(u8 *)"��",2);
	StrCopyss(&str[12],(u8 *)monthcode[(NLyear[3]-1)%10],2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������:GetJieQi
// ��������:���빫�����ڵõ�����24�������� day<15�����ϰ��½���,��֮�����°���	��������
//          ��:GetJieQiStr(2007,02,08,str) ����str[0]=4
// �䡡��:  year        ������
//          month       ������
//          day         ������
//          str         �����Ӧ���½������ڵ�ַ   1Byte
// �䡡��:  1           �ɹ�
//          0           ʧ��																			  
/////////////////////////////////////////////////////////////////////////////////////////////////////////
u8 GetJieQi(u16 year,u8 month,u8 day,u8 *JQdate)
{
	u8 bak1,value,JQ;

	if((year<2000)||(year>2050))     return 0;//������ķ�Χ����
	if((month==0) ||(month>12))      return 0;
	JQ = (month-1) *2 ;		                        //��ý���˳����(0��23
	if(day >= 15) JQ++; 	                        //�ж��Ƿ����ϰ���

	bak1=YearMonthBit[(year-2000)*3+JQ/8];        //��ý����������ֵ�����ֽ�  
	value =((bak1<<(JQ%8))&0x80);                 //��ý����������ֵ״̬

	*JQdate=days[JQ];								//�õ�����������
	if( value != 0 )
	{
		//�ж����,�Ծ����������ֵ1����1,���ǣ�1��
		if( (JQ== 1||JQ== 11||JQ== 18||JQ== 21)&&year< 2044)  (*JQdate)++;
		else                                                  (*JQdate)--;
	}
	return 1;
}

static u8 const MonthDayMax[]={31,28,31,30,31,30,31,31,30,31,30,31,};
/********************************************************************************
*��������:u8 GetJieQiStr(u16 year,u8 month,u8 day,u8 *str)
*��������:���빫�����ڵõ������ַ���	          
*��ڲ���: year        ������
*          month       ������
*          day         ������
*          str         ���ս��������ַ�����ַ   15Byte
*�������: ��	
*˵    ����GetLunarCalendarStr(2015,06,07,str) ����*str="����������15��"
********************************************************************************/
u8 GetJieQiStr(u16 year,u8 month,u8 day,u8 *str)
{
	u8 JQdate,JQ,MaxDay;

	if(GetJieQi(year,month,day,&JQdate)==0)	return 0;
 
	JQ = (month-1) *2 ;                             //��ý���˳����(0��23
	if(day >= 15) JQ++;                             //�ж��Ƿ����ϰ���

	if(day==JQdate)                                 //��������һ��������
	{
		StrCopyss(str,(u8 *)JieQiStr[JQ],5);
		return 1;
	}
                                              //���첻��һ��������
	  StrCopyss(str,(u8 *)"����������??��",15);

	if(day<JQdate)                                  //�����������С�ڱ��µĽ�������
	{
		StrCopyss(&str[2],(u8 *)JieQiStr[JQ],4);
		day=JQdate-day;
	} 

	else                                            //����������ڴ��ڱ��µĽ�������
	{
		if((JQ+1) >23)  return 0;
		StrCopyss(&str[2],(u8 *)JieQiStr[JQ+1],4);
		if(day < 15)
		{
			GetJieQi(year,month,15,&JQdate);
			day=JQdate-day;
		}
		else                                        //����
		{
			MaxDay=MonthDayMax[month-1];
			if(month==2)                            //��������
			{
				if((year%4==0)&&((year%100!=0)||(year%400==0))) MaxDay++;
			}
			if(++month==13)	month=1;
			GetJieQi(year,month,1,&JQdate);
			day=MaxDay-day+JQdate;
		}
	}
	str[10]=day/10+'0';
	str[11]=day%10+'0';
	return 1;
}
