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
// 
// In this code, the word "signal" refers to a single Dot (short) or Dash (long). One or more signals
// make up a "letter" (sometimes refered to as a "character"), which can be a normal letter (A-Z) or
// a number (0-9). "Words" are strings of letters, separated by the space character (' '), hence
// transmitting the space character will send a pause of length "Pause between Words".
// 
// Lengths:
// DOT				Defined (in ms) as CW_DOT_LENGTH (in cw.h)
// DASH				3 * CW_DOT_LENGTH
// Pause between Signals	1 * CW_DOT_LENGTH
// Pause between Letters	3 * CW_DOT_LENGTH
// Pause between Words		7 * CW_DOT_LENGTH


// *************************************************************************************************
// Include section
// system
#include "project.h"

// driver
#include "timer.h"
#include "buzzer.h"
#include "display.h"

// logic
#include "cw.h"


// *************************************************************************************************
// Prototypes section


// *************************************************************************************************
// Global Variable section
u8 CW_Test_Set_Index = 0;

// Signal patterns for CW Letters
//
// The first 5 bits (bits 7-3) encode the signal (1=dash, 0=dot), the last three
// bits (bits 2-0) encode the number of signals, e.g.:
// C = 0xA4 = 10100100
// 	Signal Data = 10100
// 	Length= 100 = 4
// 	Signal = 1010 = Dash Dot Dash Dot
//
// This rather compact encoding scheme only allows for letters up to 5 signals long.
// If anyone is interested, I've also come up with an alternate scheme (still 8 bits
// per letter) that allows letters up to 7 signals long. -- gibbons
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
extern void idle_loop(void); // in ezchronos.c


// *************************************************************************************************
// @fn          CW_Send_Char
// @brief       Send (via the buzzer) an alphanumeric character ("letter")
// @param       letter		character to send, in range '0' to '9' or 'A' to 'Z', inclusive
// @return      none
// *************************************************************************************************
void CW_Send_Char(u8 letter)
{
    u8 data_mask = 0x80; // Signal data mask
    if ((letter >= '0') && (letter <= '9')) { // Number
	letter = CW_Char[letter - '0']; // Get first "letter"
    }
    else if ((letter >= 'A') && (letter <= 'Z')) { // Alphabetic Letter
	letter = CW_Char[letter - 'A' + 10]; // +10 to offset over the number section
    }
    else if (letter == ' ') { // Send space (inter-word pause)
	Timer0_A4_Delay(CONV_MS_TO_TICKS(CW_WORD_PAUSE * CW_DOT_LENGTH));
	return;
    }
    else { // Invalid character
	return;
    }
    
    // letter in the next loop doubles as both the loop counter and the signal data
    while (letter-- & CW_LENGTH_MASK) {
	if (letter & data_mask) { // Send dash (and pause after it)
	    start_buzzer(1, CONV_MS_TO_TICKS(3*CW_DOT_LENGTH), CONV_MS_TO_TICKS(CW_SIGNAL_PAUSE * CW_DOT_LENGTH));
	}
	else { // Send dot (and pause after it)
	    start_buzzer(1, CONV_MS_TO_TICKS(CW_DOT_LENGTH), CONV_MS_TO_TICKS(CW_SIGNAL_PAUSE * CW_DOT_LENGTH));
	}
	data_mask >>= 1; // Next loop iteration will send the bit to the right
	
	// Wait until finished buzzing
	while (is_buzzer()) {
	    Timer0_A4_Delay(CONV_MS_TO_TICKS(2*CW_DOT_LENGTH)); // Go into LPM3
	}
    }
    
    // Now send inter-letter pause (space between successive letters)
    Timer0_A4_Delay(CONV_MS_TO_TICKS(CW_LETTER_PAUSE * CW_DOT_LENGTH));
}


// *************************************************************************************************
// @fn          CW_Send_String
// @brief       Send (via the buzzer) an alphanumeric, null ('\0') terminated string
// @param       str	string to send; characters in range '0' to '9' or 'A' to 'Z', inclusive
//			Must be terminated with a null character!
// @return      none
// *************************************************************************************************
void CW_Send_String(u8 * str)
{
    u8 i = 0;
    while (*(str + i) != '\0') { // Loop over string elements until null character ('\0') found
	CW_Send_Char(*(str + i));
	i++;
    }
}


// *************************************************************************************************
// @fn          CW_Send_Test
// @brief       Send (via the buzzer) an alphanumeric test string; character being sent is shown on
//			line 2, segment 0
// @param       set	Set number between 0 and 6, inclusive.
//			0 --> "01234"
//			1 --> "56789"
//			2 --> "ABCDEF"
//			3 --> "GHIJK"
//			4 --> "LMNOP"
//			5 --> "QRSTU"
//			6 --> "VWXYZ"
// @return      none
// *************************************************************************************************
void CW_Send_Test(u8 set)
{
    u8 i = 0;
    u8 letter;
    u8 set_size = 5;
    if (set >= 7) set = 6; // Bounds check

    letter = set * 5;
    if (letter > 9) { // alphabetic letter
	letter += 'A' - 10;
	if (set == 2) {
	    set_size++; //Set 2 == "ABCDEF" (six letters, instead of normal five)
	}
	else {
	    letter++; // To account for shift due to set 2 having six letters
	}
    }
    else { // numeric "letter"
	letter += '0';
    }
    
    do {
	display_char(LCD_SEG_L2_0, (letter + i), SEG_ON); // Show character currently being sent
	CW_Send_Char(letter + i);
    } while (++i < set_size);
}


// *************************************************************************************************
// @fn          sx_cw
// @brief       Send and cycle through test strings.
// @param       line	Line number
// @return      none
// *************************************************************************************************
void sx_cw(u8 line)
{
    CW_Send_Test(CW_Test_Set_Index);
    if (++CW_Test_Set_Index >= 7) CW_Test_Set_Index = 0;
}


// *************************************************************************************************
// @fn          display_cw
// @brief       Update display
// @param       line	Line number; this function assumes line 2, regardless of this argument
//		mode	DISPLAY_LINE_CLEAR or DISPLAY_LINE_UPDATE_FULL
// @return      none
// *************************************************************************************************
void display_cw(u8 line, u8 mode)
{
    if (mode == DISPLAY_LINE_UPDATE_FULL) display_chars(LCD_SEG_L2_3_2, "CW", SEG_ON);
    else if (mode == DISPLAY_LINE_CLEAR) CW_Test_Set_Index = 0; // Assume we're leaving the CW menu: reset index
}
