/*
 * File : SW_Control_Program.c
 * Author : ECORUN 2017
 * Use PIC : PIC16F1823
 * �v���O�������e : SW�̏�Ԃ�Main_Circuit�ɑ��M
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

//�֐��̒�`
void Usart_data(void);
void Bit4_data(void);

////////////�ϐ��̒�`////////////
int tx_data=0;   /*���M����f�[�^���i�[����
Bit 7-5:�������Ȃ̂ŏ�ɂO�ɃZ�b�g
Bit   4:�N���b�`�i�����j�Ɋւ���r�b�g
        ON:�P�@OFF�F�O
Bit   3:�N���b�`�i�蓮�j�Ɋւ���r�b�g
        ON:�P�@OFF�F�O
Bit   2:�z�[���Ɋւ���r�b�g
        ON:�P�@OFF�F�O
Bit   1:�C���W�F�N�V�����Ɋւ���r�b�g
        ON:�P�@OFF�F�O
Bit   0:�Z���Ɋւ���r�b�g
        ON:�P�@OFF�F�O
*/
int tx_before=0x00;    //�ȑO�̃f�[�^��ۑ����邽�߂̕ϐ�
int i=0;

void main(void) {
    OSCCON = 0b01110000;   //8MHz(�����I�V���[�^)
    TRISA =  0b00000000;   //RA�|�[�g���ׂďo��
    TRISC =  0b00011111;   //RC0?4�|�[�g���ׂē���
    ANSELA = 0b00000000;   //RApin���ׂăf�B�W�^��
    ANSELC = 0b00000000;   //RCpin���ׂăf�B�W�^��
    TXCKSEL=1;             //TX�@�\��RA0�Ɋ��蓖�Ă�
    
    PORTA = 0x00;          //������
    PORTC = 0x00;          //������
    
    //�V���A���ʐM�ݒ�
    TXSTA = 0b00100100;    /*
Bit 7�F�b�r�q�b�@�@�N���b�N�\�[�X�̑I���r�b�g
�@�@�@�@�@�@�@�@�@�@�񓯊����[�h�͂��̃r�b�g�ݒ薳�Ӗ�

Bit 6�F�s�w�X�@�@�@�X�r�b�g�ł̑��M���[�h�w��r�b�g
�@�@�@�@�@�@�@�@�@�@�P�F�X�r�b�g�f�[�^�ő��M����(�p���e�B�L��Ƃ���)
�@�@�@�@�@�@�@�@�@�@�O�F�W�r�b�g�f�[�^�ő��M����(�p���e�B�����Ƃ���)

Bit 5�F�s�w�d�m�@�@���M�������邩�̎w��r�b�g
�@�@�@�@�@�@�@�@�@�@�P�F���A���M���s��
�@�@�@�@�@�@�@�@�@�@�O�F�֎~�A���M�͍s��Ȃ�

Bit 4�F�r�x�m�b�@�@�t�r�`�q�s���[�h�̎w��r�b�g
�@�@�@�@�@�@�@�@�@�@�P�F�������[�h
�@�@�@�@�@�@�@�@�@�@�O�F�񓯊����[�h(�ʏ�͂��̐ݒ�Ƃ���)

Bit 3�F�r�d�m�c�a�@�u���[�N�������M�w��r�b�g
�@�@�@�@�@�@�@�@�@�@�P�F�u���[�N�����𑗐M����ꍇ�ɃZ�b�g���܂��B
 �@�@�@�@�@�@�@�@�@�@�O�F�ʏ�͂O�ɃZ�b�g
�@�@�@�@�@�@�@�@�@�@�P�U�e�U�Q�W�`�ɂ͂��̋@�\�͗L��܂���B

Bit 2�F�a�q�f�g�@�@�����{�[���[�g�̎w��r�b�g
�@�@�@�@�@�@�@�@�@�@�P�F�����ɐݒ�(�ʏ�͂��̐ݒ�łn�j)
�@�@�@�@�@�@�@�@�@�@�O�F�ᑬ�ɐݒ�

Bit 1�F�s�q�l�s�@�@���M���W�X�^(�s�r�q���W�X�^)�̏�Ԃ𓾂�r�b�g
�@�@�@�@�@�@�@�@�@�@�P�F���W�X�^�͋�ł�
�@�@�@�@�@�@�@�@�@�@�O�F���W�X�^�͂܂������M

Bit 0�F�s�w�X�c�@�@�X�r�b�g�ڂ̃f�[�^�p(���M���̃p���e�B�r�b�g�f�[�^�Ƃ��Ďg�p����)

*/
    RCSTAbits.SPEN = 1;     //�V���A���|�[�g�L��
    BAUDCONbits.BRG16 = 0;  //8bitBaud���[�g�W�F�l���[�^
    SPBRGH = 0x00;          
    SPBRG = 51;             //Baud���[�g9600bps SYNC=0,BRGH=1,BRG16=0

    __delay_ms(10); //���삪���肷��܂ő҂���

    while(1){   //�������疳�����[�v
        
        Bit4_data();  //�eSW�̏�Ԃ�I/O_PORT�ő��M

        Usart_data(); //�eSW�̏�Ԃ�USART�ő��M

        //�`���^�����O�h�~�p��if��
        if(tx_data!=tx_before)  __delay_ms(20);
        //�O��l�ƈႤ�ꍇ�`���^�����O���Ȃ��Ȃ�܂ő҂�

        while(!TXIF);   //TXREG����ɂȂ�܂ő҂�
        TXREG=tx_data;  //TXREG�i���M�p���W�X�^�j�Ƀf�[�^���i�[

        tx_before=tx_data;  //���̃��[�v�ł�tx_data��tx_before�ɕۑ�
        tx_data=0x00;       //������

        __delay_ms(10);     //���M���̓������M���̓�����x�����邽��

    }       
    
}

void Usart_data(void){

        if(RC0==1){
            tx_data=tx_data | 0b00000001;
            //RC0�i�Z���j���I���Ȃ�tx_data��bit0���P�ɃZ�b�g
        }

        if(RC1==1){
            tx_data=tx_data | 0b00000010;
            //RC1�i�C���W�F�N�V�����j���I���Ȃ�tx_data��bit1���P�ɃZ�b�g
        }

        if(RC2==1){
            tx_data=tx_data | 0b00000100;
            //RC2�i�z�[���j���I���Ȃ�tx_data��bit2���P�ɃZ�b�g
        }

        if(RC3==1){
            tx_data=tx_data | 0b00001000;
            //RC3�i�蓮�N���b�`�j���I���Ȃ�tx_data��bit3���P�ɃZ�b�g
        }

        if(RC4==1){
            tx_data=tx_data | 0b00010000;
            //RC4�i�����N���b�`�j���I���Ȃ�tx_data��bit4���P�ɃZ�b�g
        }

}

void Bit4_data(void){

        //RC0�i�Z���j���I���Ȃ�tx_data��bit0���P�ɃZ�b�g
        if(RC0==1){
            RA2=1;
        }
        else{
            RA2=0;
        }

        //RC1�i�C�O�j�V�����j���I���Ȃ�tx_data��bit1���P�ɃZ�b�g
        if(RC1==1){
            RA3=1;
        }
        else{
            RA3=0;
        }

        //RC2�i�z�[���j���I���Ȃ�tx_data��bit2���P�ɃZ�b�g
        if(RC2==1){
            RA4=1;
        }
        else{
            RA4=0;
        }

        //RC3�i�蓮�N���b�`�j���I���Ȃ�tx_data��bit3���P�ɃZ�b�g
        if(RC3==1){
            RA5=1;
        }

        else{
            RA5=0;
        }

}