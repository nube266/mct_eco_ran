/*
 * File:   LCD_Display_Program.c
 * Author: ECORUN 2017
 * Use PIC : PIC16F1938
 * プログラムの内容 : LCDにSeeddataを表示
 * Created on 2017/06/22, 23:24
 */

#include <xc.h>
#include "define.h"
#include "lcd.h"
#include <string.h>
#include<stdio.h>

// config1
#pragma config FCMEN = OFF		// 外部クロック監視しない
#pragma config IESO = OFF		// 2段速クロックスタートアップモード使用しない
#pragma config CLKOUTEN = OFF	// クロック出力を使用しない
#pragma config BOREN = ON		// ブラウンアウトリセットを使用する
#pragma config CPD = OFF		// データメモリを保護しない
#pragma config CP = OFF			// プログラムメモリを保護しない
#pragma config MCLRE = OFF		// 外部リセット信号を使用しない
#pragma config PWRTE = ON		// 電源ONから64ms後にプログラム開始
#pragma config WDTE = OFF		// ウォッチドッグタイマ無効
#pragma config FOSC = INTOSC	// 内部クロックを使用する

// config2
#pragma config LVP = OFF		// 低電圧プログラミング無効
#pragma config BORV = HI		// ブラウンアウトリセット電圧2.7V
#pragma config STVREN = ON		// スタックがオーバーフローやアンダーフローしたらリセット
#pragma config PLLEN = OFF		// 32MHzで動作させない(4xPLL無効)
#pragma config VCAPEN = OFF		// VCAP無効
#pragma config WRT = OFF		// Flashメモリを保護しない

//関数のプロトタイプ宣言
void initializePort( void );    //ポート設定の関数
void read(void);                 //受け取った数値を表示する文字列に変換する関数

int i=0;  //for文用
int a=0x30,b=0x30,c=0x30;           //受信データを一旦格納するための変数
int num1=0,num2=0,num3=0;//数値格納用
char moji1[]="para"; //数値を文字列に変換したときに格納する用
char moji2[]="para"; //数値を文字列に変換したときに格納する用
char moji3[]="para"; //数値を文字列に変換したときに格納する用
char rx_flag=0; //受信状態に関するフラグ    １ならば受信データがRCREGにある
                //                         ０ならば受信データがRCREGにない
int rx_data;    //送信するデータを格納する


void interrupt ISR(void){

    if(RCIF==1){//割り込みがUSART通信の受信ならば

        if(OERR==1){        //オーバーランエラーが発生した場合
            RCSTA=0;        //設定をクリア
            RCSTA=0x90;
        }
        else{               //エラーがなければ
            rx_data=RCREG;  //受信データをrx_dataに格納
            rx_flag=1;      //データ受信をしたのでフラグをたてる
            RCIF=0;         //割り込みフラグをリセット
        }

    }

}

void main (void){

    OSCCON = 0b01110010;
    /* 内部オシレータ 8MHz PLL OFF
       bit7 PPL ON OFF bit6-3 内臓クロックの選択　クロック選択*/

    //シリアル通信に関する設定
    RCSTA=0x90;
    /*シリアルポートは有効　8bit受信　イネーブルビットON　
      受信データの9bit目はデータかパリティビット        */
    SYNC=0;
    BRG16=0;    //8bitボーレートジェネレータを使用
    BRGH=1;     //非同期モードで高速
    SPBRG=51;   //ボーレート9600bps


    //割り込みに関する設定

    RCIF=0;     //USARTの割り込みフラグを初期化
    RCIE=1;     //USARTの割り込み受信を有効
    PEIE=1;     //周辺装置割り込みを有効
    GIE=1;      //全割り込み処理を初期化


    initializePort();   //各ポートの設定


    LCD lcd;    //　LCD　構造体

    lcdBegin(&lcd, 16, 2, 0x00);    //LCDの初期化

    while(1){   //無限ループ

        for(i=0;i<4;i++){
            while(!rx_flag);                //受信するまで待つ
            rx_flag=0;                      //フラグをリセット
            if(i==0&&rx_data!=0xFF) i--;    //先頭の情報がくるまでiを減算
            else if(i==1) a=rx_data;        //一桁目の数値を受け取る
            else if(i==2) b=rx_data;        //二桁目の数値を受け取る
            else if(i==3) c=rx_data;        //三桁目の数値を受け取る
        }

        lcdClear(); //LCDの表示をクリアする
        read();     //数値を文字列に変換する

        lcdSetCursor(&lcd, 0, 0);   //lcdの一行目に記述
        lcdPrint(moji1);            //時速の表示
        lcdSetCursor(&lcd, 0, 1);   //lcdの一行目に記述
        lcdPrint("km/h");           //時速の表示

    }

}



void read(void){

    num1=a-0x30;  //測定値をアスキーから一桁の数値に変換
    num2=b-0x30;  //測定値をアスキーから一桁の数値に変換
    num3=c-0x30;  //測定値をアスキーから一桁の数値に変換

    //文字列へ変換する
    if(num1==0)  strcpy(moji1,"0");
    if(num1==1)  strcpy(moji1,"1");
    if(num1==2)  strcpy(moji1,"2");
    if(num1==3)  strcpy(moji1,"3");
    if(num1==4)  strcpy(moji1,"4");
    if(num1==5)  strcpy(moji1,"5");
    if(num1==6)  strcpy(moji1,"6");
    if(num1==7)  strcpy(moji1,"7");
    if(num1==8)  strcpy(moji1,"8");
    if(num1==9)  strcpy(moji1,"9");
    if(num2==0)  strcpy(moji2,"0");
    if(num2==1)  strcpy(moji2,"1");
    if(num2==2)  strcpy(moji2,"2");
    if(num2==3)  strcpy(moji2,"3");
    if(num2==4)  strcpy(moji2,"4");
    if(num2==5)  strcpy(moji2,"5");
    if(num2==6)  strcpy(moji2,"6");
    if(num2==7)  strcpy(moji2,"7");
    if(num2==8)  strcpy(moji2,"8");
    if(num2==9)  strcpy(moji2,"9");
    if(num3==0)  strcpy(moji3,"0");
    if(num3==1)  strcpy(moji3,"1");
    if(num3==2)  strcpy(moji3,"2");
    if(num3==3)  strcpy(moji3,"3");
    if(num3==4)  strcpy(moji3,"4");
    if(num3==5)  strcpy(moji3,"5");
    if(num3==6)  strcpy(moji3,"6");
    if(num3==7)  strcpy(moji3,"7");
    if(num3==8)  strcpy(moji3,"8");
    if(num3==9)  strcpy(moji3,"9");

    //strcat関数は第一引数の後ろに第二引数を変換する関数
//     strcat(&moji1,&moji2);  //文字列の結合
//      strcat(moji1,".");     //文字列の結合
//      strcat(moji1,moji3);   //文字列の結合
//      strcat(moji1,"km/h");  //文字列の結合

     strcat(moji1,moji2);      //文字列の結合
     strcat(moji1,".");        //文字列の結合
     strcat(moji1,moji3);      //文字列の結合

}




void initializePort(void){

    PORTA = 0x00;   //すべて初期化
    PORTB = 0x00;   //すべて初期化
    PORTC = 0x00;   //すべて初期化
    /*ビット 7-0	RX<7:0>: PORTX I/O 値のビット*/


    ANSELA = 0x00;  //すべてデジタルピン
    ANSELB = 0x00;  //すべてデジタルピン
    /*  ビット 7-6	未実装 :「0」として読み出し
        ビット 5-0	ANSX<5:0>: RB<5:0> の各ピンの機能をアナログまたはデジタルのいずれかに選択します。
        0 = デジタル I/O。ピンはポートまたはデジタル特殊機能に割り当てられる
        1 = アナログ入力。ピンはアナログ入力として割り当てられる                                     */

    TRISA = 0x00;   //すべて出力
    TRISB = 0x00;   //すべて出力
    TRISC = 0x80;   //RC7(RXピン)を入力　その他を出力
    /*  ビット 7-0	TRISX<7:0>: PORTX トライステート制御ビット
        1 = PORTX ピンが入力として設定されている ( トライステート )
        0 = PORTX ピンが出力として設定されている                    */

}

