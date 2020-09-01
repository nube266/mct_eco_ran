/*
 * File:   LCD_Display_Program.c
 * Author: ECORUN 2017
 * Use PIC : PIC16F1938
 * �v���O�����̓��e : LCD��Seeddata��\��
 * Created on 2017/06/22, 23:24
 */

#include <xc.h>
#include "define.h"
#include "lcd.h"
#include <string.h>
#include<stdio.h>

// config1
#pragma config FCMEN = OFF		// �O���N���b�N�Ď����Ȃ�
#pragma config IESO = OFF		// 2�i���N���b�N�X�^�[�g�A�b�v���[�h�g�p���Ȃ�
#pragma config CLKOUTEN = OFF	// �N���b�N�o�͂��g�p���Ȃ�
#pragma config BOREN = ON		// �u���E���A�E�g���Z�b�g���g�p����
#pragma config CPD = OFF		// �f�[�^��������ی삵�Ȃ�
#pragma config CP = OFF			// �v���O������������ی삵�Ȃ�
#pragma config MCLRE = OFF		// �O�����Z�b�g�M�����g�p���Ȃ�
#pragma config PWRTE = ON		// �d��ON����64ms��Ƀv���O�����J�n
#pragma config WDTE = OFF		// �E�H�b�`�h�b�O�^�C�}����
#pragma config FOSC = INTOSC	// �����N���b�N���g�p����

// config2
#pragma config LVP = OFF		// ��d���v���O���~���O����
#pragma config BORV = HI		// �u���E���A�E�g���Z�b�g�d��2.7V
#pragma config STVREN = ON		// �X�^�b�N���I�[�o�[�t���[��A���_�[�t���[�����烊�Z�b�g
#pragma config PLLEN = OFF		// 32MHz�œ��삳���Ȃ�(4xPLL����)
#pragma config VCAPEN = OFF		// VCAP����
#pragma config WRT = OFF		// Flash��������ی삵�Ȃ�

//�֐��̃v���g�^�C�v�錾
void initializePort( void );    //�|�[�g�ݒ�̊֐�
void read(void);                 //�󂯎�������l��\�����镶����ɕϊ�����֐�

int i=0;  //for���p
int a=0x30,b=0x30,c=0x30;           //��M�f�[�^����U�i�[���邽�߂̕ϐ�
int num1=0,num2=0,num3=0;//���l�i�[�p
char moji1[]="para"; //���l�𕶎���ɕϊ������Ƃ��Ɋi�[����p
char moji2[]="para"; //���l�𕶎���ɕϊ������Ƃ��Ɋi�[����p
char moji3[]="para"; //���l�𕶎���ɕϊ������Ƃ��Ɋi�[����p
char rx_flag=0; //��M��ԂɊւ���t���O    �P�Ȃ�Ύ�M�f�[�^��RCREG�ɂ���
                //                         �O�Ȃ�Ύ�M�f�[�^��RCREG�ɂȂ�
int rx_data;    //���M����f�[�^���i�[����


void interrupt ISR(void){

    if(RCIF==1){//���荞�݂�USART�ʐM�̎�M�Ȃ��

        if(OERR==1){        //�I�[�o�[�����G���[�����������ꍇ
            RCSTA=0;        //�ݒ���N���A
            RCSTA=0x90;
        }
        else{               //�G���[���Ȃ����
            rx_data=RCREG;  //��M�f�[�^��rx_data�Ɋi�[
            rx_flag=1;      //�f�[�^��M�������̂Ńt���O�����Ă�
            RCIF=0;         //���荞�݃t���O�����Z�b�g
        }

    }

}

void main (void){

    OSCCON = 0b01110010;
    /* �����I�V���[�^ 8MHz PLL OFF
       bit7 PPL ON OFF bit6-3 �����N���b�N�̑I���@�N���b�N�I��*/

    //�V���A���ʐM�Ɋւ���ݒ�
    RCSTA=0x90;
    /*�V���A���|�[�g�͗L���@8bit��M�@�C�l�[�u���r�b�gON�@
      ��M�f�[�^��9bit�ڂ̓f�[�^���p���e�B�r�b�g        */
    SYNC=0;
    BRG16=0;    //8bit�{�[���[�g�W�F�l���[�^���g�p
    BRGH=1;     //�񓯊����[�h�ō���
    SPBRG=51;   //�{�[���[�g9600bps


    //���荞�݂Ɋւ���ݒ�

    RCIF=0;     //USART�̊��荞�݃t���O��������
    RCIE=1;     //USART�̊��荞�ݎ�M��L��
    PEIE=1;     //���ӑ��u���荞�݂�L��
    GIE=1;      //�S���荞�ݏ�����������


    initializePort();   //�e�|�[�g�̐ݒ�


    LCD lcd;    //�@LCD�@�\����

    lcdBegin(&lcd, 16, 2, 0x00);    //LCD�̏�����

    while(1){   //�������[�v

        for(i=0;i<4;i++){
            while(!rx_flag);                //��M����܂ő҂�
            rx_flag=0;                      //�t���O�����Z�b�g
            if(i==0&&rx_data!=0xFF) i--;    //�擪�̏�񂪂���܂�i�����Z
            else if(i==1) a=rx_data;        //�ꌅ�ڂ̐��l���󂯎��
            else if(i==2) b=rx_data;        //�񌅖ڂ̐��l���󂯎��
            else if(i==3) c=rx_data;        //�O���ڂ̐��l���󂯎��
        }

        lcdClear(); //LCD�̕\�����N���A����
        read();     //���l�𕶎���ɕϊ�����

        lcdSetCursor(&lcd, 0, 0);   //lcd�̈�s�ڂɋL�q
        lcdPrint(moji1);            //�����̕\��
        lcdSetCursor(&lcd, 0, 1);   //lcd�̈�s�ڂɋL�q
        lcdPrint("km/h");           //�����̕\��

    }

}



void read(void){

    num1=a-0x30;  //����l���A�X�L�[����ꌅ�̐��l�ɕϊ�
    num2=b-0x30;  //����l���A�X�L�[����ꌅ�̐��l�ɕϊ�
    num3=c-0x30;  //����l���A�X�L�[����ꌅ�̐��l�ɕϊ�

    //������֕ϊ�����
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

    //strcat�֐��͑������̌��ɑ�������ϊ�����֐�
//     strcat(&moji1,&moji2);  //������̌���
//      strcat(moji1,".");     //������̌���
//      strcat(moji1,moji3);   //������̌���
//      strcat(moji1,"km/h");  //������̌���

     strcat(moji1,moji2);      //������̌���
     strcat(moji1,".");        //������̌���
     strcat(moji1,moji3);      //������̌���

}




void initializePort(void){

    PORTA = 0x00;   //���ׂď�����
    PORTB = 0x00;   //���ׂď�����
    PORTC = 0x00;   //���ׂď�����
    /*�r�b�g 7-0	RX<7:0>: PORTX I/O �l�̃r�b�g*/


    ANSELA = 0x00;  //���ׂăf�W�^���s��
    ANSELB = 0x00;  //���ׂăf�W�^���s��
    /*  �r�b�g 7-6	������ :�u0�v�Ƃ��ēǂݏo��
        �r�b�g 5-0	ANSX<5:0>: RB<5:0> �̊e�s���̋@�\���A�i���O�܂��̓f�W�^���̂����ꂩ�ɑI�����܂��B
        0 = �f�W�^�� I/O�B�s���̓|�[�g�܂��̓f�W�^������@�\�Ɋ��蓖�Ă���
        1 = �A�i���O���́B�s���̓A�i���O���͂Ƃ��Ċ��蓖�Ă���                                     */

    TRISA = 0x00;   //���ׂďo��
    TRISB = 0x00;   //���ׂďo��
    TRISC = 0x80;   //RC7(RX�s��)����́@���̑����o��
    /*  �r�b�g 7-0	TRISX<7:0>: PORTX �g���C�X�e�[�g����r�b�g
        1 = PORTX �s�������͂Ƃ��Đݒ肳��Ă��� ( �g���C�X�e�[�g )
        0 = PORTX �s�����o�͂Ƃ��Đݒ肳��Ă���                    */

}

