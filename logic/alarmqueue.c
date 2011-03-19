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
// Alarm queue module
// *************************************************************************************************
// 
// Alarm queue notes here


// *************************************************************************************************
// Include section
// system
#include "project.h"

// driver

// logic


// *************************************************************************************************
// Prototypes section


// *************************************************************************************************
// Global Variable section


// *************************************************************************************************
// Extern section
//extern void idle_loop(void); // in ezchronos.c


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
