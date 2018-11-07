#include "wkup.h"
#include "led.h"
#include "delay.h"



void Sys_Enter_STOP(void)
{			 
	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_WakeUpPinCmd(ENABLE);  
	//PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
}

u8 Check_WKUP(void) 
{
	u8 t=0;
	LED0=0; 
	while(1)
	{
		if(WKUP_KD)
		{
			t++;			
			delay_ms(30);
			if(t>=100)
			{
				LED0=0;
				return 1;
			}
		}else 
		{ 
			LED0=1;
			return 0; 
		}
	}
} 



void EXTI0_IRQHandler(void)
{ 		    		    				     		    
	EXTI_ClearITPendingBit(EXTI_Line0); // ??LINE10???????		  
	if(Check_WKUP())
	{		  
		Sys_Enter_Standby();  
	}

	  
  
	
} 
//PA0 WKUP
void WKUP_Init(void)
{	
  GPIO_InitTypeDef  GPIO_InitStructure;  		  
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;	 
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
    //????????
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);	

  EXTI_InitStructure.EXTI_Line = EXTI_Line0;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
 	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 

	//if(Check_WKUP()==0) Sys_Standby();    //????,??????  
	
}
















