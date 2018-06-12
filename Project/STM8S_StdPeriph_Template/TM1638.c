#include "stm8s.h"
#include "delay.h"
#include "Pin_Allocation.h"
#include "TM1638.h"  
#include "G_Var.h"

unsigned char IsKeyDownRec[16]={0};
unsigned char tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
                           0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};

void TM1638_Write(unsigned char	DATA)			//д���ݺ���
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		GPIO_WriteLow(TMCLK);
		if(DATA&0X01)
			GPIO_WriteHigh(TMDIO);
		else
			GPIO_WriteLow(TMDIO);
		DATA>>=1;
		GPIO_WriteHigh(TMCLK);
	}
}
unsigned char TM1638_Read(void)					//�����ݺ���
{
  unsigned char i;
  unsigned char temp=0;
  //GPIO_WriteHigh(TMDIO);	//����Ϊ����
  
  for(i=0;i<8;i++)
  {
    temp>>=1;
    GPIO_WriteLow(TMCLK);
    BitStatus bs;
    bs = GPIO_ReadInputPin(TMDIO);
    if(bs)
      temp|=0x80;
    GPIO_WriteHigh(TMCLK);
    
  }
  
  return temp;
}
void Write_COM(unsigned char cmd)		//����������
{
	GPIO_WriteLow(TMSTB);
	TM1638_Write(cmd);
	GPIO_WriteHigh(TMSTB);
}
void Read_key(void)
{
  unsigned char c[4],i,key_value=0;

  
  GPIO_WriteLow(TMSTB);
  TM1638_Write(0x42);		           //����ɨ���� ����
  GPIO_Init(TMDIO, GPIO_MODE_IN_PU_NO_IT);//TMDIO����Ϊ����
  for(i=0;i<4;i++){		
    c[i]=TM1638_Read();
    
  }
  GPIO_Init(TMDIO, GPIO_MODE_OUT_PP_HIGH_FAST);//TMDIO����Ϊ���
  GPIO_WriteHigh(TMSTB);					           //4���ֽ����ݺϳ�һ���ֽ�
  for(i=0;i<4;i++)
    key_value|=c[i]<<i;
  for(i=0;i<8;i++){
    if((0x01<<i)&(key_value)){
      IsKeyDownRec[i]=1;
    }
  }
  
}
void Write_DATA(unsigned char add,unsigned char DATA)		//ָ����ַд������
{
	Write_COM(0x44);
	GPIO_WriteLow(TMSTB);
	TM1638_Write(0xc0|add);
	TM1638_Write(DATA);
	GPIO_WriteHigh(TMSTB);
}

void Write_oneLED(unsigned char num,unsigned char flag)	//��������һ��LED������numΪ��Ҫ���Ƶ�led��ţ�flagΪ0ʱϨ�𣬲�Ϊ0ʱ����
{
	if(flag)
		Write_DATA(2*num+1,1);
	else
		Write_DATA(2*num+1,0);
}  	
void Write_allLED(unsigned char LED_flag)					//����ȫ��LED������LED_flag��ʾ����LED״̬
{
	unsigned char i;
	for(i=0;i<8;i++)
		{
			if(LED_flag&(1<<i))
				Write_DATA(2*i+1,3);
				//Write_DATA(2*i+1,1);
			else
				Write_DATA(2*i+1,0);
		}
}

//TM1638��ʼ������
void init_TM1638(void)
{
	unsigned char i;
        GPIO_Init(TMSTB, GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(TMCLK, GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(TMDIO, GPIO_MODE_OUT_PP_HIGH_FAST);
        Write_COM(0x03);
	Write_COM(0x8f);       //���� (0x88-0x8f)8�����ȿɵ�  (С�ڵ���0x87�����Ϩ��)
	Write_COM(0x40);       //���õ�ַ�Զ���1
	GPIO_WriteLow(TMSTB);		           //
	TM1638_Write(0xc0);    //������ʼ��ַ

	for(i=0;i<16;i++)	   //����16���ֽڵ�����
		TM1638_Write(0x00);
	GPIO_WriteHigh(TMSTB);
}