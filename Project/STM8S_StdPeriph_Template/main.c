/**
******************************************************************************
* @file    Project/main.c 
* @author  MCD Application Team
* @version V2.2.0
* @date    30-September-2014
* @brief   Main program body
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "delay.h"
#include "TM1638.h" 
/* Private defines -----------------------------------------------------------*/

#define BUTTON_FILTER_TIME         5  
#define BUTTON_LONG_TIME           50                /* ����1�룬��Ϊ�����¼� */
#define Key_Num  8




u8 KeyDownCode[]= {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
//�������µļ�ֵ
u8 KeyUpCode[]={0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f};
//��������ļ�ֵ
u8 KeyLongCode[]={0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};
//���������ļ�ֵ
unsigned char num[8]={0,0,1,1,2,2,3,3};		//�����������ʾ��ֵ

typedef struct  
{  
  /* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */  
  
  
  unsigned char  which_key;
  //unsigned char  (*IsKeyDownFunc)(unsigned char which_key); /* �������µ��жϺ���,1��ʾ���� */    
  unsigned char  Count;                        /* �˲��������� */  
  unsigned char  FilterTime;                /* �˲�ʱ��(���255,��ʾ2550ms) */  
  unsigned short LongCount;                /* ���������� */  
  unsigned short LongTime;                /* �������³���ʱ��, 0��ʾ����ⳤ�� */  
  unsigned char  State;                        /* ������ǰ״̬�����»��ǵ��� */  
  unsigned char  KeyCodeUp;                /* ��������ļ�ֵ����, 0��ʾ����ⰴ������ */  
  unsigned char  KeyCodeDown;        /* �������µļ�ֵ����, 0��ʾ����ⰴ������ */  
  unsigned char  KeyCodeLong;        /* ���������ļ�ֵ����, 0��ʾ����ⳤ�� */  
  unsigned char  RepeatSpeed;        /* ������������ */  
  unsigned char  RepeatCount;        /* �������������� */  
}BUTTON_T; 

BUTTON_T Key[16];




/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void UART_Config(void);

void Pannelkey_Polling(void);
void Button_Detect(BUTTON_T *_pBtn);
unsigned char IsKey_Down(unsigned char which_key);

void Key_Init(void);
void IsKeyDownDetect(void);

void key_display(u8 i);
//
//u8 KeyValue,COLUMN_VALUE1,COLUMN_VALUE2,COLUMN_VALUE3,COLUMN_VALUE4,column_value;
//u8 ROW_VALUE1,ROW_VALUE2,ROW_VALUE3,ROW_VALUE4,row_value;

/* �����˲�ʱ��50ms, ��λ10ms 
*ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼� 
*/  


/* 
ÿ��������Ӧ1��ȫ�ֵĽṹ������� 
���Ա������ʵ���˲��Ͷ��ְ���״̬������� 
*/  

void main(void)
{
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  CLK_HSICmd(ENABLE);
  
  delay_init(16);
  Key_Init();
  UART_Config();
  
  init_TM1638();	                           //��ʼ��TM1638
  for(u8 i=0;i<8;i++)
    Write_DATA(i<<1,tab[num[i]]);	               //��ʼ���Ĵ���
  
  while(1){
    
    Pannelkey_Polling();
  }
  
  /* Infinite loop */
  
}

/* 
********************************************************************************************************* 
*        �� �� ��: bsp_DetectButton 
*        ����˵��: ���һ��������������״̬�����뱻�����Եĵ��á� 
*        ��    �Σ������ṹ����ָ�� 
*        �� �� ֵ: �� 
********************************************************************************************************* 
*/  
void Button_Detect(BUTTON_T *_pBtn)  
{  
  if (IsKeyDownRec[_pBtn->which_key])  
  {  
    IsKeyDownRec[_pBtn->which_key]=0;
    if (_pBtn->Count < _pBtn->FilterTime)  
    {  
      _pBtn->Count = _pBtn->FilterTime;  
    }  
    else if(_pBtn->Count < 2 * _pBtn->FilterTime)  
    {  
      _pBtn->Count++;  
    }  
    else  
    {  
      if (_pBtn->State == 0)  
      {  
        _pBtn->State = 1;  
        
        /* ���Ͱ�ť���µ���Ϣ */  
        if (_pBtn->KeyCodeDown > 0)  
        {  
          /* ��ֵ���밴��FIFO */ 
          key_display(_pBtn->which_key);
//          UART1_SendData8(_pBtn->KeyCodeDown);// ��¼�������±�־���ȴ��ͷ�  
//          while( UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET );
        }  
      }  
      
      if (_pBtn->LongTime > 0)  
      {  
        if (_pBtn->LongCount < _pBtn->LongTime)  
        {  
          /* ���Ͱ�ť�������µ���Ϣ */  
          if (++_pBtn->LongCount == _pBtn->LongTime)  
          {  
            /* ��ֵ���밴��FIFO */  
//            UART1_SendData8(_pBtn->KeyCodeLong);          
//            while( UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET );
          }  
        }  
        else  
        {  
          if (_pBtn->RepeatSpeed > 0)  
          {  
            if (++_pBtn->RepeatCount >= _pBtn->RepeatSpeed)  
            {  
              _pBtn->RepeatCount = 0;  
              /* ��������ÿ��10ms����1������ */  
//              UART1_SendData8(_pBtn->KeyCodeDown);          
//              while( UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET );
            }  
          }  
        }  
      }  
    }  
  }  
  else  
  {  
    if(_pBtn->Count > _pBtn->FilterTime)  
    {  
      _pBtn->Count = _pBtn->FilterTime;  
    }  
    else if(_pBtn->Count != 0)  
    {  
      _pBtn->Count--;  
    }  
    else  
    {  
      if (_pBtn->State == 1)  
      {  
        _pBtn->State = 0;  
        
        /* ���Ͱ�ť�������Ϣ */  
        if (_pBtn->KeyCodeUp > 0) /*�����ͷ�*/  
        {  
          /* ��ֵ���밴��FIFO */  
//          UART1_SendData8(_pBtn->KeyCodeUp);          
//          while( UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET );
        }  
      }  
    }  
    
    _pBtn->LongCount = 0;  
    _pBtn->RepeatCount = 0;  
  }  
}  
//����˵��: ������а�����10MS ����һ��  
void Pannelkey_Polling(void)  
{ 
  Read_key();
  for(u8 i=0;i<8;i++)
  {
    Button_Detect(&Key[i]);                /*Key_1 �� */  
    
  }
  delay_ms(10);
} 
void key_display(u8 i)
{
  if(i<8)
  {
    num[i]++;
    if(num[i]>15)
      num[i]=0;
    Write_DATA(i*2,tab[num[i]]);
    Write_allLED(1<<i);
  }
  
}
static void UART_Config(void)
{
  /* Deinitializes the UART1 and UART3 peripheral */
  UART1_DeInit();
  // UART3_DeInit();
  /* UART1 and UART3 configuration -------------------------------------------------*/
  /* UART1 and UART3 configured as follow:
  - BaudRate = 9600 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Receive and transmit enabled
  - UART1 Clock disabled
  */
  /* Configure the UART1 */
  UART1_Init((u32)9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  
  /* Enable UART1 Transmit interrupt*/
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
 
  enableInterrupts(); 
  UART1_Cmd(ENABLE);
  UART1_SendData8(0x00);
}


void Key_Init()
{
  for(u8 i=0;i<8;i++){
    
    Key[i].which_key=i;
    /* ��ʼ��USER����������֧�ְ��¡����𡢳��� */  
    //Key[i].IsKeyDownFunc = IsKey_Down;       /* �жϰ������µĺ��� */  
    Key[i].FilterTime = BUTTON_FILTER_TIME;      /* �����˲�ʱ�� */  
    Key[i].LongTime = BUTTON_LONG_TIME;          /* ����ʱ�� */  
    Key[i].Count = Key[i].FilterTime / 2;    /* ����������Ϊ�˲�ʱ���һ�� */  
    Key[i].State = 0;                            /* ����ȱʡ״̬��0Ϊδ���� */  
    Key[i].KeyCodeDown = KeyDownCode[i];         /* �������µļ�ֵ���� */  
    Key[i].KeyCodeUp =KeyUpCode[i];              /* ��������ļ�ֵ���� */  
    Key[i].KeyCodeLong = KeyLongCode[i];         /* �������������µļ�ֵ���� */  
    Key[i].RepeatSpeed = 0;                      /* �����������ٶȣ�0��ʾ��֧������ */  
    Key[i].RepeatCount = 0;                      /* ���������� */    

  }
}


//unsigned char IsKeyDown(unsigned char which_key)
//{
//  //if()
//  return 0;
//}











#ifdef USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval : None
*/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
