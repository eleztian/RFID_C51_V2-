#ifndef __EEPROM_H__
#define __EEPROM_H__
#include <reg52.h>
#include <intrins.h>

#define u8 unsigned char    
#define u16 unsigned int 

//�Զ���ͷ�ļ�
sfr ISP_WDT=0xe1;
sfr ISP_DATA=0xe2;
sfr ISP_ADDRH=0xe3;
sfr ISP_ADDRL=0xe4;
sfr ISP_CMD=0xe5;
sfr ISP_TRIG=0xe6;
sfr ISP_CONTR=0xe7;

#define read_cmd     0x01                   //�ֽڶ���������       
#define wirte_cmd    0x02                   //�ֽڱ����������       
#define erase_cmd    0x03                   //����������������   

#define enable_waitTime 0x83 //ϵͳ����ʱ��<12MHz ʱ����IAP_CONTR �Ĵ������ô�ֵ 

void eeprom_read_data(u16 beginAddr, u8* pBuf, u16 dataSize);
void eeprom_write_data(u16 beginAddr,u8* pDat,u16 dataSize);
void eeprom_sector_erase(u16 sectorAddr);
void eeprom_disable(void);

#endif 