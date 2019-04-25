#include "stm32fxxx_hal.h"
#include "stm32f4xx_hal_adc.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_adc.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usart.h"
#include "gsm.h"
#include "cmsis_os.h"
#include "tm_stm32_gpio.h"
//#include "tm_stm32_rtc.h"
#include "stm32f4xx_hal.h"
#include "tm_stm32_rtc.h"
#include "tm_stm32_ds18b20.h"
#include "tm_stm32_onewire.h"
//#include "tm_stm32_fatfs.h"
//#include "fatfs_sd_sdio.h"
//#include "tm_stm32_fatfs.h"
#define DEBUG_USART         USART2
#define DEBUG_USART_PP      TM_USART_PinsPack_1
#define ON 1
#define OFF 0
/* RTC structure */
TM_RTC_t datatime; 
/* RTC Alarm structure */
//TM_RTC_AlarmTime_t RTCA;
TM_OneWire_t OneWire1;
/* Current seconds value */
volatile uint8_t sec = 0, count_time=0, time_loop=5, time_up=4, control_pwr=ON, seek_d=0, dot_array[]={0,0,0},salty_array[]={0,0,0,0,0,0},tds_array[]={0,0,0,0,0,0},wsg_array[]={0,0,0,0,0,0},conduct_array[]={0,0,0,0,0,0};
/* GSM working structure and result enumeration */
gvol GSM_t GSM;
GSM_Result_t gsmRes;
/* SMS read structure */
GSM_SMS_Entry_t SMS_Entry;
int sms_count=0;
/* Pointer to SMS info */
GSM_SmsInfo_t* SMS_Info = NULL;
GSM_DateTime_t* datetime = NULL;
/* GSM pin code */
#define GSM_PIN         "0000"
/* GSM APN settings */
#define GSM_APN         "v-internet"//"m-wap"
#define GSM_APN_USER    ""
#define GSM_APN_PASS    ""

/* Request URL */
#define GSM_HTTP_URL    "https://sml.hcmgis.vn/sos/service"
#define GSM_HTTP_CONTENT    "application/json"
/* Array to receive data */
 uint8_t receive[1000], seek_n=0;
 uint32_t br;
float yo=3500;
#define  	Tx 				10000000.0		// chu ky tinh toan, lay mau du lieu , don vi uS
#define 	RC_ADC	  50000000.0		// thong so loc van toc RC=10^6/(2*pi*f_ca't)
#define  	pi 				3.141593 	// gia tri hang so Pi
/* Array with data to send */
 uint8_t    send[] = "{ \"request\": \"InsertObservation\", \"service\": \"SOS\", \"version\": \"2.0.0\", \"offering\": \"HCMOffering05\", \"observation\":{ \"type\": \"http://www.opengis.net/def/observationType/OGC-OM/2.0/OM_Measurement\", \"procedure\": \"HCMProcedure05\", \"observedProperty\": \"device_power\", \"featureOfInterest\": { \"identifier\": { \"value\": \"HCMFeatureOfInterest05\", \"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\" },\"name\": [ { \"value\": \"HCMSensor05\", \"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\" } ], \"sampledFeature\":[ \"http://www.52north.org/test/featureOfInterest/world\" ], \"geometry\": { \"type\": \"Point\", \"coordinates\": [ 10.674445608668465, 106.76782028497925 ], \"crs\": { \"type\": \"name\", \"properties\": { \"name\": \"EPSG:4326\" } } } },\"phenomenonTime\": \"2011-11-11T11:11:15+07:00\", \"resultTime\": \"2011-11-11T11:11:15+07:00\", \"result\": { \"uom\": \"percent\", \"value\": 10 } } }";
 uint8_t  send_sms[] = "NHIET DO: 25(do C)\nNANG LUONG PIN: 70(%)\nTHOI GIAN LAY MAU: 05 (phut)\nTRANG THAI: OFF\nTHOI GIAN: 00h00";
 uint8_t send_salty[]="{\"request\": \"InsertObservation\",\"service\": \"SOS\",\"version\": \"2.0.0\",\"offering\": \"HCMOffering05\",\"observation\": {\"type\": \"http://www.opengis.net/def/observationType/OGC-OM/2.0/OM_Measurement\",\"procedure\": \"HCMProcedure05\",\"observedProperty\": \"measure_water_salinity\",\"featureOfInterest\": {\"identifier\": {\"value\": \"HCMFeatureOfInterest05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"},\"name\": [{\"value\": \"HCMSensor05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"}],\"sampledFeature\": [\"http://www.52north.org/test/featureOfInterest/world\"],\"geometry\": {\"type\": \"Point\",\"coordinates\": [10.674445608668465,106.76782028497925],\"crs\": {\"type\": \"name\",\"properties\": {\"name\": \"EPSG:4326\"}}}},\"phenomenonTime\": \"2018-10-16T08:53:15+07:00\",\"resultTime\": \"2018-10-26T08:53:15+07:00\",\"result\": {\"uom\": \"perthousand\",\"value\": 40    }}}";
 uint8_t send_temp[]   ="{\"request\": \"InsertObservation\",\"service\": \"SOS\",\"version\": \"2.0.0\",\"offering\": \"HCMOffering05\",\"observation\": {\"type\": \"http://www.opengis.net/def/observationType/OGC-OM/2.0/OM_Measurement\",\"procedure\": \"HCMProcedure05\",\"observedProperty\": \"device_temperature\",\"featureOfInterest\": {\"identifier\": {\"value\": \"HCMFeatureOfInterest05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"},\"name\": [{\"value\": \"HCMSensor05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"}],\"sampledFeature\": [\"http://www.52north.org/test/featureOfInterest/world\"],\"geometry\": {\"type\": \"Point\",\"coordinates\": [10.674445608668465,106.76782028497925],\"crs\": {\"type\": \"name\",\"properties\": {\"name\": \"EPSG:4326\"}}}},\"phenomenonTime\": \"2018-10-16T08:53:15+07:00\",\"resultTime\": \"2018-10-26T08:53:15+07:00\",\"result\": {\"uom\": \"Celsius\",\"value\": 40.0}}}";
 uint8_t send_conduct[]="{\"request\": \"InsertObservation\",\"service\": \"SOS\",\"version\": \"2.0.0\",\"offering\": \"HCMOffering05\",\"observation\": {\"type\": \"http://www.opengis.net/def/observationType/OGC-OM/2.0/OM_Measurement\",\"procedure\": \"HCMProcedure05\",\"observedProperty\": \"measure_water_conductivity\",\"featureOfInterest\": {\"identifier\": {\"value\": \"HCMFeatureOfInterest05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"},\"name\": [{\"value\": \"HCMSensor05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"}],\"sampledFeature\": [\"http://www.52north.org/test/featureOfInterest/world\"],\"geometry\": {\"type\": \"Point\",\"coordinates\": [10.674445608668465,106.76782028497925],\"crs\": {\"type\": \"name\",\"properties\": {\"name\": \"EPSG:4326\"}}}},\"phenomenonTime\": \"2019-01-18T08:53:15+07:00\",\"resultTime\": \"2019-01-18T08:53:15+07:00\",\"result\": {\"uom\": \"uS/cm\",\"value\": 10000 }}}";
 uint8_t send_tds[]    ="{\"request\": \"InsertObservation\",\"service\": \"SOS\",\"version\": \"2.0.0\",\"offering\": \"HCMOffering05\",\"observation\": {\"type\": \"http://www.opengis.net/def/observationType/OGC-OM/2.0/OM_Measurement\",\"procedure\": \"HCMProcedure05\",\"observedProperty\": \"measure_water_total_dissolved_solid\",\"featureOfInterest\": {\"identifier\": {\"value\": \"HCMFeatureOfInterest05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"},\"name\": [{\"value\": \"HCMSensor05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"}],\"sampledFeature\": [\"http://www.52north.org/test/featureOfInterest/world\"],\"geometry\": {\"type\": \"Point\",\"coordinates\": [10.674445608668465,106.76782028497925],\"crs\": {\"type\": \"name\",\"properties\": {\"name\": \"EPSG:4326\"}}}},\"phenomenonTime\": \"2019-01-18T08:53:15+07:00\",\"resultTime\": \"2019-01-18T08:53:15+07:00\",\"result\": {\"uom\": \"ppm\",\"value\": 10000 }}}";
 uint8_t send_wsg[]    ="{\"request\": \"InsertObservation\",\"service\": \"SOS\",\"version\": \"2.0.0\",\"offering\": \"HCMOffering05\",\"observation\": {\"type\": \"http://www.opengis.net/def/observationType/OGC-OM/2.0/OM_Measurement\",\"procedure\": \"HCMProcedure05\",\"observedProperty\": \"measure_water_specific_gravity\",     \"featureOfInterest\": {\"identifier\": {\"value\": \"HCMFeatureOfInterest05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"},\"name\": [{\"value\": \"HCMSensor05\",\"codespace\": \"http://www.opengis.net/def/nil/OGC/0/unknown\"}],\"sampledFeature\": [\"http://www.52north.org/test/featureOfInterest/world\"],\"geometry\": {\"type\": \"Point\",\"coordinates\": [10.674445608668465,106.76782028497925],\"crs\": {\"type\": \"name\",\"properties\": {\"name\": \"EPSG:4326\"}}}},\"phenomenonTime\": \"2019-01-18T08:53:15+07:00\",\"resultTime\": \"2019-01-18T08:53:15+07:00\",\"result\": {\"uom\": \"specific_gravity\",\"value\": 1.003 }}}";

#define EXPECTING_SENSORS	1
/* Thread prototypes */
void GSM_Update_Thread(void const* params);
void GSM_Main_Thread(void const* params);
void GSM_SMS_CHECK(void);
void GSM_SMS_Thread(void const* params);
void Error_Handler(void);
void SEND_STATUS(void);
float lowpass_filter(float x,float *y0,float RC);
//static void MX_GPIO_Init(void);
/* Thread definitions */
osThreadDef(GSM_Update, GSM_Update_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
osThreadDef(GSM_Main, GSM_Main_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
osThreadDef(GSM_SMS, GSM_SMS_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
osThreadId GSM_Update_ThreadId, GSM_Main_ThreadId,GSM_SMS_ThreadId;
RTC_HandleTypeDef hrtc;
volatile char buf[40];
volatile uint8_t devices, i, action=1, unaction=1, ix=0;
uint8_t device[EXPECTING_SENSORS][8];
uint8_t alarm_device[EXPECTING_SENSORS][8];
float temps[EXPECTING_SENSORS];
volatile int32_t adcValue, tempValue, temp_dot;
char buffer_x[40];
void SystemClock_Config(void);
void SystemClock_Config(void);
/* GSM callback declaration */
int GSM_Callback(GSM_Event_t evt, GSM_EventParams_t* params);
int main(void) {
    TM_RCC_InitSystem();                                    /* Init system */
		//SystemClock_Config();  
		HAL_Init(); 
  /* Initialize all configured peripherals */
		//MX_RTC_Init();  
		TM_ADC_Init(ADC1, ADC_CHANNEL_0);
		/* Initialize delay */
		TM_DELAY_Init();	/* Init HAL layer */
    //TM_GPIO_Init(GPIOG, GPIO_Pin_13, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
		//TM_GPIO_Init(GPIOG, GPIO_Pin_14, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
		TM_GPIO_Init(GPIOA, GPIO_Pin_6, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
		//TM_GPIO_Init(GPIOA, GPIO_Pin_7, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
		//TM_GPIO_Init(GPIOE, GPIO_Pin_3, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low);
		//TM_DELAY_Init();
	//MX_GPIO_Init();
    TM_USART_Init(DEBUG_USART, DEBUG_USART_PP, 9600);     /* Init USART for debug purpose */
		TM_USART_Init(USART3, TM_USART_PinsPack_1, 9600);
		TM_RTC_Init(TM_RTC_ClockSource_External);
		/*datatime.Day = 18;
		datatime.Month = 4;
		datatime.Year = 19;
		datatime.WeekDay = 4;
		datatime.Hours = 8;
		datatime.Minutes = 9;
		datatime.Seconds = 0;*/
		//TM_RTC_SetDateTime(&datatime, TM_RTC_Format_BIN);	
    /* Wakeup IRQ every 60 second */
    TM_RTC_Interrupts(TM_RTC_Int_5s);          
    /* Initialize threads */
		//TM_OneWire_Init(&OneWire1, GPIOG, GPIO_PIN_11);
		TM_OneWire_Init(&OneWire1, GPIOE, GPIO_PIN_5);
		//TM_OneWire_Init(&OneWire1, GPIOD, GPIO_PIN_11);
		devices = TM_OneWire_First(&OneWire1);	
		/* Get full ROM value, 8 bytes, give location of first byte where to save */
		TM_OneWire_GetFullROM(&OneWire1, device[0]);		
		TM_DS18B20_SetResolution(&OneWire1, device[0], TM_DS18B20_Resolution_12bits);
		TM_DS18B20_DisableAlarmTemperature(&OneWire1, device[0]);
		/*TM_USART_Puts(USART3, "*OK,0\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "*OK,0\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "L,?\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "C,0\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "O,EC,1\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "O,TDS,1\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "O,S,1\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "O,SG,1\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "C,0\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "O,EC,1\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "O,TDS,1\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "O,S,1\r\n");Delayms(1500);
		TM_USART_Puts(USART3, "O,SG,1\r\n");Delayms(1500);
		*/
		//count_time=0, time_loop=5, time_up=3, control_pwr=ON, seek_d=0;			
    GSM_Update_ThreadId = osThreadCreate(osThread(GSM_Update), NULL);
    GSM_Main_ThreadId = osThreadCreate(osThread(GSM_Main), NULL);
		GSM_SMS_ThreadId = osThreadCreate(osThread(GSM_SMS), NULL);
    /* Start kernel */
    osKernelStart();  
	while (1);
}
void TM_RTC_WakeupHandler() {
	/* Get time */
	//GSM_DATETIME_Get(&GSM, datetime, 1);
	//TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);
	//printf("Time %02d:%02d:%02d    ",datatime.Hours,datatime.Minutes,datatime.Seconds);
	//printf("Ngay:%02d  Thang:%02d  Nam:%02d\n",datatime.Day,datatime.Month,datatime.Year);
	//TM_USART_Puts(USART1,"AT\r\n");
	//TM_USART_Gets(USART1, buffer_y, sizeof(buffer_y));
	//printf(buffer_y);printf("\n");
	if(action==1){
		if(unaction==1){
	if(count_time>=12){count_time=0;time_up++;}
	count_time++;
	//TM_GPIO_TogglePinValue(GPIOG,GPIO_Pin_13);
	//TM_GPIO_TogglePinValue(GPIOE,GPIO_Pin_3);
	TM_GPIO_TogglePinValue(GPIOA,GPIO_Pin_6);
	//V_LION=9.9->2000V->0%, _LION=12.6->2570->100%
	//adcValue
	adcValue=(lowpass_filter(TM_ADC_Read(ADC1, ADC_CHANNEL_0),&yo, RC_ADC)-2000)*100/570;
	//adcValue=TM_ADC_Read(ADC1, ADC_CHANNEL_0);
	
	if(adcValue>99)adcValue=99;
	if(adcValue<1)adcValue=1;
  //printf("Battery: %d\n",adcValue);
		//TM_DS18B20_Read(&OneWire1,device[0], &temps[0]);
		/* Print temperature */
	TM_DS18B20_StartAll(&OneWire1);		
	/* Wait until all are done on one onewire port */
	while (!TM_DS18B20_AllDone(&OneWire1));		
	/* Read temperature from each device separatelly */
	/* Read temperature from ROM address and store it to temps variable */
	TM_DS18B20_Read(&OneWire1,device[0], &temps[0]);
	tempValue=temps[0];
	temp_dot=((temps[0]-tempValue)*10);
	//printf("Temp: %f\n",temps[0]);
	TM_USART_Puts(USART3,"R\r\n");
	TM_USART_Gets(USART3, buffer_x, sizeof(buffer_x));
	//printf(buffer_x);printf("\n");
	for( int a = 0; a < sizeof(buffer_x); a++ )
	{
				if(a>=0){if(buffer_x[a]=='\r')seek_n=a;}
		//if(a>seek_n)buffer_x[a]=0;
	}
	for( int a = 0; a < sizeof(buffer_x); a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){if(a>seek_n)buffer_x[a]=0;}
	}
	seek_d=0;
	for( int a = 0; a < seek_n; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){if(buffer_x[a]==','){dot_array[seek_d]=a;seek_d++;}}
	}
	for( int a = 0; a < dot_array[0]; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){conduct_array[a]=buffer_x[a];}
	}
	for( int a = dot_array[0]; a < 6; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){conduct_array[a]=' ';}
	}
	//conduct_array[dot_array[0]]='\r';
	for( int a = (dot_array[0]+1); a < dot_array[1]; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){salty_array[a-(dot_array[0]+1)]=buffer_x[a];}
	}
	for( int a = (dot_array[1]-dot_array[0]-1); a < 6; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){salty_array[a]=' ';}
	}
	//salty_array[dot_array[1]-dot_array[0]-1]='\r';
	for( int a = (dot_array[1]+1); a < dot_array[2]; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){tds_array[a-(dot_array[1]+1)]=buffer_x[a];}
	}
	for( int a = (dot_array[2]-dot_array[1]-1); a < 6; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){tds_array[a]=' ';}
	}
	//tds_array[dot_array[2]-dot_array[1]-1]='\r';
	for( int a = (dot_array[2]+1); a < seek_n; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){wsg_array[a-(dot_array[2]+1)]=buffer_x[a];}
	}
	for( int a = (seek_n-dot_array[2]-1); a < 6; a++ )
	{
				//if(buffer_x[a]=='\n')seek_n=a;
		if(a>=0){wsg_array[a]=' ';}
	}
}
}
}
/* 1ms handler */
void TM_DELAY_1msHandler() {
    GSM_UpdateTime(&GSM, 1);                /* Update GSM library time for 1 ms */
    osSystickHandler();                     /* Kernel systick handler processing */
		//HAL_IncTick();
}
float lowpass_filter(float x,float *y0,float RC)
{
	float	y;
	y = *y0 + Tx* (x - *y0)/(Tx + RC);
	*y0 = y;
	return(y);
}
/***********************************************/
/**            Thread implementations         **/
/***********************************************/

/**
 * \brief  Update GSM received data thread
 */
void GSM_Update_Thread(void const* params) {
    while (1) {
        /* Process GSM update */
        GSM_Update(&GSM);
    }
}

/**
 * \brief  Application thread to work with GSM module only
 */
void GSM_SMS_Thread(void const* params) { 
while (1) {
        /* Process callback checks */
       if(action==1){
				
	GSM_ProcessCallbacks(&GSM);
                
				 while ((SMS_Info = GSM_SMS_GetReceivedInfo(&GSM, 1)) != NULL) {
					 unaction=0; 
					 sms_count++;
            /* Read SMS from memory */
            if ((gsmRes = GSM_SMS_Read(&GSM, SMS_Info->Position, &SMS_Entry, 1)) == gsmOK) {
                //printf("SMS READ OK!\r\n");
								//printf("Temp: %d; \n", tempValue);
								send_sms[10]=tempValue/10+'0';
								send_sms[11]=tempValue%10+'0';
								send_sms[35]=adcValue/10+'0';
								send_sms[36]=adcValue%10+'0';
								send_sms[60]=time_loop/10+'0';
								send_sms[61]=time_loop%10+'0';
							if(control_pwr==ON)
								{
									send_sms[82]='O';
									send_sms[83]='N';
									send_sms[84]=' ';
								}
								if(control_pwr==OFF)
								{
									send_sms[82]='O';
									send_sms[83]='F';
									send_sms[84]='F';
								}
                /* Make actions according to received SMS string */               
                GSM_SMS_CHECK();
								                /* Send it back to user */
                if (gsmRes == gsmOK) {
                    printf("SEND BACK OK!\r\n");
                } else {
                    printf("Error trying to send: %d\r\n", gsmRes);
                }
							
            } else {
                printf("Error trying to read: %d\r\n", gsmRes);
            }
            
            /* Clear information about new SMS */
            GSM_SMS_ClearReceivedInfo(&GSM, SMS_Info, 1);
						unaction=1; 
						if(sms_count==10){GSM_SMS_MassDelete(&GSM, GSM_SMS_MassDelete_All, 1);sms_count=0;}
}
}	
}
}
void GSM_Main_Thread(void const* params) {    
    /* Init GSM library with PIN code */
    printf("GSM Init status: %d\r\n", GSM_Init(&GSM, GSM_PIN, 115200, GSM_Callback));
    while(1){
			if(unaction==1){ 
		GSM_ProcessCallbacks(&GSM);	
			if(time_loop==time_up){
				action=0;
				time_up=0;
					if(control_pwr==ON){					
						//printf("Value:  %d\n", adcValue);
            TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);
						send_salty[743]=datatime.Hours/10+'0'; 
						send_salty[744]=datatime.Hours%10+'0'; 
						//send[768]=':';
						send_salty[785]=datatime.Hours/10+'0'; 
						send_salty[786]=datatime.Hours%10+'0';
						//send[811]=':';
						//Minutes
						send_salty[746]=datatime.Minutes/10+'0'; 
						send_salty[747]=datatime.Minutes%10+'0'; 
						//send[771]=':';
						send_salty[788]=datatime.Minutes/10+'0'; 
						send_salty[789]=datatime.Minutes%10+'0'; 
						//send[814]=':';
						//Salty Value

						send_salty[842]=tds_array[0];
						send_salty[843]=tds_array[1];
						send_salty[844]=tds_array[2];
						send_salty[845]=tds_array[3];
						send_salty[846]=tds_array[4];
						send_salty[847]=tds_array[5];		
						//year
						send_salty[734]=datatime.Year/10+'0'; 
						send_salty[735]=datatime.Year%10+'0'; 
						//send[802]=':';
						send_salty[776]=datatime.Year/10+'0'; 
						send_salty[777]=datatime.Year%10+'0';
						//send[759]=':';
						//month
						send_salty[737]=datatime.Month/10+'0'; 
						send_salty[738]=datatime.Month%10+'0'; 
						//send[762]=':';						
						send_salty[779]=datatime.Month/10+'0'; 
						send_salty[780]=datatime.Month%10+'0';
						//send[805]=':';
						//day
						send_salty[740]=datatime.Day/10+'0'; 
						send_salty[741]=datatime.Day%10+'0'; 
						//send[765]=':';
						send_salty[782]=datatime.Day/10+'0'; 
						send_salty[783]=datatime.Day%10+'0';
						//send[808]='T';						
					  /* Try to connect to network */
            if ((gsmRes = GSM_GPRS_Attach(&GSM, GSM_APN, GSM_APN_USER, GSM_APN_PASS, 1)) == gsmOK) {
                printf("GPRS Attached\r\n");
                
                /* We are connected, now begin HTTP */
                if ((gsmRes = GSM_HTTP_Begin(&GSM, 1)) == gsmOK) {
                    printf("HTTP Begin OK\r\n");                    
                    {
                      /* POST REQUEST with data */
											//Hours										
                        if ((gsmRes = GSM_HTTP_SetData(&GSM, send_salty, sizeof(send_salty), 1)) == gsmOK) {
                            /* Make actual HTTP request */
													//UART_SEND_STR(FROMMEM("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
													//UART_SEND_STR(FROMMEM("AT+HTTPSSL="));  
													 if ((gsmRes = GSM_HTTP_SetContent(&GSM, GSM_HTTP_CONTENT, 1)) == gsmOK) {
                            if ((gsmRes = GSM_HTTP_Execute(&GSM, GSM_HTTP_URL, GSM_HTTP_Method_POST, GSM_HTTP_SSL_Enable, 1)) == gsmOK) {
                                /* HTTP request executed to server, wait for response data */
                                while (GSM_HTTP_DataAvailable(&GSM, 1)) {
                                    /* Read as many bytes as possible */
                                    /* We are expecting only one read here as we are expecting less data than our array can hold! */
                                    if ((gsmRes = GSM_HTTP_Read(&GSM, receive, sizeof(receive), &br, 1)) == gsmOK) {
                                        //printf("Successfully read %d/%d bytes of data\r\n", br, sizeof(receive));
                                        printf(receive);
                                        /* Check if data are the same */
                                        //if (strncmp((void *)receive, (void *)send, sizeof(send)) == 0) {
                                        //    printf("Received and sent data are the same!\r\n");
                                        //} else {
                                        //    printf("Received is not the same as sent!\r\n");
                                        //}
                                    } else {
                                        //printf("Error trying to read %d bytes of data: %d\r\n", sizeof(receive), gsmRes);
                                    }
                                }
                            } 
														}else {
                                printf("Could not execute request to server: %d\r\n", gsmRes);
                            }
                        } else {
                            printf("Error trying to set POST data: %d\r\n", gsmRes);
                        }
                    }
                    
                    /* End HTTP */
                    GSM_HTTP_End(&GSM, 1);
                } else {
                    printf("Problems trying to begin HTTP: %d\r\n", gsmRes);
                }
								//osDelay(200);
                SEND_STATUS();
                /* Detach from GPRS */
                if ((gsmRes = GSM_GPRS_Detach(&GSM, 1)) == gsmOK) {
                    printf("GPRS Detached\r\n");
                } else {
                    printf("Problems trying to detach GPRS: %d\r\n", gsmRes);
                }
            } else {
                printf("Could not attach to GPRS: %d\r\n", gsmRes);
            }						
				}
				action=1;}
		}
   } 
}
		
/***********************************************/
/**               Library callback            **/
/***********************************************/
int GSM_Callback(GSM_Event_t evt, GSM_EventParams_t* params) {
    switch (evt) {                              /* Check events */
        case gsmEventIdle:
            printf("Stack is IDLE!\r\n");
            break;
        case gsmEventSMSCMTI:                   /* Information about new SMS received */
            printf("SMS received!\r\n");
            break;
        default:
            break;
    }
    
    return 0;
}
/* printf handler */
int fputc(int ch, FILE* fil) {
    TM_USART_Putc(DEBUG_USART, ch);         /* Send over debug USART */
    return ch;                              /* Return OK */
}
void GSM_SMS_CHECK(void){
								if (strcmp(SMS_Entry.Data, "SWESOSHCM STA") == 0) {
									TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);
									send_sms[97]=datatime.Hours/10+'0'; 
									send_sms[98]=datatime.Hours%10+'0'; 
									send_sms[100]=datatime.Minutes/10+'0'; 
									send_sms[101]=datatime.Minutes%10+'0'; 
									gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, send_sms, 1);
                } else if (strcmp(SMS_Entry.Data, "SWESOSHCM ON") == 0) {
                    control_pwr=ON;time_up=0;action=1;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "ON OK", 1);                
                }
								else if (strcmp(SMS_Entry.Data, "SWESOSHCM OFF") == 0) {
                    control_pwr=OFF;action=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OFF OK", 1);                
                }
								else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 5") == 0) {
                    time_loop=5;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 6") == 0) {
                    time_loop=6;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 7") == 0) {
                    time_loop=7;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }
								else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 8") == 0) {
                    time_loop=8;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 9") == 0) {
                    time_loop=9;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 10") == 0) {
                    time_loop=10;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 11") == 0) {
                    time_loop=11;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 12") == 0) {
                    time_loop=12;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 13") == 0) {
                    time_loop=13;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 14") == 0) {
                    time_loop=14;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 15") == 0) {
                    time_loop=15;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 16") == 0) {
                    time_loop=16;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }
								else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 17") == 0) {
                    time_loop=17;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }
								else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 18") == 0) {
                    time_loop=18;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 19") == 0) {
                    time_loop=19;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 20") == 0) {
                    time_loop=20;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 21") == 0) {
                    time_loop=21;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }
								else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 22") == 0) {
                    time_loop=22;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }
								else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 23") == 0) {
                    time_loop=23;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 24") == 0) {
                    time_loop=24;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 25") == 0) {
                    time_loop=25;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 26") == 0) {
                    time_loop=26;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 27") == 0) {
                    time_loop=27;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 28") == 0) {
                    time_loop=28;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 29") == 0) {
                    time_loop=29;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 30") == 0) {
                    time_loop=30;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 31") == 0) {
                    time_loop=31;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 32") == 0) {
                    time_loop=32;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 33") == 0) {
                    time_loop=33;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 34") == 0) {
                    time_loop=34;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 35") == 0) {
                    time_loop=35;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 36") == 0) {
                    time_loop=36;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 37") == 0) {
                    time_loop=37;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 38") == 0) {
                    time_loop=38;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 39") == 0) {
                    time_loop=39;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 40") == 0) {
                    time_loop=40;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 41") == 0) {
                    time_loop=41;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 42") == 0) {
                    time_loop=42;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 43") == 0) {
                    time_loop=43;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 44") == 0) {
                    time_loop=44;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 45") == 0) {
                    time_loop=45;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 46") == 0) {
                    time_loop=46;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 47") == 0) {
                    time_loop=47;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 48") == 0) {
                    time_loop=48;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 49") == 0) {
                    time_loop=49;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 50") == 0) {
                    time_loop=50;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 51") == 0) {
                    time_loop=51;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 52") == 0) {
                    time_loop=52;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 53") == 0) {
                    time_loop=53;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 54") == 0) {
                    time_loop=54;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 55") == 0) {
                    time_loop=55;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 56") == 0) {
                    time_loop=56;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 57") == 0) {
                    time_loop=57;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 58") == 0) {
                    time_loop=58;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }else if (strcmp(SMS_Entry.Data, "SWESOSHCM FRE 59") == 0) {
                    time_loop=59;time_up=0;
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "OK", 1);                
                }
								else if (strcmp(SMS_Entry.Data, "SWESOSHCM CLEAR SMS") == 0) {
            /* Delete all SMS messages */
								if ((gsmRes = GSM_SMS_MassDelete(&GSM, GSM_SMS_MassDelete_All, 1)) == gsmOK) {
										//printf("SMS MASS DELETE OK\r\n");
								} else {
										//printf("Error trying to mass delete: %d\r\n", gsmRes);
								}
								}
								else {
                    gsmRes = GSM_SMS_Send(&GSM, SMS_Entry.Number, "No OK", 1);
                }	
}
void SEND_STATUS(void){
	//Device power
						//TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);
						send[766]=datatime.Hours/10+'0'; 
						send[767]=datatime.Hours%10+'0'; 
						//send[768]=':';
						send[809]=datatime.Hours/10+'0'; 
						send[810]=datatime.Hours%10+'0';
						//send[811]=':';
						//Minutes
						send[769]=datatime.Minutes/10+'0'; 
						send[770]=datatime.Minutes%10+'0'; 
						//send[771]=':';
						send[812]=datatime.Minutes/10+'0'; 
						send[813]=datatime.Minutes%10+'0'; 
						//send[814]=':';
						//Power Value
						if(adcValue<10){
						send[865]=' ';
						send[866]=adcValue%10+'0';
							}
						else{
						send[865]=adcValue/10+'0';
						send[866]=adcValue%10+'0';
							}
						//year
						send[757]=datatime.Year/10+'0'; 
						send[758]=datatime.Year%10+'0'; 
						//send[802]=':';
						send[800]=datatime.Year/10+'0'; 
						send[801]=datatime.Year%10+'0';
						//send[759]=':';
						//month
						send[760]=datatime.Month/10+'0'; 
						send[761]=datatime.Month%10+'0'; 
						//send[762]=':';						
						send[803]=datatime.Month/10+'0'; 
						send[804]=datatime.Month%10+'0';
						//send[805]=':';
						//day
						send[763]=datatime.Day/10+'0'; 
						send[764]=datatime.Day%10+'0'; 
						//send[765]=':';
						send[806]=datatime.Day/10+'0'; 
						send[807]=datatime.Day%10+'0';
						//send[808]='T';	
	//Temperature value
						send_temp[739]=datatime.Hours/10+'0'; 
						send_temp[740]=datatime.Hours%10+'0'; 
						//send[768]=':';
						send_temp[781]=datatime.Hours/10+'0'; 
						send_temp[782]=datatime.Hours%10+'0';
						//send[811]=':';
						//Minutes
						send_temp[742]=datatime.Minutes/10+'0'; 
						send_temp[743]=datatime.Minutes%10+'0'; 
						//send[771]=':';
						send_temp[784]=datatime.Minutes/10+'0'; 
						send_temp[785]=datatime.Minutes%10+'0'; 
						//send[814]=':';
						//temp Value
						if(tempValue<10){
						send_temp[834]='0';
						send_temp[835]=tempValue%10+'0';
						send_temp[836]='.';		
							}
						else{
						send_temp[834]=tempValue/10+'0';
						send_temp[835]=tempValue%10+'0';
						send_temp[836]='.';	
							}
						//temp_dot=((temps[0]-tempValue)*10);
						send_temp[837]=temp_dot+'0';
						//year
						send_temp[730]=datatime.Year/10+'0'; 
						send_temp[731]=datatime.Year%10+'0'; 
						//send[802]=':';
						send_temp[772]=datatime.Year/10+'0'; 
						send_temp[773]=datatime.Year%10+'0';
						//send[759]=':';
						//month
						send_temp[733]=datatime.Month/10+'0'; 
						send_temp[734]=datatime.Month%10+'0'; 
						//send[762]=':';						
						send_temp[775]=datatime.Month/10+'0'; 
						send_temp[776]=datatime.Month%10+'0';
						//send[805]=':';
						//day
						send_temp[736]=datatime.Day/10+'0'; 
						send_temp[737]=datatime.Day%10+'0'; 
						//send[765]=':';
						send_temp[778]=datatime.Day/10+'0'; 
						send_temp[779]=datatime.Day%10+'0';	
//conduct value
						send_conduct[747]=datatime.Hours/10+'0'; 
						send_conduct[748]=datatime.Hours%10+'0'; 
						//send[768]=':';
						send_conduct[789]=datatime.Hours/10+'0'; 
						send_conduct[790]=datatime.Hours%10+'0';
						//send[811]=':';
						//Minutes
						send_conduct[750]=datatime.Minutes/10+'0'; 
						send_conduct[751]=datatime.Minutes%10+'0'; 
						//send[771]=':';
						send_conduct[792]=datatime.Minutes/10+'0'; 
						send_conduct[793]=datatime.Minutes%10+'0'; 
						send_conduct[840]=conduct_array[0];
						send_conduct[841]=conduct_array[1];
						send_conduct[842]=conduct_array[2];
						send_conduct[843]=conduct_array[3];
						send_conduct[844]=conduct_array[4];
						send_conduct[845]=conduct_array[5];
						//send[814]=':';
						//Value [80]->[84]
						//if(tempValue<10){
						//send_temp[834]='0';
						//send_temp[835]=tempValue%10+'0';
						//send_temp[836]='.';		
						//	}
						//else{
						//send_temp[834]=tempValue/10+'0';
						//send_temp[835]=tempValue%10+'0';
						//send_temp[836]='.';	
						//	}
						//temp_dot=((temps[0]-tempValue)*10);
						//send_temp[837]=temp_dot+'0';
						//year
						send_conduct[738]=datatime.Year/10+'0'; 
						send_conduct[739]=datatime.Year%10+'0'; 
						//send[802]=':';
						send_conduct[780]=datatime.Year/10+'0'; 
						send_conduct[781]=datatime.Year%10+'0';
						//send[759]=':';
						//month
						send_conduct[741]=datatime.Month/10+'0'; 
						send_conduct[742]=datatime.Month%10+'0'; 
						//send[762]=':';						
						send_conduct[783]=datatime.Month/10+'0'; 
						send_conduct[784]=datatime.Month%10+'0';
						//send[805]=':';
						//day
						send_conduct[744]=datatime.Day/10+'0'; 
						send_conduct[745]=datatime.Day%10+'0'; 
						//send[765]=':';
						send_conduct[786]=datatime.Day/10+'0'; 
						send_conduct[787]=datatime.Day%10+'0';							
//tds value
						send_tds[756]=datatime.Hours/10+'0'; 
						send_tds[757]=datatime.Hours%10+'0'; 
						//send[768]=':';
						send_tds[798]=datatime.Hours/10+'0'; 
						send_tds[799]=datatime.Hours%10+'0';
						//send[811]=':';
						//Minutes
						send_tds[759]=datatime.Minutes/10+'0'; 
						send_tds[760]=datatime.Minutes%10+'0'; 
						//send[771]=':';
						send_tds[801]=datatime.Minutes/10+'0'; 
						send_tds[802]=datatime.Minutes%10+'0'; 
						send_tds[847]=salty_array[0];
						send_tds[848]=salty_array[1];
						send_tds[849]=salty_array[2];
						send_tds[850]=salty_array[3];
						send_tds[851]=salty_array[4];
						send_tds[852]=salty_array[5];
						//send[814]=':';
						//Value [80]->[84]
						//if(tempValue<10){
						//send_temp[834]='0';
						//send_temp[835]=tempValue%10+'0';
						//send_temp[836]='.';		
						//	}
						//else{
						//send_temp[834]=tempValue/10+'0';
						//send_temp[835]=tempValue%10+'0';
						//send_temp[836]='.';	
						//	}
						//temp_dot=((temps[0]-tempValue)*10);
						//send_temp[837]=temp_dot+'0';
						//year
						send_tds[747]=datatime.Year/10+'0'; 
						send_tds[748]=datatime.Year%10+'0'; 
						//send[802]=':';
						send_tds[789]=datatime.Year/10+'0'; 
						send_tds[790]=datatime.Year%10+'0';
						//send[759]=':';
						//month
						send_tds[750]=datatime.Month/10+'0'; 
						send_tds[751]=datatime.Month%10+'0'; 
						//send[762]=':';						
						send_tds[792]=datatime.Month/10+'0'; 
						send_tds[793]=datatime.Month%10+'0';
						//send[805]=':';
						//day
						send_tds[753]=datatime.Day/10+'0'; 
						send_tds[754]=datatime.Day%10+'0'; 
						//send[765]=':';
						send_tds[795]=datatime.Day/10+'0'; 
						send_tds[796]=datatime.Day%10+'0';	
//wsg value
						send_wsg[756]=datatime.Hours/10+'0'; 
						send_wsg[757]=datatime.Hours%10+'0'; 
						//send[768]=':';
						send_wsg[798]=datatime.Hours/10+'0'; 
						send_wsg[799]=datatime.Hours%10+'0';
						//send[811]=':';
						//Minutes
						send_wsg[759]=datatime.Minutes/10+'0'; 
						send_wsg[760]=datatime.Minutes%10+'0'; 
						//send[771]=':';
						send_wsg[801]=datatime.Minutes/10+'0'; 
						send_wsg[802]=datatime.Minutes%10+'0';
						send_wsg[860]=wsg_array[0];
						send_wsg[861]=wsg_array[1];
						send_wsg[862]=wsg_array[2];
						send_wsg[863]=wsg_array[3];
						send_wsg[864]=wsg_array[4];
						send_wsg[865]=wsg_array[5];
						//send[814]=':';
						//Value [80]->[84]
						//if(tempValue<10){
						//send_temp[834]='0';
						//send_temp[835]=tempValue%10+'0';
						//send_temp[836]='.';		
						//	}
						//else{
						//send_temp[834]=tempValue/10+'0';
						//send_temp[835]=tempValue%10+'0';
						//send_temp[836]='.';	
						//	}
						//temp_dot=((temps[0]-tempValue)*10);
						//send_temp[837]=temp_dot+'0';
						//year
						send_wsg[747]=datatime.Year/10+'0'; 
						send_wsg[748]=datatime.Year%10+'0'; 
						//send[802]=':';
						send_wsg[789]=datatime.Year/10+'0'; 
						send_wsg[790]=datatime.Year%10+'0';
						//send[759]=':';
						//month
						send_wsg[750]=datatime.Month/10+'0'; 
						send_wsg[751]=datatime.Month%10+'0'; 
						//send[762]=':';						
						send_wsg[792]=datatime.Month/10+'0'; 
						send_wsg[793]=datatime.Month%10+'0';
						//send[805]=':';
						//day
						send_wsg[753]=datatime.Day/10+'0'; 
						send_wsg[754]=datatime.Day%10+'0'; 
						//send[765]=':';
						send_wsg[795]=datatime.Day/10+'0'; 
						send_wsg[796]=datatime.Day%10+'0';		
/* Try to connect to network */
             {                
                /* We are connected, now begin HTTP */
                if ((gsmRes = GSM_HTTP_Begin(&GSM, 1)) == gsmOK) {
                    //printf("HTTP Begin OK\r\n");                    
                    {
                      /* POST REQUEST with data */
											//Hours										
                        if ((gsmRes = GSM_HTTP_SetData(&GSM, send, sizeof(send), 1)) == gsmOK) {
                            /* Make actual HTTP request */
													//UART_SEND_STR(FROMMEM("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
													//UART_SEND_STR(FROMMEM("AT+HTTPSSL="));  
													 if ((gsmRes = GSM_HTTP_SetContent(&GSM, GSM_HTTP_CONTENT, 1)) == gsmOK) {
                            if ((gsmRes = GSM_HTTP_Execute(&GSM, GSM_HTTP_URL, GSM_HTTP_Method_POST, GSM_HTTP_SSL_Enable, 1)) == gsmOK) {
                                /* HTTP request executed to server, wait for response data */
                                while (GSM_HTTP_DataAvailable(&GSM, 1)) {
                                    /* Read as many bytes as possible */
                                    /* We are expecting only one read here as we are expecting less data than our array can hold! */
                                    if ((gsmRes = GSM_HTTP_Read(&GSM, receive, sizeof(receive), &br, 1)) == gsmOK) {
                                        //printf("Successfully read %d/%d bytes of data\r\n", br, sizeof(receive));
                                        printf(receive);
                                        /* Check if data are the same */
                                        //if (strncmp((void *)receive, (void *)send, sizeof(send)) == 0) {
                                        //    printf("Received and sent data are the same!\r\n");
                                        //} else {
                                        //    printf("Received is not the same as sent!\r\n");
                                        //}
                                    } else {
                                        //printf("Error trying to read %d bytes of data: %d\r\n", sizeof(receive), gsmRes);
                                    }
                                }
                            } 
														}else {
                                //printf("Could not execute request to server: %d\r\n", gsmRes);
                            }
                        } else {
                            //printf("Error trying to set POST data: %d\r\n", gsmRes);
                        }
                    }
                    
                    /* End HTTP */
                    GSM_HTTP_End(&GSM, 1);
                } else {
                    //printf("Problems trying to begin HTTP: %d\r\n", gsmRes);
                }
                //osDelay(200);
                /* We are connected, now begin HTTP */
                if ((gsmRes = GSM_HTTP_Begin(&GSM, 1)) == gsmOK) {
                    //printf("HTTP Begin OK\r\n");                    
                    {
                      /* POST REQUEST with data */
											//Hours										
                        if ((gsmRes = GSM_HTTP_SetData(&GSM, send_temp, sizeof(send_temp), 1)) == gsmOK) {
                            /* Make actual HTTP request */
													//UART_SEND_STR(FROMMEM("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
													//UART_SEND_STR(FROMMEM("AT+HTTPSSL="));  
													 if ((gsmRes = GSM_HTTP_SetContent(&GSM, GSM_HTTP_CONTENT, 1)) == gsmOK) {
                            if ((gsmRes = GSM_HTTP_Execute(&GSM, GSM_HTTP_URL, GSM_HTTP_Method_POST, GSM_HTTP_SSL_Enable, 1)) == gsmOK) {
                                /* HTTP request executed to server, wait for response data */
                                while (GSM_HTTP_DataAvailable(&GSM, 1)) {
                                    /* Read as many bytes as possible */
                                    /* We are expecting only one read here as we are expecting less data than our array can hold! */
                                    if ((gsmRes = GSM_HTTP_Read(&GSM, receive, sizeof(receive), &br, 1)) == gsmOK) {
                                        //printf("Successfully read %d/%d bytes of data\r\n", br, sizeof(receive));
                                        printf(receive);
                                        /* Check if data are the same */
                                        //if (strncmp((void *)receive, (void *)send, sizeof(send)) == 0) {
                                        //    printf("Received and sent data are the same!\r\n");
                                        //} else {
                                        //    printf("Received is not the same as sent!\r\n");
                                        //}
                                    } else {
                                        //printf("Error trying to read %d bytes of data: %d\r\n", sizeof(receive), gsmRes);
                                    }
                                }
                            } 
														}else {
                                //printf("Could not execute request to server: %d\r\n", gsmRes);
                            }
                        } else {
                            //printf("Error trying to set POST data: %d\r\n", gsmRes);
                        }
                    }
                    
                    /* End HTTP */
                    GSM_HTTP_End(&GSM, 1);
                } else {
                    //printf("Problems trying to begin HTTP: %d\r\n", gsmRes);
                }  

//osDelay(200);
                /* We are connected, now begin HTTP */
                if ((gsmRes = GSM_HTTP_Begin(&GSM, 1)) == gsmOK) {
                    //printf("HTTP Begin OK\r\n");                    
                    {
                      /* POST REQUEST with data */
											//Hours										
                        if ((gsmRes = GSM_HTTP_SetData(&GSM, send_tds, sizeof(send_tds), 1)) == gsmOK) {
                            /* Make actual HTTP request */
													//UART_SEND_STR(FROMMEM("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
													//UART_SEND_STR(FROMMEM("AT+HTTPSSL="));  
													 if ((gsmRes = GSM_HTTP_SetContent(&GSM, GSM_HTTP_CONTENT, 1)) == gsmOK) {
                            if ((gsmRes = GSM_HTTP_Execute(&GSM, GSM_HTTP_URL, GSM_HTTP_Method_POST, GSM_HTTP_SSL_Enable, 1)) == gsmOK) {
                                /* HTTP request executed to server, wait for response data */
                                while (GSM_HTTP_DataAvailable(&GSM, 1)) {
                                    /* Read as many bytes as possible */
                                    /* We are expecting only one read here as we are expecting less data than our array can hold! */
                                    if ((gsmRes = GSM_HTTP_Read(&GSM, receive, sizeof(receive), &br, 1)) == gsmOK) {
                                        //printf("Successfully read %d/%d bytes of data\r\n", br, sizeof(receive));
                                        printf(receive);
                                        /* Check if data are the same */
                                        //if (strncmp((void *)receive, (void *)send, sizeof(send)) == 0) {
                                        //    printf("Received and sent data are the same!\r\n");
                                        //} else {
                                        //    printf("Received is not the same as sent!\r\n");
                                        //}
                                    } else {
                                        //printf("Error trying to read %d bytes of data: %d\r\n", sizeof(receive), gsmRes);
                                    }
                                }
                            } 
														}else {
                                //printf("Could not execute request to server: %d\r\n", gsmRes);
                            }
                        } else {
                            //printf("Error trying to set POST data: %d\r\n", gsmRes);
                        }
                    }
                    
                    /* End HTTP */
                    GSM_HTTP_End(&GSM, 1);
                } else {
                    //printf("Problems trying to begin HTTP: %d\r\n", gsmRes);
                }              
//osDelay(200);
                /* We are connected, now begin HTTP */
                if ((gsmRes = GSM_HTTP_Begin(&GSM, 1)) == gsmOK) {
                    //printf("HTTP Begin OK\r\n");                    
                    {
                      /* POST REQUEST with data */
											//Hours										
                        if ((gsmRes = GSM_HTTP_SetData(&GSM, send_wsg, sizeof(send_wsg), 1)) == gsmOK) {
                            /* Make actual HTTP request */
													//UART_SEND_STR(FROMMEM("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
													//UART_SEND_STR(FROMMEM("AT+HTTPSSL="));  
													 if ((gsmRes = GSM_HTTP_SetContent(&GSM, GSM_HTTP_CONTENT, 1)) == gsmOK) {
                            if ((gsmRes = GSM_HTTP_Execute(&GSM, GSM_HTTP_URL, GSM_HTTP_Method_POST, GSM_HTTP_SSL_Enable, 1)) == gsmOK) {
                                /* HTTP request executed to server, wait for response data */
                                while (GSM_HTTP_DataAvailable(&GSM, 1)) {
                                    /* Read as many bytes as possible */
                                    /* We are expecting only one read here as we are expecting less data than our array can hold! */
                                    if ((gsmRes = GSM_HTTP_Read(&GSM, receive, sizeof(receive), &br, 1)) == gsmOK) {
                                        //printf("Successfully read %d/%d bytes of data\r\n", br, sizeof(receive));
                                        printf(receive);
                                        /* Check if data are the same */
                                        //if (strncmp((void *)receive, (void *)send, sizeof(send)) == 0) {
                                        //    printf("Received and sent data are the same!\r\n");
                                        //} else {
                                        //    printf("Received is not the same as sent!\r\n");
                                        //}
                                    } else {
                                        //printf("Error trying to read %d bytes of data: %d\r\n", sizeof(receive), gsmRes);
                                    }
                                }
                            } 
														}else {
                                //printf("Could not execute request to server: %d\r\n", gsmRes);
                            }
                        } else {
                            //printf("Error trying to set POST data: %d\r\n", gsmRes);
                        }
                    }
                    
                    /* End HTTP */
                    GSM_HTTP_End(&GSM, 1);
                } else {
                    //printf("Problems trying to begin HTTP: %d\r\n", gsmRes);
                }              								
         //osDelay(200);
                /* We are connected, now begin HTTP */
                if ((gsmRes = GSM_HTTP_Begin(&GSM, 1)) == gsmOK) {
                    //printf("HTTP Begin OK\r\n");                    
                    {
                      /* POST REQUEST with data */
											//Hours										
                        if ((gsmRes = GSM_HTTP_SetData(&GSM, send_conduct, sizeof(send_conduct), 1)) == gsmOK) {
                            /* Make actual HTTP request */
													//UART_SEND_STR(FROMMEM("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
													//UART_SEND_STR(FROMMEM("AT+HTTPSSL="));  
													 if ((gsmRes = GSM_HTTP_SetContent(&GSM, GSM_HTTP_CONTENT, 1)) == gsmOK) {
                            if ((gsmRes = GSM_HTTP_Execute(&GSM, GSM_HTTP_URL, GSM_HTTP_Method_POST, GSM_HTTP_SSL_Enable, 1)) == gsmOK) {
                                /* HTTP request executed to server, wait for response data */
                                while (GSM_HTTP_DataAvailable(&GSM, 1)) {
                                    /* Read as many bytes as possible */
                                    /* We are expecting only one read here as we are expecting less data than our array can hold! */
                                    if ((gsmRes = GSM_HTTP_Read(&GSM, receive, sizeof(receive), &br, 1)) == gsmOK) {
                                        //printf("Successfully read %d/%d bytes of data\r\n", br, sizeof(receive));
                                        printf(receive);
                                        /* Check if data are the same */
                                        //if (strncmp((void *)receive, (void *)send, sizeof(send)) == 0) {
                                        //    printf("Received and sent data are the same!\r\n");
                                        //} else {
                                        //    printf("Received is not the same as sent!\r\n");
                                        //}
                                    } else {
                                        //printf("Error trying to read %d bytes of data: %d\r\n", sizeof(receive), gsmRes);
                                    }
                                }
                            } 
														}else {
                                //printf("Could not execute request to server: %d\r\n", gsmRes);
                            }
                        } else {
                            //printf("Error trying to set POST data: %d\r\n", gsmRes);
                        }
                    }
                    
                    /* End HTTP */
                    GSM_HTTP_End(&GSM, 1);
                } else {
                    //printf("Problems trying to begin HTTP: %d\r\n", gsmRes);
                }                 
						 }					
				}	
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	printf("Error");
  /* USER CODE END Error_Handler_Debug */
}
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
//end of file
