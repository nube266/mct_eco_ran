/*
 * File:   define.h
 * Author: Daichi
 *
 * Created on 2016/11/03, 2:11
 */

#ifndef DEFINE_H
#define	DEFINE_H

#define _XTAL_FREQ	8000000	// 8MHz
#define period 0.1  //ˆêŽüŠú@‚OD‚P•b

// pin config
#define SEN1	RA3
#define SEN2	RA4
#define	SW1		RA6		// red
#define SW2		RA7		// blue

#define W0	0x20
#define W1	0x10
#define W2	0x11
#define W3	0x12
#define W4	0x13
#define W5	0x14

#define MIN(x,y)	((x>y)?(y):(x))

typedef unsigned char BYTE, *PBYTE;



#endif	/* DEFINE_H */