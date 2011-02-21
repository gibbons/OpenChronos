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
// Tilt-set Module
// *************************************************************************************************
// 
// This module uses the accelerometer for setting values (say for an alarm). Including it when
// compiling modifies the set_value(...) function in logic/user.c.


// *************************************************************************************************
// Include section
// system
#include "project.h"

// driver
#include "vti_as.h"
#include "timer.h"

//logic
#include "tiltset.h"
#include "menu.h"


// *************************************************************************************************
// Prototypes section


// *************************************************************************************************
// Global Variable section
struct tiltset sTiltset;


// *************************************************************************************************
// Extern section
extern void idle_loop(void); // in ezchronos.c;


// *************************************************************************************************
// @fn          tiltset_start
// @brief       Start the tilt-set module; this starts the accelerometer, so only start when ready
// @param       none
// @return      none
// *************************************************************************************************
void tiltset_start(void)
{
#ifdef CONFIG_ACCEL
    sTiltset.active = 0; //Initialize value
    if (ptrMenu_L1 == &menu_L1_Acceleration) return; // Don't activate if accel. module active
#endif

    as_start();
    sTiltset.active = 1;
}


// *************************************************************************************************
// @fn          tiltset_stop
// @brief       Stop the tilt-set module
// @param       none
// @return      none
// *************************************************************************************************
void tiltset_stop(void)
{
    if (!sTiltset.active) return;
    as_stop();
    sTiltset.active = 0;
}


// *************************************************************************************************
// @fn          tiltset_get_state
// @brief       Send (via the buzzer) an alphanumeric, null ('\0') terminated string
// @param       none
// @return      u8 State	Returns one of the tilt-set module states, as defined in tiltset.h
// *************************************************************************************************
u8 tiltset_get_state()
{
    if (!sTiltset.active) return TILTSET_LEVEL; // Not running; just return neutral state

    // Read xyz data
    u8 xyz_data[3];
    as_get_data(xyz_data);
    
    u8 X_accel = xyz_data[0];
    u8 X_accel_neg = X_accel & 0x80; // Negative flag
    if (X_accel_neg) { // Negative X-acceleration value
	X_accel = (~X_accel) + 1; // Negate to make it positive (2's complement)
    }
    
    u8 Y_accel = xyz_data[1];
    u8 Y_accel_neg = Y_accel & 0x80;
    if (Y_accel_neg) { // Negatie Y-accel value
	Y_accel = (~Y_accel) + 1; // Make value positive
    }
    
    if (Y_accel > TILTSET_LR_THRESHOLD) {
	sTiltset.armed = (Y_accel_neg) ? TILTSET_ARMED_LEFT : TILTSET_ARMED_RIGHT;
    }
    
    if (X_accel > TILTSET_UD_LARGE_THRESHOLD) {
	sTiltset.delay = CONV_MS_TO_TICKS(TILTSET_REPEAT_DELAY_FAST);
	return (X_accel_neg) ? TILTSET_UP_LARGE : TILTSET_DOWN_LARGE;
    }
    else if (X_accel > TILTSET_UD_MIN_THRESHOLD) {
	sTiltset.delay = CONV_MS_TO_TICKS(TILTSET_REPEAT_DELAY_SLOW);
	return (X_accel_neg) ? TILTSET_UP_SMALL : TILTSET_DOWN_SMALL;
    }
    else { // Watch is level (on X axis)
	sTiltset.delay = CONV_MS_TO_TICKS(TILTSET_REPEAT_DELAY_SLOW);
	switch (sTiltset.armed) {
	    case TILTSET_ARMED_LEFT:
		sTiltset.armed = 0;
		return TILTSET_LEFT_TIP;
		break; // Not technically needed; just here in case return statement above is taken out
		
	    case TILTSET_ARMED_RIGHT:
		sTiltset.armed = 0;
		return TILTSET_RIGHT_TIP;
		break; // Not technically needed
		
	    default:
		return TILTSET_LEVEL;
	}   
    }
}


// *************************************************************************************************
// @fn          tiltset_idle_loop
// @brief       Delays (in LPM3) the correct amount to simulate button repeats; for use in
//		set_value(...), in user.c
// @param       none
// @return      none
// *************************************************************************************************
void tiltset_idle_loop(void)
{
    if (!sTiltset.active) {
	idle_loop();
    }
    else {
	Timer0_A4_Delay(sTiltset.delay);
    }
}
