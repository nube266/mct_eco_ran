/*
 * File : Main_Circuit_Program.c
 * Author : ECORUN 2017
 * Use PIC : PIC16F1938
 * プログラム内容 : Main_Circuit制御
 * Created on 2017/06/22, 22:51
 */


#include<xc.h>

#define _XTAL_FREQ  8000000     /* delay用(クロック8MHzで動作時) */

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

//変数の定義
int rx_data;/*送信するデータを格納する
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
int rx_cel = 0x00;  //セルモータ制御関数
int rx_ign = 0x00;  //イグニッション制御関数
int rx_hor = 0x00;  //ホーン制御関数
int rx_ser = 0x00;  //サーボ制御関数
char rx_flag=0; //受信状態に関するフラグ    １ならば受信データがRCREGにある
                //                         ０ならば受信データがRCREGにない


void interrupt Interreceive(void){

    if(RCIF==1){//割り込みがUSART通信の受信ならば

        if(OERR==1){        //オーバーランエラーが発生した場合
            RCSTA=0;
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

    initializePort();   //各ポートの設定

    //シリアル通信に関する設定
    RCSTA=0x90;
    /*シリアルポートは有効　8bit受信　イネーブルビットON　
      受信データの9bit目はデータかパリティビット        */
    SYNC=0;
    BRG16=0;    //8bitボーレートジェネレータを使用
    BRGH=1;     //非同期モードで高速
    SPBRG=51;  //ボーレート9600bps

    //受信割り込みに関する設定
    RCIF=0; //USARTの割り込みフラグを初期化
    RCIE=1; //USARTの割り込み受信を有効
    PEIE=1; //周辺装置割り込みを有効
    GIE=1;  //全割り込み処理を初期化


    __delay_ms(10);

    while(1){   //無限ループ
        //シリアル通信によるデータの取得
        if(rx_flag==1){
            
            //各SWの状態確認 & 1bit目にデータをシフト
            rx_cel = rx_data & 0x01;  //cel_SWの状態を代入            
            rx_ign = (rx_data & 0x02) >> 1;  //ign_SWの状態を代入            
            rx_hor = (rx_data & 0x04) >> 2;  //hor_SWの状態を代入            
            rx_ser = (rx_data & 0x08) >> 3;  //ser_SWの状態を代入
            
            
            //動作指定 (2重確認)
            if(rx_hor == 1 && RC2 == 1){  //ホーンSW ON
                LATAbits.LATA1 = 1;       
            }
            else{                         //ホーンSW OFF
                LATAbits.LATA1 = 0; 
            }
            
            if(rx_ser == 1 && RC3 == 1){  //サーボSW ON
                LATAbits.LATA4 = 1;
            }
            else{                         //サーボSW OFF
                LATAbits.LATA4 = 0; 
            }
            
            if(rx_ign == 1 && RC1 == 1){  //イグニッションSW ON
                LATBbits.LATB0 = 1;
            }
            else{                         //イグニッションSW OFF
                LATBbits.LATB0 = 0; 
            } 
            
            if(rx_cel == 1 && RC0 == 1){  //セルモータSW ON
                LATBbits.LATB1 = 1;
            }
            else{                         //セルモータSW OFF
                LATBbits.LATB1 = 0; 
            }            
            
            rx_data = 0x00;  //データクリア
            rx_flag=0;       //フラグクリア
        }
        
        
        if(rx_ign == 0){                 // エンジン停止 >> 電磁バルブ ON 
            LATAbits.LATA0 = 1;
        }
        else{                            // 電磁バルブ OFF
            LATAbits.LATA0 = 0;
        }
    }
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


    TRISA = 0x00;   
    /*RA0 : ソレノイド0 Signal OUTPUT
      RA1 : ホーン Signal OUTPUT
      RA2 : ソレノイド1 Signal OUTPUT (未使用)
      RA3 : ソレノイド2 Signal OUTPUT (未使用)
      RA4 : SERVO Signal OUTPUT
      RA5 : エンコーダ (未使用)
      RA6 : OPEN
      RA7 : OPEN
     */
    TRISB = 0x00;   
    /*RB0 : イグニッション Signal OUTPUT
      RB1 : セルモータ　Signal OUTPUT
      RB2 : リレー Signal OUTPUT (未使用)
      RB3 : OPEN
      RB4 : OPEN
      RB5 : OPEN
      RB6 : CLK
      RB7 : DAT
     */
    TRISC = 0xAF;   
    /*RC0 : SW_data0_cel INPUT
      RC1 : SW_data1_ign INPUT
      RC2 : SW_data2_hor INPUT
      RC3 : SW_data3_ser INPUT
      RC4 : SDA
      RC5 : SUPPLY_Signal INPUT
      RC6 : TX PIN OUTPUT
      RC7 : RX PIN INPUT
     */
    TRISEbits.TRISE3 = 1;
    /*  ビット 7-0	TRISX<7:0>: PORTX トライステート制御ビット
        1 = PORTX ピンが入力として設定されている ( トライステート )
        0 = PORTX ピンが出力として設定されている                    */

}
