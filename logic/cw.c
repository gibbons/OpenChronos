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
// A scheme (8 bits, or 1 byte per character) that allows characters
// up to 7 signals long. -- gibbons
// 
// 0 = DOT, 1 = DASH
// There's one start bit (1) before the first signal (DOT or DASH)
//
// e.g.  C = -.-. = 0x1A = 0001 1010b
//                            ^ ^__ First DASH
//                            |____ Start bit
const u8 CW_Char[] =
{
  0x5E,	// ' .----.
  0x36,	// ( -.--.
  0x6D,	// ) -.--.-
  0x19,	// * -..-	(multiplication sign, same as 'x')
  0x2A,	// + .-.-.
  0x73,	// , --..--
  0x61,	// - -....-	(dash or minus sign)
  0x55,	// . .-.-.-	(period or full stop)
  0x32,	// / -..-.
  0x3F,	// 0 -----
  0x2F,	// 1 .----
  0x27,	// 2 ..---
  0x23,	// 3 ...--
  0x21,	// 4 ....-
  0x20,	// 5 .....
  0x30,	// 6 -....
  0x38,	// 7 --...
  0x3C,	// 8 ---..
  0x3E,	// 9 ----.
  0x78,	// : ---...
  0x00,	// ; undefined
  0x00,	// < undefined 
  0x31,	// = -...-
  0x00,	// > undefined
  0x4C,	// ? ..--..
  0x5A,	// @ .--.-.
  0x05,	// A .-
  0x18,	// B -...
  0x1A,	// C -.-.
  0x0C,	// D -..
  0x02,	// E .
  0x12,	// F ..-.
  0x0E,	// G --.
  0x10,	// H ....
  0x04,	// I ..
  0x17, // J .---
  0x0D,	// K -.-
  0x14,	// L .-..
  0x07,	// M --
  0x06,	// N -.
  0x0F,	// O ---
  0x16,	// P .--.
  0x1D,	// Q --.-
  0x0A,	// R .-.
  0x08,	// S ...
  0x03,	// T -
  0x09,	// U ..-
  0x11,	// V ...-
  0x0B,	// W .--
  0x19,	// X -..-
  0x1B,	// Y -.--
  0x1C	// Z --..
};

// *************************************************************************************************
// Extern section
extern void idle_loop(void); // in ezchronos.c


// *************************************************************************************************
// @fn          CW_Send_Char
// @brief       Send (via the buzzer) an alphanumeric character ("letter")
// @param       letter		character to send, in range 39 (''') to 90 ('Z'), inclusive
// @return      none
// *************************************************************************************************
void CW_Send_Char(u8 letter)
{
    
    if ((letter >= 39) && (letter <= 90)) { // In range
	letter = CW_Char[letter - 39]; // Get first "letter"
	//gibbons TODO: check if '=' operator or memcpy(...) is more appropriate
    }
    else if (letter == ' ') { // Send space (inter-word pause)
	Timer0_A4_Delay(CONV_MS_TO_TICKS(CW_WORD_PAUSE * CW_DOT_LENGTH));
	return;
    }
    else { // Invalid character
	return;
    }
    
    int i = 0x80; // 0x80 = 1000 0000b
    while (i > letter) i >>= 1;
    while (i > 0) {
	if (i & letter) { // Send dash (and pause after it)
	    start_buzzer(1, CONV_MS_TO_TICKS(3*CW_DOT_LENGTH), CONV_MS_TO_TICKS(CW_SIGNAL_PAUSE * CW_DOT_LENGTH));
	}
	else { // Send dot (and pause after it)
	    start_buzzer(1, CONV_MS_TO_TICKS(CW_DOT_LENGTH), CONV_MS_TO_TICKS(CW_SIGNAL_PAUSE * CW_DOT_LENGTH));
	}
	i >>= 1; // Next loop iteration will send the bit to the right
	
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
// @brief       Send (via the buzzer) a null ('\0') terminated string
// @param       str	string to send; characters in range 39 (''') to 90 ('Z'), inclusive
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
// @brief       Send (via the buzzer) a 4 character test string;
//			character being sent is shown on line 2, segment 0
// @param       set	Set number between 0 and 12, inclusive.
// @return      none
// *************************************************************************************************
void CW_Send_Test(u8 set)
{
    u8 i, letter;
    if (set > 12) set = 12; // Bounds check

    letter = set * 4 + 39; // gibbons TODO: more efficient to write letter = set << 2; ?

    for (i = 0; i < 4; i++) {
	display_char(LCD_SEG_L2_0, (letter + i), SEG_ON); // Show character currently being sent
	CW_Send_Char(letter + i);
    };
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
    if (++CW_Test_Set_Index > 12) CW_Test_Set_Index = 0;
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
