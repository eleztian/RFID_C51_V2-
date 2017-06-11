
#include "eeprom.h"


void ISP_IAP_disable(void)                  //�ر�ISP_IAP  
{     
    EA=1;                                   //�ָ��ж�  
    ISP_CONTR = 0x00;       
    ISP_CMD = 0x00;        
    ISP_TRIG = 0x00;                                               
}  
  
void ISP_IAP_trigger()                       //����   
{     
    EA=0;                                    //�����2��ָ���������ִ��,�ʹ��ж�             
    ISP_TRIG = 0x46;                         //�ʹ���������0x46       
    ISP_TRIG = 0xB9;                         //�ʹ���������0xB9       
}  
         
  
void ISP_IAP_readData(u16 beginAddr, u8* pBuf, u16 dataSize) //��ȡ����  
{  
    ISP_DATA=0;                               //����,����Ҳ����  
    ISP_CMD = read_cmd;                       //ָ��:��ȡ  
    ISP_CONTR = enable_waitTime;              //����ISP_IAP�����͵ȴ�ʱ��   
    while(dataSize--)                         //ѭ����ȡ  
    {  
        ISP_ADDRH = (u8)(beginAddr >> 8);     //�͵�ַ���ֽ�        
        ISP_ADDRL = (u8)(beginAddr & 0x00ff); //�͵�ַ���ֽ�  
        ISP_IAP_trigger();                  //����  
        beginAddr++;                        //��ַ++  
        *pBuf++ = ISP_DATA;                 //�����ݱ��浽���ջ�����  
    }  
    ISP_IAP_disable();                      //�ر�ISP_IAP����  
}         
     
void ISP_IAP_writeData(u16 beginAddr,u8* pDat,u16 dataSize) //д����      
{             
    ISP_CONTR = enable_waitTime;                //����ISP_IAP�����͵ȴ�ʱ��         
    ISP_CMD = wirte_cmd;                        //���ֽڱ��������  
    while(dataSize--)  
    {  
        ISP_ADDRH = (u8)(beginAddr >> 8);       //�͵�ַ���ֽ�         
        ISP_ADDRL = (u8)(beginAddr & 0x00ff);   //�͵�ַ���ֽ�             
        ISP_DATA = *pDat++;                     //������         
        beginAddr++;   
        ISP_IAP_trigger();                      //����  
    }            
    ISP_IAP_disable();   //�ر�                                        
}      
          
void ISP_IAP_sectorErase(u16 sectorAddr)        //��������  
{             
    ISP_CONTR = enable_waitTime;                //����ISP_IAP;���͵ȴ�ʱ��         
    ISP_CMD = erase_cmd;                        //����������������           
    ISP_ADDRH = (u8)(sectorAddr >> 8);            //�͵�ַ���ֽ�         
    ISP_ADDRL = (u8)(sectorAddr & 0X00FF);      //�͵�ַ���ֽ�             
    ISP_IAP_trigger();                          //����  
    ISP_IAP_disable();                          //�ر�ISP_IAP����           
}  