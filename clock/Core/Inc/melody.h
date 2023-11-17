/*
 * melody.h
 *
 *  Created on: Nov 17, 2023
 *      Author: okeunji
 */

#ifndef INC_MELODY_H_
#define INC_MELODY_H_

#include "stdint.h"
#include "main.h"

#define B0  31
#define C1  33
#define CS1 35
#define D1  37
#define DS1 39
#define E1  41
#define F1  44
#define FS1 46
#define G1  49
#define GS1 52
#define A1  55
#define AS1 58
#define B1  62
#define C2  65
#define CS2 69
#define D2  73
#define DS2 78
#define E2  82
#define F2  87
#define FS2 93
#define G2  98
#define GS2 104
#define A2  110
#define AS2 117
#define B2  123
#define C3  131
#define CS3 139
#define D3  147
#define DS3 156
#define E3  165
#define F3  175
#define FS3 185
#define G3  196
#define GS3 208
#define A3  220
#define AS3 233
#define B3  247
#define C4  262
#define CS4 277
#define D4  294
#define DS4 311
#define E4  330
#define F4  349
#define FS4 370
#define G4  392
#define GS4 415
#define A4  440
#define AS4 466
#define B4  494
#define C5  523
#define CS5 554
#define D5  587
#define DS5 622
#define E5  659
#define F5  698
#define FS5 740
#define G5  784
#define GS5 831
#define A5  880
#define AS5 932
#define B5  988
#define C6  1047
#define CS6 1109
#define D6  1175
#define DS6 1245
#define E6  1319
#define F6  1397
#define FS6 1480
#define G6  1568
#define GS6 1661
#define A6  1760
#define AS6 1865
#define B6  1976
#define C7  2093
#define CS7 2217
#define D7  2349
#define DS7 2489
#define E7  2637
#define F7  2794
#define FS7 2960
#define G7  3136
#define GS7 3322
#define A7  3520
#define AS7 3729
#define B7  3951
#define C8  4186
#define CS8 4435
#define D8  4699
#define DS8 4978
#define REST      0

typedef struct{
	uint16_t freq;
	uint16_t delay;
}_BUZZER;

_BUZZER *buzzer;
_BUZZER bell[64] = {
		{G4, 4}, {G4, 4}, {A4, 4}, {A4, 4}, // 4
		{G4, 4}, {G4, 4}, {E4, 8}, // 3
		{G4, 4}, {G4, 4}, {E4, 4}, {E4, 4}, {D4, 12}, // 5
		{G4, 4}, {G4, 4}, {A4, 4}, {A4, 4}, // 4
		{G4, 4}, {G4, 4}, {E4, 8}, // 3
		{G4, 4}, {E4, 4}, {D4, 4}, {E4, 4}, {C4, 12} // 5
};

_BUZZER harry[64] = {
    {REST, 2},
    {D4, 4},
    {G4, 4}, {AS4, 6}, {A4, 4},
    {G4, 2}, {D5, 4},
    {C5, 2},
    {A4, 2},
    {G4, 4}, {AS4, 6}, {A4, 4},
    {F4, 2}, {GS4, 4},
    {D4, 6},
    {D4, 4},

    {G4, 4}, {AS4, 6}, {A4, 4},
    {G4, 2}, {D5, 4},
    {F5, 1}, {E5, 4},
    {DS5, 2}, {B4, 4},
    {DS5, 4}, {D5, 6}, {CS5, 4},
    {CS4, 2}, {B4, 4},
    {G4, 6},
    {AS4, 4}, //32

    {D5, 2}, {AS4, 4},
    {D5, 2}, {AS4, 4},
    {DS5, 2}, {D5, 4},
    {CS5, 2}, {A4, 4},
    {AS4, 2}, {D5, 4}, {CS5, 4},
    {CS4, 2}, {D4, 4},
    {D5, 1},
    {REST, 4}, {AS4, 4}, //48

    {D5, 2}, {AS4, 4},
    {D5, 2}, {AS4, 4},
    {F5, 2}, {E5, 4},
    {DS5, 2}, {B4, 4},
    {DS5, 4}, {D5, 6}, {CS5, 4},
    {CS4, 2}, {AS4, 4},
    {G4, 1}
};



uint8_t seq = 0;
uint8_t stop = 0;

#endif /* INC_MELODY_H_ */
