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

#ifndef ALARMQUEUE_H_
#define ALARMQUEUE_H_

// *************************************************************************************************
// 
// Alarm queue notes here



// Defines Section
// ******************************************************************************

#define AQ_DOW_EN	(BIT2)
#define AQ_DAY_EN	(BIT3)
#define AQ_MONTH_EN	(BIT4)
#define AQ_YEAR_EN	(BIT5)

#define AQ_REPEAT	(BIT7)


// Global variable section
struct alarm_job {
    u8	minute;
    u8	hour;
    u8	day; // For day or day-of-week (dow)
    u8	month; // NOTE: month and year not natively supported by RTC alarm function
    u16	year; // gibbons TODO: really need this?
    
    u8	config; // Bit field of config values (see defines above)
    
    alarm_job* next; // For the linked-list of alarm jobs (sorted chronologically)
};


// Prototypes Section
// ******************************************************************************
extern void CW_Send_Char(u8 letter);
extern void CW_Send_String(u8 * word);

extern void sx_cw(u8 line);
extern void display_cw(u8 line, u8 mode);	

extern const u8 CW_Char[];

#endif