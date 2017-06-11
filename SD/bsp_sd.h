
#ifndef __BSP_SD_H_
#define __BSP_SD_H_

//=============================================================
//����SD����Ҫ��4���ź���
sbit SD_CLK = P3^7;
sbit SD_DI  = P3^5;
sbit SD_DO  = P3^6;
sbit SD_CS  = P3^4;
//===========================================================
//��д���ݱ������
extern unsigned char xdata DATA[512];
//===========================================================
//��������
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