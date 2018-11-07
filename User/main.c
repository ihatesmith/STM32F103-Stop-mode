#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "stm32f10x.h"

void EXTI4_IRQHandler(void);
void NVIC_Configuration(void);
void EXTI_Configuration(void);
void Uart_Print(unsigned char *str,unsigned char str_size);

 int main(void){
 unsigned char str1[30]=" ......Entry Stop Mode......\r\n";
 unsigned char str2[30]=" !!!Exit Stop Mode!!!!\r\n";

	 
 delay_init();	    	 //延时函数初始化	  
 uart_init(115200);	 	//串口初始化为115200
 KEY_Init();
 LED_Init();
 EXTI_Configuration();
 NVIC_Configuration();
	 /*
	LCD_Init();	 //LCD3?ê??ˉ
	POINT_COLOR=RED;
	 
	LCD_ShowString(30,50,200,16,16,"Warship STM32");	
	LCD_ShowString(30,70,200,16,16,"STOP TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/11/05");
*/
   while(1){

      /***********Entry Stop Mode...************/
      GPIO_ResetBits(GPIOB,GPIO_Pin_7);
      delay_ms(1000);
      Uart_Print(str1,sizeof(str1));
       
       
      // EXTI Line Pending bits and RTC alarm flag must be reset before entry Stop mode
      // ref AN2629 P.12 Table 4.
      EXTI_ClearITPendingBit(EXTI_Line14);
      RTC_ClearFlag(RTC_FLAG_ALR);
      // Configure all GPIO as analog to reduce current consumption on non used IOs 
      GPIO_ReducConsum();
      PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);//进入停止模式
      
      /***********Exit Stop Mode...************/

      GPIO_SetBits(GPIOB,GPIO_Pin_7);
      delay_ms(1000);
      Uart_Print(str2,sizeof(str2));

	 }
	 
 }

void RCC_Configuration(void)
{   
        SystemInit();
}


void NVIC_Configuration(void){
	NVIC_InitTypeDef NVIC_InitStructure;


	/* Configure one bit for preemption priority */
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	//KEY0
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI_Configuration(void)
{
   EXTI_InitTypeDef EXTI_InitStructure;

   GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource14);

   EXTI_InitStructure.EXTI_Line = EXTI_Line14;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);

}

//KEY0按下，退出停止模式
void EXTI15_10_IRQHandler(void)
{
   if(EXTI->PR & (MASK_BIT14))
   {
      if(EXTI_GetITStatus(EXTI_Line14) != RESET) 
      {
         EXTI_ClearITPendingBit(EXTI_Line14);
         

         RCC_Configuration();//时钟初始化
         
         KEY_Init();
         LED_Init();
         uart_init(115200);

      
      }
   }
}

void Uart_Print(unsigned char *str, unsigned char str_size){
   int i=0;
    GPIO_SetBits(GPIOA, GPIO_Pin_8);       // RS485 module @ DE mode
      for(i =0;i<str_size;i++)
      {
         USART_SendData(USART1, str[i]);
         while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET); 
      }
      GPIO_ResetBits(GPIOA, GPIO_Pin_8);       // RS485 module @ DE mode
}
