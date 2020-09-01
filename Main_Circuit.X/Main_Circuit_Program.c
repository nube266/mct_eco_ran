/*
 * File : Main_Circuit_Program.c
 * Author : ECORUN 2017
 * Use PIC : PIC16F1938
 * �v���O�������e : Main_Circuit����
 * Created on 2017/06/22, 22:51
 */


#include<xc.h>

#define _XTAL_FREQ  8000000     /* delay�p(�N���b�N8MHz�œ��쎞) */

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

//�ϐ��̒�`
int rx_data;/*���M����f�[�^���i�[����
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
int rx_cel = 0x00;  //�Z�����[�^����֐�
int rx_ign = 0x00;  //�C�O�j�b�V��������֐�
int rx_hor = 0x00;  //�z�[������֐�
int rx_ser = 0x00;  //�T�[�{����֐�
char rx_flag=0; //��M��ԂɊւ���t���O    �P�Ȃ�Ύ�M�f�[�^��RCREG�ɂ���
                //                         �O�Ȃ�Ύ�M�f�[�^��RCREG�ɂȂ�


void interrupt Interreceive(void){

    if(RCIF==1){//���荞�݂�USART�ʐM�̎�M�Ȃ��

        if(OERR==1){        //�I�[�o�[�����G���[�����������ꍇ
            RCSTA=0;
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

    initializePort();   //�e�|�[�g�̐ݒ�

    //�V���A���ʐM�Ɋւ���ݒ�
    RCSTA=0x90;
    /*�V���A���|�[�g�͗L���@8bit��M�@�C�l�[�u���r�b�gON�@
      ��M�f�[�^��9bit�ڂ̓f�[�^���p���e�B�r�b�g        */
    SYNC=0;
    BRG16=0;    //8bit�{�[���[�g�W�F�l���[�^���g�p
    BRGH=1;     //�񓯊����[�h�ō���
    SPBRG=51;  //�{�[���[�g9600bps

    //��M���荞�݂Ɋւ���ݒ�
    RCIF=0; //USART�̊��荞�݃t���O��������
    RCIE=1; //USART�̊��荞�ݎ�M��L��
    PEIE=1; //���ӑ��u���荞�݂�L��
    GIE=1;  //�S���荞�ݏ�����������


    __delay_ms(10);

    while(1){   //�������[�v
        //�V���A���ʐM�ɂ��f�[�^�̎擾
        if(rx_flag==1){
            
            //�eSW�̏�Ԋm�F & 1bit�ڂɃf�[�^���V�t�g
            rx_cel = rx_data & 0x01;  //cel_SW�̏�Ԃ���            
            rx_ign = (rx_data & 0x02) >> 1;  //ign_SW�̏�Ԃ���            
            rx_hor = (rx_data & 0x04) >> 2;  //hor_SW�̏�Ԃ���            
            rx_ser = (rx_data & 0x08) >> 3;  //ser_SW�̏�Ԃ���
            
            
            //����w�� (2�d�m�F)
            if(rx_hor == 1 && RC2 == 1){  //�z�[��SW ON
                LATAbits.LATA1 = 1;       
            }
            else{                         //�z�[��SW OFF
                LATAbits.LATA1 = 0; 
            }
            
            if(rx_ser == 1 && RC3 == 1){  //�T�[�{SW ON
                LATAbits.LATA4 = 1;
            }
            else{                         //�T�[�{SW OFF
                LATAbits.LATA4 = 0; 
            }
            
            if(rx_ign == 1 && RC1 == 1){  //�C�O�j�b�V����SW ON
                LATBbits.LATB0 = 1;
            }
            else{                         //�C�O�j�b�V����SW OFF
                LATBbits.LATB0 = 0; 
            } 
            
            if(rx_cel == 1 && RC0 == 1){  //�Z�����[�^SW ON
                LATBbits.LATB1 = 1;
            }
            else{                         //�Z�����[�^SW OFF
                LATBbits.LATB1 = 0; 
            }            
            
            rx_data = 0x00;  //�f�[�^�N���A
            rx_flag=0;       //�t���O�N���A
        }
        
        
        if(rx_ign == 0){                 // �G���W����~ >> �d���o���u ON 
            LATAbits.LATA0 = 1;
        }
        else{                            // �d���o���u OFF
            LATAbits.LATA0 = 0;
        }
    }
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


    TRISA = 0x00;   
    /*RA0 : �\���m�C�h0 Signal OUTPUT
      RA1 : �z�[�� Signal OUTPUT
      RA2 : �\���m�C�h1 Signal OUTPUT (���g�p)
      RA3 : �\���m�C�h2 Signal OUTPUT (���g�p)
      RA4 : SERVO Signal OUTPUT
      RA5 : �G���R�[�_ (���g�p)
      RA6 : OPEN
      RA7 : OPEN
     */
    TRISB = 0x00;   
    /*RB0 : �C�O�j�b�V���� Signal OUTPUT
      RB1 : �Z�����[�^�@Signal OUTPUT
      RB2 : �����[ Signal OUTPUT (���g�p)
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
    /*  �r�b�g 7-0	TRISX<7:0>: PORTX �g���C�X�e�[�g����r�b�g
        1 = PORTX �s�������͂Ƃ��Đݒ肳��Ă��� ( �g���C�X�e�[�g )
        0 = PORTX �s�����o�͂Ƃ��Đݒ肳��Ă���                    */

}
