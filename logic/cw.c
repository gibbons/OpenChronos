// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
//	 
//	 
//	  Redistribution and use in source and binary forms, with or without 
//	  modification, are permitted provided that the following conditions 
//	  are met:
//	
//	    Redistributions of source code must retain the above copyright 
//	    notice, this list of conditions and the following disclaimer.
//	 
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the 
//	    documentation and/or other materials provided with the   
//	    distribution.
//	 
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//	
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// CW (Morse Code) module routines.
// *************************************************************************************************

// *************************************************************************************************
// Include section
// system
#include "project.h"

// driver
#include "timer.h"
#include "buzzer.h"

// logic
#include "cw.h"


// *************************************************************************************************
// Prototypes section
//void CW_Send_Char(u8 letter);
//void CW_Send_Str(u8 * word);

// *************************************************************************************************
// Global Variable section

// Signal patterns for CW numbers and letters
//
// The first 5 bits (bits 8-4) encode the signal (1=dash, 0=dot), the last three
// bits (bits 3-1) encode the number of signals, e.g.:
// C = 0xA4 = 10100100
// 	Signal Data = 10100
// 	Length= 100 = 4
// 	Signal = 1010 = Dash Dot Dash Dot

const u8 CW_Char[] =
{
  0xFD,	// 0
  0x7D,	// 1
  0x3D,	// 2
  0x1D,	// 3
  0x0D,	// 4
  0x05,	// 5
  0x85,	// 6
  0xC5,	// 7
  0xE5,	// 8
  0xF5,	// 9
  0x42,	// A
  0x84,	// B
  0xA4,	// C
  0x83,	// D
  0x01,	// E
  0x24,	// F
  0xC3,	// G
  0x04,	// H
  0x02,	// I
  0x74, // J
  0xA3,	// K
  0x44,	// L
  0xC2,	// M
  0x82,	// N
  0xE3,	// O
  0x64,	// P
  0xD4,	// Q
  0x43,	// R
  0x03,	// S
  0x81,	// T
  0x23,	// U
  0x14,	// V
  0x63,	// W
  0x94,	// X
  0xB4,	// Y
  0xC4	// Z
};

// *************************************************************************************************
// Extern section
extern void idle_loop(void);


// *************************************************************************************************
// @fn          RTC_A_ISR
// @brief       IRQ handler for RTC Module
// @param       none
// @return      none
// *************************************************************************************************

void CW_Send_Char(u8 letter)
{
    u8 mask = 0x80;
    if ((letter >= '0') && (letter <= '9')) { // Number
	letter = CW_Char[letter - '0'];
    }
    else if ((letter >= 'A') && (letter <= 'Z')) { // Alphabetic Letter
	letter = CW_Char[letter - 'A' + 10]; // +10 to offset over the number section
    }
    else if (letter == ' ') { // Send space (inter-word pause)
	Timer0_A4_Delay(CONV_MS_TO_TICKS(CW_WORD_PAUSE * CW_DOT_LENGTH));
	return;
    }
    else {
	return;
    }
    
    while (letter-- & CW_LENGTH_MASK) {
	if (letter & mask) { // Send dash
	    start_buzzer(1, CONV_MS_TO_TICKS(3*CW_DOT_LENGTH), CONV_MS_TO_TICKS(CW_SIGNAL_PAUSE * CW_DOT_LENGTH));
	}
	else { // Send dot
	    start_buzzer(1, CONV_MS_TO_TICKS(CW_DOT_LENGTH), CONV_MS_TO_TICKS(CW_SIGNAL_PAUSE * CW_DOT_LENGTH));
	}
	mask >>= 1;
	// Wait until finished buzzing
	while (is_buzzer()) {
	    idle_loop(); // Go into LPM3
	}
	
	// Now send inter-signal pause (space between successive dots and dashs in the letter)
	//Timer0_A4_Delay(CONV_MS_TO_TICKS(CW_SIGNAL_PAUSE));
    }
    
    // Now send inter-letter pause (space between successive letters)
    Timer0_A4_Delay(CONV_MS_TO_TICKS(CW_LETTER_PAUSE * CW_DOT_LENGTH));
    
}


void CW_Send_String(u8 * word)
{
    u8 i = 0;
    while (*(word + i) != '\0') {
	CW_Send_Char(*(word + i));
	i++;
    }
}
