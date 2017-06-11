#include <reg52.h>//����ͨ������
#include <stdio.h>
#include <string.h>
#include "bsp_sd.h"

#define WEIGOU WDT_CONTR=0x34 

//===========================================================
//����512�ֽڻ�������ע����Ҫʹ�� xdata�ؼ���
unsigned char xdata DATA[512];

void delayms(unsigned int count)
{
    int i,j;                                                                               
    for(i=0;i<count;i++)
      for(j=0;j<260;j++);                                                                                  
}
//===========================================================
//дһ�ֽڵ�SD��,ģ��SPI���߷�ʽ
void SdWrite(unsigned char n)
{

	unsigned char i;

	for(i=8;i;i--)
	{
		SD_CLK=0;
		SD_DI=(n&0x80);
		n<<=1;
		SD_CLK=1;
	}
	SD_DI=1;
}
//===========================================================
//��SD����һ�ֽ�,ģ��SPI���߷�ʽ
unsigned char SdRead()
{
	unsigned char n,i;
	for(i=8;i;i--)
	{
		SD_CLK=0;
		SD_CLK=1;
		n<<=1;
		if(SD_DO) n|=1;

	}
	return n;
}
//============================================================
//���SD������Ӧ
unsigned char SdResponse()
{
	unsigned char i=0,response;

	while(i<=8)
	{
		response = SdRead();
		if(response==0x00)
		break;
		if(response==0x01)
		break;
		i++;
	}
	return response;
}
//================================================================
//�����SD��
void SdCommand(unsigned char command, unsigned long argument, unsigned char CRC)
{

	SdWrite(command|0x40);
	SdWrite(((unsigned char *)&argument)[0]);
	SdWrite(((unsigned char *)&argument)[1]);
	SdWrite(((unsigned char *)&argument)[2]);
	SdWrite(((unsigned char *)&argument)[3]);
	SdWrite(CRC);
}
//================================================================
//��ʼ��SD��
unsigned char SdInit(void)
{
	int delay=0, trials=0;
	unsigned char i;
	unsigned char response=0x01;

	SD_CS=1;
	for(i=0;i<=9;i++)
	SdWrite(0xff);
	SD_CS=0;

	//Send Command 0 to put MMC in SPI mode
	SdCommand(0x00,0,0x95);


	response=SdResponse();

	if(response!=0x01)
	{
		return 0;
	}

	while(response==0x01)
	{
		SD_CS=1;
		SdWrite(0xff);
		SD_CS=0;
		SdCommand(0x01,0x00ffc000,0xff);
		response=SdResponse();
	}
	SD_CS=1;
	SdWrite(0xff);
	return 1;
}
//================================================================
//��SD��ָ����ַд����,һ�����512�ֽ�
unsigned char SdWriteBlock(unsigned long address)
{
	unsigned int i;
	unsigned char dataResp;
	//Block size is 512 bytes exactly
	//First Lower SS

	SD_CS=0;
	//Then send write command
	SdCommand(0x18,address,0xff);
	WEIGOU;
	if(SdResponse()==00)
	{
		SdWrite(0xff);
		SdWrite(0xff);
		SdWrite(0xff);
		//command was a success - now send data
		//start with DATA TOKEN = 0xFE
		SdWrite(0xfe);
		//now send data

			
		WEIGOU;
		for(i=0;i<512;i++)
			SdWrite(DATA[i]);
		WEIGOU;
			
		//data block sent - now send checksum
		SdWrite(0xff); //���ֽ�CRCУ��, Ϊ0XFFFF ��ʾ������CRC
		SdWrite(0xff);
		//Now read in the DATA RESPONSE token
		dataResp=SdRead();
		//Following the DATA RESPONSE token
		//are a number of BUSY bytes
		//a zero byte indicates the MMC is busy

		while(SdRead()==0);
		WEIGOU;
		dataResp=dataResp&0x0f; //mask the high byte of the DATA RESPONSE token
		SD_CS=1;
		SdWrite(0xff);
		if(dataResp==0x0b)
		{
		//printf("DATA WAS NOT ACCEPTED BY CARD -- CRC ERROR\n");
		return 0;
		}
		if(dataResp==0x05)
		return 1;

		//printf("Invalid data Response token.\n");
		return 0;
	}
	//printf("Command 0x18 (Write) was not received by the MMC.\n");
	return 0;
}

//=======================================================================
//��SD��ָ����ַ��ȡ����,һ�����512�ֽ�
unsigned char SdReadBlock(unsigned long address,unsigned long len)
{
	unsigned int i;
	//Block size is 512 bytes exactly
	//First Lower SS

	 //printf("MMC_read_block\n");

	SD_CS=0;
	//Then send write command
	SdCommand(0x11,address,0xff);

	if(SdResponse()==00)
	{
	//command was a success - now send data
	//start with DATA TOKEN = 0xFE
	while(SdRead()!=0xfe);

	WEIGOU;
	for(i=0;i<len;i++)
		DATA[i]=SdRead();
	WEIGOU;	
		
	//data block sent - now send checksum
	SdRead();
	SdRead();
	//Now read in the DATA RESPONSE token
	SD_CS=1;
	SdRead();
	return 1;
	}
	 //printf("Command 0x11 (Read) was not received by the MMC.\n");
	return 0;
	}

	void initbaud(void)
	{
		TMOD=0X20;
		TH1=0XFD;
		TL1=0XFD;
		PCON=0X00;
		TR1=1;
		SCON=0X50;//8λ���ؿɱ�
		//SCON=0X52;//8λ���ؿɱ� TI���ж�
}
//============================================================
//������ 

bit writeID(unsigned long addTemp){
	unsigned char ok;
	SdInit();         //SD����ʼ��
	
	ok = SdWriteBlock(addTemp);
	WEIGOU;
	delayms(900);
	WEIGOU;
	return ok;
}
bit readID(unsigned long addTemp,unsigned long len){
	SdInit();         //SD����ʼ��
	SdReadBlock(addTemp,len);//ÿ�ζ���len�ֽڷŵ�������
	return 1;
}
bit checkExist(unsigned char* id){
	int i=0;
	WEIGOU;
	for(i=0;i<512;i+=4){
		if (strncmp(id,DATA+i,4) == 0)
			return 1;
	}
	return 0;
}
