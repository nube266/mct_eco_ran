#ifndef LCD_H
#define	LCD_H

#include <xc.h>
#include "define.h"

// bus config
#define BUS8BIT	1		// 1:8bit bus, 0:4bit bus

// pin config
#define RS		LATA0	// 0:command, 1:character
#define RW		LATA1	// 0:write, 1:read
#define ENABLE	LATA2	// activated by a HIGH pulse.
#define DB0		LATB7
#define DB1		LATB6
#define DB2		LATB5
#define DB3		LATB4
#define DB4		LATB3
#define DB5		LATB2
#define DB6		LATB1
#define DB7		LATB0

// commands
#define LCD_CLEAR_DISPLAY	0x01
#define LCD_RETURN_HOME		0x02
#define LCD_ENTRY_MODE_SET	0x04
#define LCD_DISPLAY_CONTROL	0x08
#define LCD_CURSOR_SHIFT	0x10
#define LCD_FUNCTION_SET	0x20
#define LCD_SET_CGRAM_ADDRESS	0x40
#define LCD_SET_DDRAM_ADDRESS	0x80
// options of Entry Mode Set
#define LCD_ENTRY_RIGHTTOLEFT	0x00
#define LCD_ENTRY_LEFTTORIGHT	0x02
#define LCD_ENTRY_SHIFT_ENABLE	0x01
#define LCD_ENTRY_SHIFT_DISABLE	0x00
// options of Display on/off Control
#define LCD_DISPLAY_ON			0x04
#define LCD_DISPLAY_OFF			0x00
#define LCD_DISPLAY_CURSOR_ON	0x02
#define LCD_DISPLAY_CURSOR_OFF	0x00
#define LCD_DISPLAY_BLINK_ON	0x01
#define LCD_DISPLAY_BLINK_OFF	0x00
// options of Cursor or Display Shift
#define LCD_SHIFT_DISPLAY	0x08
#define LCD_SHIFT_CURSOR	0x00
#define LCD_SHIFT_RIGHT		0x04
#define LCD_SHIFT_LEFT		0x00
// options of Function Set
#define LCD_FUNC_8BIT		0x10
#define LCD_FUNC_4BIT		0x00
#define LCD_FUNC_2LINE		0x08
#define LCD_FUNC_1LINE		0x00
#define LCD_FUNC_5x11DOT	0x04
#define LCD_FUNC_5x8DOT		0x00

#define HIGH	1
#define LOW		0

typedef struct _Lcd {
	BYTE DisplayFunction;
	BYTE DisplayControl;
	BYTE DisplayMode;
	BYTE DisplayCursor;

	BYTE NumLines;
	BYTE Cols;

	BYTE RowOffsets[4];
} LCD, *PLCD;

void lcdBegin(PLCD lcd, BYTE cols, BYTE lines, BYTE charsize);

void lcdClear();
void lcdHome();

void lcdSetCursor(PLCD lcd, BYTE col, BYTE line);

void lcdCreateChar(BYTE num, PBYTE character);

void lcdPrint(BYTE str[]);

inline void lcdCommand(BYTE com);
inline void lcdWrite(BYTE val);
void lcdSend(BYTE val, BYTE mode);
inline void lcdPulseEnable();

#endif	/* LCD_H */


