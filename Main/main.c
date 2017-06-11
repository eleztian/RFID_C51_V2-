/**
������ʹ��˵����
	ˢ����1��
	�û�ƥ��ʧ����1��
	����û���2������ɺ���2��
	ɾ���û���3������ɺ���3��
	ˢɾ���� ������k3 3s ��ɾ�������û���ɾ����ɺ���4��
*/

#include <string.h>
#include <reg52.h>
#include "main.h"
#include "bsp_mfrc522.h"
#include "bsp_eeprom.h"

/* �������ݵ��洢�� */
#define updataID() eeprom_sector_erase(NORMAL_CARD_ADDR);\
                   eeprom_write_data(NORMAL_CARD_ADDR, DATA, BUFF_LEN)
/* ���»�ȡ���� */
#define reloadID() eeprom_read_data(NORMAL_CARD_ADDR, DATA, BUFF_LEN)


unsigned  char xdata DATA[BUFF_LEN]={0}; /* ����ʹ��xdata */
unsigned int Card_Timer0 = 0;  /* ��¼�Ѵ濨������ */	
unsigned char G_ucTempbuf[4]; /* ��ȡ�Ŀ�����Ϣ */
unsigned char Flag = 0; /* ��־���� */
unsigned char Timer0 = 0; /* ��ʱ��0������ */
unsigned char Timer1 = 0; /* ��ʱ��1������*/

/*
 *  �������� i ��
 */
void FM10ms(unsigned char i){
	while(i>0){
        FM = 0;
        delay_10ms(8);
        FM = 1;
        delay_10ms(8);
        i--;
	}
}

/* ��ʼ�������� 9600 */
void initbaud(void)
{
    TMOD=0X20;
    TH1=0XFD;
    TL1=0XFD;
    PCON=0X00;
    TR1=1;
    SCON=0X50;
}

/* ��ʱ��0��ʼ����  */
void Timer00Init(void)
{
	TMOD|=0x01;
	TH0 = (65536-50000)/256;
	TL0 = (65536-50000)%256;
	EA = 1;
	ET0 = 1;
	TR0 = 0;
}

/* ��ʱ��1��ʼ����  */
void Timer1Init(void)
{
    TMOD |= 0X01;
    TL1 = 0X00;
    TH1 = 0X4C;
    TR1 = 1;
    ET1 = 1;
    EA = 1;
}

/* ����
   ���� 1
   ������ 0*/
bit checkExist(unsigned char* id){
	int i=0;
	for(i=0;i<BUFF_LEN;i+=4){
		if (strncmp(id,DATA+i,4) == 0)
			return 1;
	}
	return 0;
}

/* ɾ�������û� */
void deletAllUser(){
	memset(DATA,0X00, BUFF_LEN);
    updataID();
    reloadID();
}

/* ��������û� */
void get_card()
{
    unsigned status;
    long int Counter_reset = 0;	/* ��ʱ������ */   
    while(1){
        Counter_reset++;
        if(Counter_reset == 10000)/*  ����rc522 
                                      ��Ч��ֹ������RC522������*/
        {
            PcdReset();
            PcdAntennaOff(); 
            delay_10ms(1);
            PcdAntennaOn();
            M500PcdConfigISOType( 'A' );
            Counter_reset = 0;
        }
        status = PcdRequest(PICC_REQALL, G_ucTempbuf); /* Ѱ�� */
        if (status != MI_OK)
            continue;
        status = PcdAnticoll(G_ucTempbuf); /* ����ͻ */
        if (status == MI_OK)
        {
            PcdHalt(); /* �ر�ic��*/
            break;
        }
        
        delay_10ms(1);	
    }
}

/* ����û� */
void addUser(unsigned char* admin){
    unsigned char zro[]={0,0,0,0,0xFF,0XFF,0XFF,0XFF};
    unsigned char old_id[4]={0,0,0,0};
    unsigned int i,j;
    while(1){
        for(i=0;i<4;i++)
            G_ucTempbuf[i]=0;
        get_card(); 
        if(strncmp(old_id,G_ucTempbuf,4) == 0 && Flag == 1){ //5s�ڲ����ظ�ˢͬһ�ſ�
            continue;
        }
        else{
            strncpy(old_id,G_ucTempbuf,4); /* ��¼��ǰ���ţ�Ϊ��һ�η��ظ� */
            Flag = 1;
            TR0 = 0;
            TR0 = 1;
        }
        if(strncmp(admin,G_ucTempbuf,4)==0) /* ����Ա�� */
            break;
        else if(!checkExist(G_ucTempbuf)){
            for(i=0;i<BUFF_LEN;i+=4){
                if(strncmp(DATA+i,zro,4)==0 || strncmp(DATA+i,zro+4,4)==0){
                    for(j=0;j<4;j++){
                        DATA[i+j] = G_ucTempbuf[j];
                    }
                    SBUF=1;
                    while(!TI);
                    TI=0;
                    delay_10ms(1);
                    break;
                }
            }
        }
    }
    /* ���µ��洢�� */
    updataID();
    reloadID();
    delay_10ms(10);
}

/* ɾ���û� */
void deletUser(unsigned char *admin){
    unsigned char zro[]={0,0,0,0,0xFF,0XFF,0XFF,0XFF};  /* ����DATA �� 215���ֽڳ���0xFF ԭ���꣬��TM��֪��*/
    unsigned char old_id[4]={0,0,0,0};
    unsigned int i,j;
    while(1){
        for(i=0;i<4;i++)
            G_ucTempbuf[i]=0;
        get_card();
         if(strncmp(old_id,G_ucTempbuf,4) == 0 && Flag == 1){ /* 5s�ڲ����ظ�ˢͬһ�ſ� */
            continue;
        }
        else{
            strncpy(old_id,G_ucTempbuf,4); /* ��¼��ǰ���ţ�Ϊ��һ�η��ظ� */
            Flag = 1;
            TR0 = 0;
            TR0 = 1;
        }
        if(strncmp(admin,G_ucTempbuf,4)==0) /* ����Ա�� */
            break;
        else if( !checkExist(G_ucTempbuf)){
            for(i=0;i<BUFF_LEN;i+=4){ /* ��ȫ���������������е�Ҫɾ����idȫ��ɾ�� */
                if(strncmp(DATA+i,G_ucTempbuf,4)==0){
                    for(j=0;j<4;j++){
                        DATA[i+j] = 0;
                    }
                }
            }
        }
    }
    updataID();
    delay_10ms(10);
}

/* ���� ͨ�����ڷ���1������ */
void debug_send_card(unsigned char *x)
{
    unsigned char i;
    for(i=0;i<4;i++){
        SBUF=x[i];
        while(!TI);
        TI=0;
        delay_10ms(1);
    }
}


void main()
{   
    long int Counter_reset = 0;	/* ��ʱ������ */   
    unsigned char admin[8];	/* ����Ա������Ϣ�� ǰ4λ ����û��� ��4λɾ���û���û��ʹ�ã�*/
    unsigned char old_id[4]={0,0,0,0};
    unsigned int i;
    IP=2; /* �������ȼ�����ֹ��ռ���Ź��ж� */
    OPENDOOR = 1;
    initbaud();
    Timer00Init();
    Timer1Init(); /* �������Ź� */

    /* ��ʼ��RFID */
    PcdReset();
    PcdAntennaOff(); 
    delay_10ms(1);
    PcdAntennaOn();  
    M500PcdConfigISOType( 'A' );
    
    memset(DATA,0x00, BUFF_LEN); /* ���� */
    
    for(i=0;i<8;i++){
        admin[i] = DATA[i];
    }
     
    reloadID(); /* ��ȡ�û����� */
    
    FM10ms(5); /* �����ʼ���ɹ�  ��������5�� */
    
    /***************  ��ʼ����� *******************/
    
    while ( 1 )
    { 	
        get_card(); /* ��ȡһ�����ţ����� */
        if(strncmp(old_id,G_ucTempbuf,4) == 0 && Flag == 1){ /* 5s�ڲ����ظ�ˢͬһ�ſ� */
            continue;
        }
        else{
            strncpy(old_id,G_ucTempbuf,4); /* ��¼��ǰ���ţ�Ϊ��һ�η��ظ� */
            Flag = 1;
            TR0 = 0;
            TR0 = 1;
        }
        FM10ms(1); /* ˢ������1�� */
        //debug_send_card(G_ucTempbuf);
        
        if(strncmp(admin,G_ucTempbuf,4)==0){ /* ƥ�䵽����Ա�� �����û� */
            FM10ms(2);
            addUser(admin);
            FM10ms(2);
        }
        else if(strncmp(admin+4,G_ucTempbuf,4)==0){ /* ƥ�䵽����Աɾ���� ɾ���û� */
            if(DELETEALL==0){
                delay_10ms(500); /*��סK3��ˢɾ������Ա������5S��ɾ�������û� */
                if(DELETEALL==0){
                    deletAllUser();
                    FM10ms(4);
                    continue;
                }
            }
            FM10ms(3);
            deletUser(admin+4);
            FM10ms(3);
        }
        else{		
            if(checkExist(G_ucTempbuf)){ /* ƥ���û� */
                OPENDOOR = 0;   /* ���� */
                delay_10ms(30);
                OPENDOOR = 1;
            }
            else{
                FM10ms(1);
            }
            delay_10ms(90);
        }
        PcdHalt(); /* �ر�ic�� */
        delay_10ms(1);	
    }
}

/* ��ʱȥ�ظ�ˢ�� */
void time0() interrupt 1{
	TH0 = (65536-50000)/256;
	TL0 = (65536-50000)%256;
	Timer0++;
	if(Timer0 == 100){
		Flag = 0;
		Timer0 = 0;
		TR0 = 0;
	}
}

/* ���Ź���ʱι�� */
void time1() interrupt 3{ 
    TH0 = 0x00;
	TL0 = 0x4c;
    Timer1++;
    if (Timer1 == 165) /* 8.250������1�ο��Ź� */
    {
        WEIGOU;
        Timer1 = 0;
    }
}

