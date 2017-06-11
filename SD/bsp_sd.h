
#ifndef __BSP_SD_H_
#define __BSP_SD_H_

//=============================================================
//定义SD卡需要的4根信号线
sbit SD_CLK = P3^7;
sbit SD_DI  = P3^5;
sbit SD_DO  = P3^6;
sbit SD_CS  = P3^4;
//===========================================================
//读写数据保存对象
extern unsigned char xdata DATA[512];
//===========================================================
//函数声明
void delayms(unsigned int count);
void SdWrite(unsigned char n);
unsigned char SdRead();
unsigned char SdResponse();
void SdCommand(unsigned char command, unsigned long argument, unsigned char CRC);
unsigned char SdWriteBlock(unsigned long address);
unsigned char SdReadBlock(unsigned long address,unsigned long len);
void initbaud(void);
bit writeID(unsigned long addTemp);
bit readID(unsigned long addTemp,unsigned long len);
bit checkExist(unsigned char* id);

#endif /* __BSP_SD_H_ */