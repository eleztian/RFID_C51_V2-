#ifndef __EEPROM_H__
#define __EEPROM_H__
#include <reg52.h>
#include <intrins.h>

#define u8 unsigned char    
#define u16 unsigned int 

//自定义头文件
sfr ISP_WDT=0xe1;
sfr ISP_DATA=0xe2;
sfr ISP_ADDRH=0xe3;
sfr ISP_ADDRL=0xe4;
sfr ISP_CMD=0xe5;
sfr ISP_TRIG=0xe6;
sfr ISP_CONTR=0xe7;

#define read_cmd     0x01                   //字节读数据命令       
#define wirte_cmd    0x02                   //字节编程数据命令       
#define erase_cmd    0x03                   //扇区擦除数据命令   

#define enable_waitTime 0x83 //系统工作时钟<12MHz 时，对IAP_CONTR 寄存器设置此值 

void eeprom_read_data(u16 beginAddr, u8* pBuf, u16 dataSize);
void eeprom_write_data(u16 beginAddr,u8* pDat,u16 dataSize);
void eeprom_sector_erase(u16 sectorAddr);
void eeprom_disable(void);

#endif 