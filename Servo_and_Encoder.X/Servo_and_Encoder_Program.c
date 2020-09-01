/*
 * File:   Servo_and_Encoder_Program.c
 * Author: ECORUN 2017
 * Use PIC : PIC16F1827
 *プログラム内容 : サーボモータ制御 & スピードメータデータ制御
 * Created on 2017/06/22, 23:45
 */


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pic16f1827.h>

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.



#define _XTAL_FREQ 8000000
#define TMR2_OFFSET 6
#define TMR4_OFFSET 0


unsigned char SpeedTime[4] = {0xFF,0x30,0x30,0x30};
unsigned char Moji[4];
unsigned char i=0;  
unsigned char j = 1;
unsigned char T1_Flag = 0;
unsigned short long T2_num = 0;
unsigned char T2_Flag = 0;
unsigned int Cal;
unsigned int NUM1;
double Speed;   
unsigned short long Speed_long;



void interrupt ISR (void){
    
   
    //タイマ1割り込み処理
    if(PIR1bits.TMR1IF == 1){  //タイマ1がオーバーフロー発生
        INTCONbits.IOCIE  = 0;  //状態変化割り込み禁止
        T2_Flag = 1;  //T2_Flag セット
        T1_Flag = 0;  //T1_Flag クリア
        TMR1 = 0;
        T1CONbits.TMR1ON = 0;  //タイマ1停止
        INTCONbits.IOCIE = 1;  //状態変化割込み許可
        PIR1bits.TMR1IF = 0;  //タイマ1オーバーフローフラグクリア
    }
    
    //状態変化割り込み処理
    if(INTCONbits.IOCIF == 1){  //状態変化割り込み発生
        if(IOCBFbits.IOCBF3 == 1){
            IOCBFbits.IOCBF3 = 0;
            
            if(T1CONbits.TMR1ON != 1){  //タイマ2が停止している
               // TMR2=0;                
                 T1CONbits.TMR1ON = 1;  //タイマ2起動
             }
    
             else{
                 T1CONbits.TMR1ON = 0;  //タイマ停止
                 T2_num = TMR1;  //タイマのカウント数を代入
                 TMR1=0;                
                 T1_Flag = 1;  //T1_Flag セット
             }
         }
        INTCONbits.IOCIF = 0;  //割り込みフラグクリア
     }
    
   
    
     // タイマ2割り込み処理
    if(TMR2IF)  //20ms経過によってタイマ2割り込み発生
    {

        // ||サーボのパルス幅変更|| //
        /*PR4に書き込む値は125~250までの間で変更可能
          125 = 1ms
          250 = 2ms
            1 = 8us
          */
        
        if(RA3 == 1){
            PR4 = TMR4_OFFSET + 250;//2msec  
        }
        
        else{
            PR4 = TMR4_OFFSET + 125;  // 1msec
        }
        
        RB0 = 1;
        T4CONbits.TMR4ON = 1; //T4 START
        TMR2 = TMR2_OFFSET;   // 20msec毎に割り込み
        TMR2IF = 0;           // タイマ2割り込みフラグクリア
        
    }

    //タイマ4割り込み処理
    if(TMR4IF){  //パルス周期経過　(PR4まで経過)
                
        RB0 = 0;
        T4CONbits.TMR4ON = 0; //タイマ4 停止
        PIR3bits.TMR4IF = 0;  //タイマ4割り込みフラグクリア
        
    }
    
    }


int main(int argc, char** argv) {
        
        
    //周波数設定
    OSCCON = 0b01110000; //8MHz
    
    
    //シリアルポート設定
    APFCON0bits.RXDTSEL = 0; //NO.7PIN RX/DT enable
    APFCON1 = 0x00; //NO.8PIN TX/CK enable
  
    
    // I/Oポート設定
    TRISA = 0b00101000;
    /*
      RA7 Encorder_Signal OUTPUT
      RA5 MCLR INPUT
      RA3 Servo_Signal INPUT
      RA0~2,4,6 unenable 
     */
    ANSELA = 0x00; // PORTA all digital
    
    TRISB = 0b00001010;
    /*
      RB4 Encorder2_PWM_Signal INPUT
      RB3 Encorder1_PWM_Signal INPUT mijissou
      RB1,2 SerialPORT_RX/TX
      RB0 Servo_PWM_Signal OUTPUT
      RB5~7 unenable 
     */
    ANSELB = 0x00; // PORTB all digital 
    
    PORTA = 0x00;  //初期化
    PORTB = 0x00;  //初期化
   
    
    
    
   // ||スピードメータ設定|| //
    
    //プルアップ設定
   /* WPUBbits.WPUB3 = 1;  //RB3 Pullup
    WPUBbits.WPUB4 = 1;  //RB4 Pullup
    OPTION_REGbits.nWPUEN = 0;  //Pullup enable
   */ 
    
    //状態変化割り込み設定
    IOCBPbits.IOCBP3 = 1;  //RB3立ち上がりエッジ指定
    //IOCBPbits.IOCBP4 = 1;  //RB4立ち上がりエッジ指定
    IOCBN = 0x00;  //立下りエッジなし
    IOCBF = 0;  // 状態変化フラグクリア
    
    INTCONbits.IOCIF = 0;
    
    //タイマ1割り込み設定
    PIE1bits.TMR1IE = 1;  //タイマ1割り込み許可
    T1CON = 0b00110000;  //postscaler1:16 prescaler1:64 FOSC/4
    //PR2 = 254; //254カウント
    //シリアル通信設定
    TXSTA = 0b00100100;  //非同期モード8bit送信 高速
    RCSTAbits.SPEN = 1;  //シリアルポート有効
    BAUDCONbits.BRG16 = 0; //BRG16 = 0
    SPBRGH = 0;  
    SPBRG = 51;
        
  
    
    //INTCON = 0b01001000; // 全割り込み禁止(PEIE & IOCIE　set)
    
    
    
    // ||サーボモータ設定|| //
    
    // 周波数設定
    
    T2CON = 0b01001110;   // post 1:10  pre 1:16 fosc/64
    PR2 = 255;
    
    T4CON = 0b00000110;  //  post 1:10  pre 1:16 fosc/64
    //割り込み設定
    PIE1bits.TMR2IE = 1; //タイマ2割り込み有効
    PIE3bits.TMR4IE = 1; //タイマ4割り込み有効
    
    //サーボDuty設定
    TMR2 = TMR2_OFFSET;
    PR4 = TMR4_OFFSET + 188; //1.5ms duty
    
    
    T2CONbits.TMR2ON = 1; //タイマ2 start
    T4CONbits.TMR4ON = 0; //タイマ4 stop
    PIR1bits.TMR2IF = 0;  //タイマ2割り込みフラグクリア
    PIR3bits.TMR4IF = 0;  //タイマ2割り込みフラグクリア
    
    
    INTCONbits.IOCIE = 1;
    PEIE = 1;
    GIE = 1;//全割り込みを許可
    
    //Moji[0] = 0xFF;
    
    while(1){
        if( TXSTAbits.TRMT== 1){    
            __delay_ms(5);            
            TXREG = SpeedTime[i];             
            i++;
            if(i==4){
                i=0;          
            }          
        }
        
        
        if((T1_Flag == 1) && (T2_Flag == 0)){
            Speed = ((double)176715/T2_num);
            Speed_long=(unsigned short long)Speed;
            Cal=Speed_long%100;
            SpeedTime[1] = (Cal/10) | 0x30;   
            SpeedTime[2] = (Speed_long%10) | 0x30;
            Speed*=10;
            Speed_long=(unsigned short long)Speed;
            SpeedTime[3] = (Speed_long%10) | 0x30;
            T1_Flag = 0;
        }
        if(T2_Flag == 1){
            SpeedTime[1] = 0x30;
            SpeedTime[2] = 0x30;
            SpeedTime[3] = 0x30;
            T2_Flag = 0;
        }

            
    }   
   return (EXIT_SUCCESS);
 
}
