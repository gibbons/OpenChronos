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

#ifndef TILTSET_H_
#define TILTSET_H_


// Defines Section
// ******************************************************************************
// States returned from tiltset_get_state()
#define TILTSET_UP_LARGE	(1u)
#define TILTSET_UP_SMALL	(2u)
#define TILTSET_LEVEL		(3u)
#define TILTSET_DOWN_SMALL	(4u)
#define TILTSET_DOWN_LARGE	(5u)
#define TILTSET_LEFT_TIP	(6u)
#define TILTSET_RIGHT_TIP	(7u)

// For internal use
#define TILTSET_ARMED_LEFT	(10u)
#define TILTSET_ARMED_RIGHT	(11u)

// Thresholds, in raw sensor units
#define TILTSET_UD_MIN_THRESHOLD	(0x13)		//0x13 raw sensor units = 340 milli-g's (20 degrees)
#define TILTSET_UD_LARGE_THRESHOLD	(0x24)		//0x24 raw sensor units = 642 milli-g's (40 degrees)
#define TILTSET_LR_THRESHOLD		(0x1C)		//0x1C raw sensor units = 500 milli-g's (30 degrees)

// Delays between "button presses"
#define TILTSET_REPEAT_DELAY_FAST	(200u)
#define TILTSET_REPEAT_DELAY_SLOW	(400u)

// Prototypes Section
// ******************************************************************************
struct tiltset {
    u8	active;		// Whether running or not; won't run when Accelerometer Module is running
    u8	armed;		// For storing whether the watch was tipped left or right
    
    u16	delay;
};
extern struct tiltset sTiltset;

#endif