/*
 * File : SW_Control_Program.c
 * Author : ECORUN 2017
 * Use PIC : PIC16F1823
 * プログラム内容 : SWの状態をMain_Circuitに送信
 * Created on 2017/06/22, 22:57
 */


#include <xc.h>

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

#define _XTAL_FREQ 8000000 //8MHZ

//関数の定義
void Usart_data(void);
void Bit4_data(void);

////////////変数の定義////////////
int tx_data=0;   /*送信するデータを格納する
Bit 7-5:未実装なので常に０にセット
Bit   4:クラッチ（自動）に関するビット
        ON:１　OFF：０
Bit   3:クラッチ（手動）に関するビット
        ON:１　OFF：０
Bit   2:ホーンに関するビット
        ON:１　OFF：０
Bit   1:インジェクションに関するビット
        ON:１　OFF：０
Bit   0:セルに関するビット
        ON:１　OFF：０
*/
int tx_before=0x00;    //以前のデータを保存するための変数
int i=0;

void main(void) {
    OSCCON = 0b01110000;   //8MHz(内部オシレータ)
    TRISA =  0b00000000;   //RAポートすべて出力
    TRISC =  0b00011111;   //RC0?4ポートすべて入力
    ANSELA = 0b00000000;   //RApinすべてディジタル
    ANSELC = 0b00000000;   //RCpinすべてディジタル
    TXCKSEL=1;             //TX機能をRA0に割り当てる
    
    PORTA = 0x00;          //初期化
    PORTC = 0x00;          //初期化
    
    //シリアル通信設定
    TXSTA = 0b00100100;    /*
Bit 7：ＣＳＲＣ　　クロックソースの選択ビット
　　　　　　　　　　非同期モードはこのビット設定無意味

Bit 6：ＴＸ９　　　９ビットでの送信モード指定ビット
　　　　　　　　　　１：９ビットデータで送信する(パリティ有りとする)
　　　　　　　　　　０：８ビットデータで送信する(パリティ無しとする)

Bit 5：ＴＸＥＮ　　送信を許可するかの指定ビット
　　　　　　　　　　１：許可、送信を行う
　　　　　　　　　　０：禁止、送信は行わない

Bit 4：ＳＹＮＣ　　ＵＳＡＲＴモードの指定ビット
　　　　　　　　　　１：同期モード
　　　　　　　　　　０：非同期モード(通常はこの設定とする)

Bit 3：ＳＥＮＤＢ　ブレーク文字送信指定ビット
　　　　　　　　　　１：ブレーク文字を送信する場合にセットします。
 　　　　　　　　　　０：通常は０にセット
　　　　　　　　　　１６Ｆ６２８Ａにはこの機能は有りません。

Bit 2：ＢＲＧＨ　　高速ボーレートの指定ビット
　　　　　　　　　　１：高速に設定(通常はこの設定でＯＫ)
　　　　　　　　　　０：低速に設定

Bit 1：ＴＲＭＴ　　送信レジスタ(ＴＳＲレジスタ)の状態を得るビット
　　　　　　　　　　１：レジスタは空です
　　　　　　　　　　０：レジスタはまだ未送信

Bit 0：ＴＸ９Ｄ　　９ビット目のデータ用(送信時のパリティビットデータとして使用する)

*/
    RCSTAbits.SPEN = 1;     //シリアルポート有効
    BAUDCONbits.BRG16 = 0;  //8bitBaudレートジェネレータ
    SPBRGH = 0x00;          
    SPBRG = 51;             //Baudレート9600bps SYNC=0,BRGH=1,BRG16=0

    __delay_ms(10); //動作が安定するまで待つため

    while(1){   //ここから無限ループ
        
        Bit4_data();  //各SWの状態をI/O_PORTで送信

        Usart_data(); //各SWの状態をUSARTで送信

        //チャタリング防止用のif文
        if(tx_data!=tx_before)  __delay_ms(20);
        //前回値と違う場合チャタリングがなくなるまで待つ

        while(!TXIF);   //TXREGが空になるまで待つ
        TXREG=tx_data;  //TXREG（送信用レジスタ）にデータを格納

        tx_before=tx_data;  //このループでのtx_dataをtx_beforeに保存
        tx_data=0x00;       //初期化

        __delay_ms(10);     //送信側の動作を受信側の動作より遅くするため

    }       
    
}

void Usart_data(void){

        if(RC0==1){
            tx_data=tx_data | 0b00000001;
            //RC0（セル）がオンならtx_dataのbit0を１にセット
        }

        if(RC1==1){
            tx_data=tx_data | 0b00000010;
            //RC1（インジェクション）がオンならtx_dataのbit1を１にセット
        }

        if(RC2==1){
            tx_data=tx_data | 0b00000100;
            //RC2（ホーン）がオンならtx_dataのbit2を１にセット
        }

        if(RC3==1){
            tx_data=tx_data | 0b00001000;
            //RC3（手動クラッチ）がオンならtx_dataのbit3を１にセット
        }

        if(RC4==1){
            tx_data=tx_data | 0b00010000;
            //RC4（自動クラッチ）がオンならtx_dataのbit4を１にセット
        }

}

void Bit4_data(void){

        //RC0（セル）がオンならtx_dataのbit0を１にセット
        if(RC0==1){
            RA2=1;
        }
        else{
            RA2=0;
        }

        //RC1（イグニション）がオンならtx_dataのbit1を１にセット
        if(RC1==1){
            RA3=1;
        }
        else{
            RA3=0;
        }

        //RC2（ホーン）がオンならtx_dataのbit2を１にセット
        if(RC2==1){
            RA4=1;
        }
        else{
            RA4=0;
        }

        //RC3（手動クラッチ）がオンならtx_dataのbit3を１にセット
        if(RC3==1){
            RA5=1;
        }

        else{
            RA5=0;
        }

}