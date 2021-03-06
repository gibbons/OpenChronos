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

#ifndef CW_H_
#define CW_H_

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



// Defines Section
// ******************************************************************************
// Length of a "dot" in ms; three times this length for a dash
#define CW_DOT_LENGTH		(50u)

// For masking off the letter length in cw_char[]
#define CW_LENGTH_MASK		(0x07)

// Pause lengths, in units of CW_DOT_LENGTH's
#define CW_SIGNAL_PAUSE		(1u)
#define CW_LETTER_PAUSE		(3u)
#define CW_WORD_PAUSE		(7u)

// Prototypes Section
// ******************************************************************************
extern void CW_Send_Char(u8 letter);
extern void CW_Send_String(u8 * word);

extern void sx_cw(u8 line);
extern void display_cw(u8 line, u8 mode);	

extern const u8 CW_Char[];

#endif