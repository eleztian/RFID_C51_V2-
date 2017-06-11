/**
蜂鸣器使用说明：
	刷卡响1声
	用户匹配失败响1声
	添加用户响2声，完成后响2声
	删除用户响3声，完成后响3声
	刷删除卡 并长按k3 3s ，删除所有用户，删除完成后响4声
*/

#include <string.h>
#include <reg52.h>
#include "main.h"
#include "bsp_mfrc522.h"
#include "bsp_eeprom.h"

/* 更新数据到存储器 */
#define updataID() eeprom_sector_erase(NORMAL_CARD_ADDR);\
                   eeprom_write_data(NORMAL_CARD_ADDR, DATA, BUFF_LEN)
/* 重新获取数据 */
#define reloadID() eeprom_read_data(NORMAL_CARD_ADDR, DATA, BUFF_LEN)


unsigned  char xdata DATA[BUFF_LEN]={0}; /* 必须使用xdata */
unsigned int Card_Timer0 = 0;  /* 记录已存卡的数量 */	
unsigned char G_ucTempbuf[4]; /* 读取的卡号信息 */
unsigned char Flag = 0; /* 标志变量 */
unsigned char Timer0 = 0; /* 定时器0计数器 */
unsigned char Timer1 = 0; /* 定时器1计数器*/

/*
 *  蜂鸣器响 i 声
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

/* 初始化波特率 9600 */
void initbaud(void)
{
    TMOD=0X20;
    TH1=0XFD;
    TL1=0XFD;
    PCON=0X00;
    TR1=1;
    SCON=0X50;
}

/* 定时器0初始化；  */
void Timer00Init(void)
{
	TMOD|=0x01;
	TH0 = (65536-50000)/256;
	TL0 = (65536-50000)%256;
	EA = 1;
	ET0 = 1;
	TR0 = 0;
}

/* 定时器1初始化；  */
void Timer1Init(void)
{
    TMOD |= 0X01;
    TL1 = 0X00;
    TH1 = 0X4C;
    TR1 = 1;
    ET1 = 1;
    EA = 1;
}

/* 查重
   存在 1
   不存在 0*/
bit checkExist(unsigned char* id){
	int i=0;
	for(i=0;i<BUFF_LEN;i+=4){
		if (strncmp(id,DATA+i,4) == 0)
			return 1;
	}
	return 0;
}

/* 删除所有用户 */
void deletAllUser(){
	memset(DATA,0X00, BUFF_LEN);
    updataID();
    reloadID();
}

/* 清空所有用户 */
void get_card()
{
    unsigned status;
    long int Counter_reset = 0;	/* 定时器计数 */   
    while(1){
        Counter_reset++;
        if(Counter_reset == 10000)/*  重置rc522 
                                      有效防止运行中RC522线脱落*/
        {
            PcdReset();
            PcdAntennaOff(); 
            delay_10ms(1);
            PcdAntennaOn();
            M500PcdConfigISOType( 'A' );
            Counter_reset = 0;
        }
        status = PcdRequest(PICC_REQALL, G_ucTempbuf); /* 寻卡 */
        if (status != MI_OK)
            continue;
        status = PcdAnticoll(G_ucTempbuf); /* 防冲突 */
        if (status == MI_OK)
        {
            PcdHalt(); /* 关闭ic卡*/
            break;
        }
        
        delay_10ms(1);	
    }
}

/* 添加用户 */
void addUser(unsigned char* admin){
    unsigned char zro[]={0,0,0,0,0xFF,0XFF,0XFF,0XFF};
    unsigned char old_id[4]={0,0,0,0};
    unsigned int i,j;
    while(1){
        for(i=0;i<4;i++)
            G_ucTempbuf[i]=0;
        get_card(); 
        if(strncmp(old_id,G_ucTempbuf,4) == 0 && Flag == 1){ //5s内不能重复刷同一张卡
            continue;
        }
        else{
            strncpy(old_id,G_ucTempbuf,4); /* 记录当前卡号，为下一次防重复 */
            Flag = 1;
            TR0 = 0;
            TR0 = 1;
        }
        if(strncmp(admin,G_ucTempbuf,4)==0) /* 管理员卡 */
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
    /* 更新到存储器 */
    updataID();
    reloadID();
    delay_10ms(10);
}

/* 删除用户 */
void deletUser(unsigned char *admin){
    unsigned char zro[]={0,0,0,0,0xFF,0XFF,0XFF,0XFF};  /* 由于DATA 后 215个字节出现0xFF 原因不详，真TM不知道*/
    unsigned char old_id[4]={0,0,0,0};
    unsigned int i,j;
    while(1){
        for(i=0;i<4;i++)
            G_ucTempbuf[i]=0;
        get_card();
         if(strncmp(old_id,G_ucTempbuf,4) == 0 && Flag == 1){ /* 5s内不能重复刷同一张卡 */
            continue;
        }
        else{
            strncpy(old_id,G_ucTempbuf,4); /* 记录当前卡号，为下一次防重复 */
            Flag = 1;
            TR0 = 0;
            TR0 = 1;
        }
        if(strncmp(admin,G_ucTempbuf,4)==0) /* 管理员卡 */
            break;
        else if( !checkExist(G_ucTempbuf)){
            for(i=0;i<BUFF_LEN;i+=4){ /* 完全遍历，将里面所有的要删除的id全部删除 */
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

/* 调试 通过串口发送1个卡号 */
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
    long int Counter_reset = 0;	/* 定时器计数 */   
    unsigned char admin[8];	/* 管理员卡号信息， 前4位 添加用户， 后4位删除用户（没有使用）*/
    unsigned char old_id[4]={0,0,0,0};
    unsigned int i;
    IP=2; /* 降低优先级，防止抢占看门狗中断 */
    OPENDOOR = 1;
    initbaud();
    Timer00Init();
    Timer1Init(); /* 开启看门狗 */

    /* 初始化RFID */
    PcdReset();
    PcdAntennaOff(); 
    delay_10ms(1);
    PcdAntennaOn();  
    M500PcdConfigISOType( 'A' );
    
    memset(DATA,0x00, BUFF_LEN); /* 清零 */
    
    for(i=0;i<8;i++){
        admin[i] = DATA[i];
    }
     
    reloadID(); /* 读取用户卡号 */
    
    FM10ms(5); /* 程序初始化成功  蜂鸣器想5声 */
    
    /***************  初始化完成 *******************/
    
    while ( 1 )
    { 	
        get_card(); /* 获取一个卡号，阻塞 */
        if(strncmp(old_id,G_ucTempbuf,4) == 0 && Flag == 1){ /* 5s内不能重复刷同一张卡 */
            continue;
        }
        else{
            strncpy(old_id,G_ucTempbuf,4); /* 记录当前卡号，为下一次防重复 */
            Flag = 1;
            TR0 = 0;
            TR0 = 1;
        }
        FM10ms(1); /* 刷到卡响1声 */
        //debug_send_card(G_ucTempbuf);
        
        if(strncmp(admin,G_ucTempbuf,4)==0){ /* 匹配到管理员卡 增加用户 */
            FM10ms(2);
            addUser(admin);
            FM10ms(2);
        }
        else if(strncmp(admin+4,G_ucTempbuf,4)==0){ /* 匹配到管理员删除卡 删除用户 */
            if(DELETEALL==0){
                delay_10ms(500); /*按住K3键刷删除管理员卡长按5S后删除所有用户 */
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
            if(checkExist(G_ucTempbuf)){ /* 匹配用户 */
                OPENDOOR = 0;   /* 开门 */
                delay_10ms(30);
                OPENDOOR = 1;
            }
            else{
                FM10ms(1);
            }
            delay_10ms(90);
        }
        PcdHalt(); /* 关闭ic卡 */
        delay_10ms(1);	
    }
}

/* 定时去重复刷卡 */
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

/* 看门狗定时喂狗 */
void time1() interrupt 3{ 
    TH0 = 0x00;
	TL0 = 0x4c;
    Timer1++;
    if (Timer1 == 165) /* 8.250秒清零1次看门狗 */
    {
        WEIGOU;
        Timer1 = 0;
    }
}

