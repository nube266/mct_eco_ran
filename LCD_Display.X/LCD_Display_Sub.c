/*
 * File:   LCD_Display_Sub.c
 * Author: ECORUN 2017
 * Use PIC : PIC16F1938
 * プログラムの内容 : LCDにSeeddataを表示 *
 * Created on 2017/06/22, 23:31
 */


#include <xc.h>
#include "lcd.h"

void lcdBegin(PLCD lcd, BYTE cols, BYTE lines, BYTE charsize){
	if(BUS8BIT){
		lcd->DisplayFunction = LCD_FUNC_8BIT;
	}
	if(lines > 1){
		lcd->DisplayFunction |= LCD_FUNC_2LINE;
	}
	lcd->NumLines = lines;
	lcd->Cols = cols;
	if(charsize == LCD_FUNC_5x11DOT){
		lcd->DisplayFunction |= LCD_FUNC_5x11DOT;
	}

	lcd->RowOffsets[0] = 0x00;
	lcd->RowOffsets[1] = 0x40;
	lcd->RowOffsets[2] = 0x00 + cols;
	lcd->RowOffsets[3] = 0x40 + cols;

	// initialize sequence
	__delay_ms(50);

	RS = 0;
	ENABLE = 0;
	RW = 0;

	// first function set
	lcdCommand(LCD_FUNCTION_SET | lcd->DisplayFunction);
	__delay_us(4500);
	// second function set
	lcdCommand(LCD_FUNCTION_SET | lcd->DisplayFunction);
	__delay_us(200);
	// third function set
	lcdCommand(LCD_FUNCTION_SET | lcd->DisplayFunction);

	// finally function set
	lcdCommand(LCD_FUNCTION_SET | lcd->DisplayFunction);

	// display control, display on
	lcd->DisplayControl = LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR_OFF | LCD_DISPLAY_BLINK_OFF;
	lcdCommand(LCD_DISPLAY_CONTROL | lcd->DisplayControl);
	// clear
	lcdCommand(LCD_CLEAR_DISPLAY);
	__delay_us(2000);
	// entry mode
	lcd->DisplayMode = LCD_ENTRY_LEFTTORIGHT | LCD_ENTRY_SHIFT_DISABLE;
	lcdCommand(LCD_ENTRY_MODE_SET | lcd->DisplayMode);

	lcd->DisplayCursor = LCD_SHIFT_CURSOR | LCD_SHIFT_RIGHT;
}

void lcdClear(){
	lcdCommand(LCD_CLEAR_DISPLAY);
	__delay_us(2000);
}

void lcdHome(){
	lcdCommand(LCD_RETURN_HOME);
	__delay_us(2000);
}

void lcdSetCursor(PLCD lcd, BYTE col, BYTE line){
	if(line > 4){
		line = 4;
	}
	if(line > lcd->NumLines){
		line = lcd->NumLines;
	}

	lcdCommand(LCD_SET_DDRAM_ADDRESS | (col + lcd->RowOffsets[line]));
}

void lcdCreateChar(BYTE num, PBYTE character){
	int i = 0;

	lcdCommand(LCD_SET_CGRAM_ADDRESS | (num << 3));
	for(i = 0; i < 7; i++){
		lcdWrite((*(character + i)) & 0x1F);
	}
}

void lcdPrint(BYTE str[]){
	for(; *str != '\0'; str++){
		lcdWrite(*str);
	}
}

inline void lcdCommand(BYTE com){
	lcdSend(com, LOW);
}

inline void lcdWrite(BYTE val){
	lcdSend(val, HIGH);
}

void lcdSend(BYTE val, BYTE mode){
	ENABLE = 0;
	RS = mode;
	RW = 0;

	DB0 = (val & 0x01);
	DB1 = (val & 0x02) >> 1;
	DB2 = (val & 0x04) >> 2;
	DB3 = (val & 0x08) >> 3;
	DB4 = (val & 0x10) >> 4;
	DB5 = (val & 0x20) >> 5;
	DB6 = (val & 0x40) >> 6;
	DB7 = (val & 0x80) >> 7;

	lcdPulseEnable();
}

inline void lcdPulseEnable(){
	ENABLE = 0;
	__delay_us(1);
	ENABLE = 1;
	__delay_us(1);
	ENABLE = 0;
	__delay_us(100);
}





