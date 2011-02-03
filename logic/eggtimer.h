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
//
// Eggtimer is currently just a copy of stopwatch.
// I'm going to be turning it into a count down timer.
//
// TO DO:
//    Make sure it doesn't interfere with stopwatch 
//      (timer.c causing weird things especially relating to alt mode)
//      (do NOT run both eggtimer and stopwatch at the same time)
//      (this is because Stopwatch and Eggtimer both use the same Capture & Compare Register)
//    Figure out why I have to use the stopwatch display update flag.
//    Set it so stopwatch and eggtimer blink their respective icons when selected
//      this prevents confusion as to which is currently selected.
//    Change beeping so user can stop it at will, instead of having to wait.
// *************************************************************************************************

#ifndef eggtimer_H_
#define eggtimer_H_

// *************************************************************************************************
// Include section
#include <project.h>

#ifdef CONFIG_EGGTIMER

// *************************************************************************************************
// Prototypes section
extern void init_eggtimer(void);
extern void start_eggtimer(void);
extern void stop_eggtimer(void);
extern void reset_eggtimer(void);
//extern u8 is_eggtimer(void); // gibbons TODO: remove this
extern void eggtimer_tick(void);
extern void stop_eggtimer_alarm(void);
//extern void update_eggtimer_timer(void); // gibbons TODO: remove this
extern void mx_eggtimer(u8 line);
extern void sx_eggtimer(u8 line);
extern void nx_eggtimer(u8 line);
extern void display_eggtimer(u8 line, u8 update);
extern void set_eggtimer(void);


// *************************************************************************************************
// Defines section
#define EGGTIMER_STOP				(0u)
#define EGGTIMER_RUN				(1u)
#define EGGTIMER_ALARM				(2u)

#define EGGTIMER_ALARM_DURATION			(10u)

// *************************************************************************************************
// Global Variable section
struct eggtimer
{
        //NOTE: u8 means unsigned char
	u8	state;
	u8	drawFlag;
	u8	menu_active; // Flag to determine if the eggtimer's currently active, or running in the background
	
	// Values that are decremented each second
	u8	hours;
	u8	minutes;
	u8	seconds;
	
	// Values to default to, after timer runs out and is cleared
	u8	default_hours;
	u8	default_minutes;
	u8	default_seconds;
	
	u8	duration; //Number of times to request buzzer double-beep when time is up
};
extern struct eggtimer sEggtimer;


// *************************************************************************************************
// Extern section

#endif // CONFIG_EGGTIMER

#endif /*eggtimer_H_*/
