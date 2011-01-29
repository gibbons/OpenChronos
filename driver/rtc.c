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
// Real Time Clock module routines.
// *************************************************************************************************

// *************************************************************************************************
// Include section
//gibbons TODO: update include section as necessary
// system
#include "project.h"

// driver
#include "timer.h"
#include "ports.h"
#include "buzzer.h"
#include "vti_ps.h"
#ifdef FEATURE_PROVIDE_ACCEL
#include "vti_as.h"
#endif
#include "display.h"

// logic
#include "clock.h"
#include "battery.h"
#include "stopwatch.h"
#include "alarm.h"
#include "altitude.h"
#include "display.h"
#include "rfsimpliciti.h"
#include "simpliciti.h"
#ifdef FEATURE_PROVIDE_ACCEL
#include "acceleration.h"
#endif

#ifdef CONFIG_PROUT
#include "prout.h"
#endif

#ifdef CONFIG_VARIO
#include "vario.h"
#endif

//pfs
#ifndef ELIMINATE_BLUEROBIN
#include "bluerobin.h"
#endif

#include "temperature.h"

#ifdef CONFIG_EGGTIMER
#include "eggtimer.h"
#endif

#ifdef CONFIG_SIDEREAL
#include "sidereal.h"
#endif

#ifdef CONFIG_STRENGTH
#include "strength.h"
#endif

// *************************************************************************************************
// Prototypes section
//gibbons TODO: update prototypes

//void Timer0_Init(void);
//void (*fptr_Timer0_A3_function)(void);
 

// *************************************************************************************************
// Defines section
//gibbons TODO: update this section as needed

// *************************************************************************************************
// Global Variable section
//gibbons TODO: update this section as needed
//struct timer sTimer;

// *************************************************************************************************
// Extern section
//gibbons TODO: update this section as needed
//extern void BRRX_TimerTask_v(void);
//extern void to_lpm(void);


//gibbons TODO: update documentation
// *************************************************************************************************
// @fn          RTC_A_ISR
// @brief       IRQ handler for RTC Module
// @param       none
// @return      none
// *************************************************************************************************
//pfs 
#ifdef __GNUC__  
#include <signal.h>
interrupt (RTC_VECTOR) RTC_A_ISR(void) //gibbons TODO: changed to RTC 1-sec ISR
#else
#pragma vector = RTC_VECTOR
__interrupt void RTC_A_ISR(void)
#endif
{
	sTime.second++;
	if (sTime.second == 60) sTime.second = 0;
	
	static u8 button_lock_counter = 0; //gibbons TODO: need to put these just in the 1-sec interrupt section?
	static u8 button_beep_counter = 0;
	switch (RTCIV) {
		case RTC_RT0PSIFG: // Interval timer (16384Hz - 128Hz interrupts (binary powers) )
			break;
			
		case RTC_RT1PSIFG: // Interval timer (64Hz - 0.5Hz interrupts (binary powers) )
			break;
			
		case RTC_RTCRDYIFG: // RTC registers ready and safe to read (Use this for 1-sec update)
			
			
			// Add 1 second to stored (global) time, update sTime.drawFlag appropriately
			clock_tick();
			
			// Set clock update flag
			display.flag.update_time = 1;
			
			// While SimpliciTI stack operates or BlueRobin searches, freeze system state
			//pfs
#ifdef ELIMINATE_BLUEROBIN
			if (is_rf())
#else
			if (is_rf() || is_bluerobin_searching()) 
#endif
			{
				// SimpliciTI automatic timeout
				if (sRFsmpl.timeout == 0) 
				{
					simpliciti_flag |= SIMPLICITI_TRIGGER_STOP;
				}
				else
				{
					sRFsmpl.timeout--;
				}
				
				// Exit from LPM3 on RETI
				_BIC_SR_IRQ(LPM3_bits);     
				return;
			}
			
			// -------------------------------------------------------------------
			// Service modules that require 1/min processing
			if (sTime.drawFlag >= 2) 
			{
				// Measure battery voltage to keep track of remaining battery life
				request.flag.voltage_measurement = 1;
				
				// Check if alarm needs to be turned on
				//check_alarm(); //gibbons TODO: remove
			}

			// -------------------------------------------------------------------
			// Service active modules that require 1/s processing
			
			// Generate alarm signal
			if (sAlarm.state == ALARM_ON) 
			{
				// Decrement alarm duration counter
				if (sAlarm.duration-- > 0)
				{
					request.flag.buzzer = 1;
				}
				else
				{
					sAlarm.duration = ALARM_ON_DURATION;
					stop_alarm();
				}
			}

#ifdef CONFIG_PROUT
			if (is_prout()) prout_tick();
#endif

#ifdef CONFIG_VARIO
			if(is_vario()) vario_tick();
#endif

#ifdef CONFIG_STRENGTH
			// One more second gone by.
			if(is_strength())
			{
				strength_tick();
			}            
#endif

			// Do a temperature measurement each second while menu item is active
			if (is_temp_measurement()) request.flag.temperature_measurement = 1;
			
			// Do a pressure measurement each second while menu item is active
#ifdef CONFIG_ALTITUDE
			if (is_altitude_measurement()) 
			{
				// Countdown altitude measurement timeout while menu item is active
				sAlt.timeout--;

				// Stop measurement when timeout has elapsed
				if (sAlt.timeout == 0)	
				{
					stop_altitude_measurement();
					// Show ---- m/ft
					display_chars(LCD_SEG_L1_3_0, (u8*)"----", SEG_ON);
					// Clear up/down arrow
					display_symbol(LCD_SYMB_ARROW_UP, SEG_OFF);
					display_symbol(LCD_SYMB_ARROW_DOWN, SEG_OFF);
				}
				
				// In case we missed the IRQ due to debouncing, get data now
				if ((PS_INT_IN & PS_INT_PIN) == PS_INT_PIN) request.flag.altitude_measurement = 1;
			}	
#endif

#ifdef FEATURE_PROVIDE_ACCEL
			// Count down timeout
			if (is_acceleration_measurement()) 
			{
				// Countdown acceleration measurement timeout 
				sAccel.timeout--;

				// Stop measurement when timeout has elapsed
				if (sAccel.timeout == 0) as_stop();	
				
				// If DRDY is (still) high, request data again
				if ((AS_INT_IN & AS_INT_PIN) == AS_INT_PIN) request.flag.acceleration_measurement = 1; 
			}	
#endif

			//pfs
#ifndef ELIMINATE_BLUEROBIN
			// If BlueRobin transmitter is connected, get data from API
			if (is_bluerobin()) get_bluerobin_data();
#endif
			
			// If battery is low, decrement display counter
			if (sys.flag.low_battery)
			{
				if (sBatt.lobatt_display-- == 0) 
				{
					message.flag.prepare = 1;
					message.flag.type_lobatt = 1;
					sBatt.lobatt_display = BATTERY_LOW_MESSAGE_CYCLE;
				}
			}
			
			// If a message has to be displayed, set display flag
			if (message.all_flags)
			{
				if (message.flag.prepare)
				{
					message.flag.prepare = 0;
					message.flag.show    = 1;
				}
				else if (message.flag.erase) // message cycle is over, so erase it
				{
					message.flag.erase       = 0;
					message.flag.block_line1 = 0;
					message.flag.block_line2 = 0;
					display.flag.full_update = 1;
				}	
			}
			
			// -------------------------------------------------------------------
			// Check idle timeout, set timeout flag
			if (sys.flag.idle_timeout_enabled)
			{
				if (--sTime.last_activity == 0) sys.flag.idle_timeout = 1; //setFlag(sysFlag_g, SYS_TIMEOUT_IDLE);
			} //gibbons TODO: fixed this?
			
			// -------------------------------------------------------------------
			// Detect continuous button high states

			if (BUTTON_STAR_IS_PRESSED && BUTTON_UP_IS_PRESSED)
			{
				if (button_beep_counter++ > LEFT_BUTTON_LONG_TIME)
				{
					// Toggle no_beep buttons flag
					sys.flag.no_beep = ~sys.flag.no_beep;
			
					// Show "beep / nobeep" message synchronously with next second tick
					message.flag.prepare = 1;
					if (sys.flag.no_beep)	message.flag.type_no_beep_on   = 1;
					else					message.flag.type_no_beep_off  = 1;
					
					// Reset button beep counter
					button_beep_counter = 0;
				}
			} else if (BUTTON_NUM_IS_PRESSED && BUTTON_DOWN_IS_PRESSED) // Trying to lock/unlock buttons?
			{
				if (button_lock_counter++ > LEFT_BUTTON_LONG_TIME)
				{
					// Toggle lock / unlock buttons flag
					sys.flag.lock_buttons = ~sys.flag.lock_buttons;
			
					// Show "buttons are locked/unlocked" message synchronously with next second tick
					message.flag.prepare = 1;
					if (sys.flag.lock_buttons)	message.flag.type_locked   = 1;
					else						message.flag.type_unlocked = 1;
					
					// Reset button lock counter
					button_lock_counter = 0;
				}
			}
			else // Trying to create a long button press?
			{
				// Reset button lock counter
				button_lock_counter = 0;
				
				if (BUTTON_STAR_IS_PRESSED) 	
				{
					sButton.star_timeout++;
					
					// Check if button was held low for some seconds
					if (sButton.star_timeout > LEFT_BUTTON_LONG_TIME) 
					{
						button.flag.star_long = 1;
						sButton.star_timeout = 0;
					}
				}
				else
				{
					sButton.star_timeout = 0;
				}
			
				if (BUTTON_NUM_IS_PRESSED) 	
				{
					sButton.num_timeout++;
				
					// Check if button was held low for some seconds
					if (sButton.num_timeout > LEFT_BUTTON_LONG_TIME) 
					{
						button.flag.num_long = 1;
						sButton.num_timeout = 0;
					}
				}
				else
				{
					sButton.num_timeout = 0;
				}
			}
			
			
			// Exit from LPM3 on RETI
			_BIC_SR_IRQ(LPM3_bits); //gibbons TODO: move this to other interrupts from the RTC?
			
			break;
			
		case RTC_RTCTEVIFG: // Interval alarm event (min or hour changed, or rollover to midnight or noon) (choose one)
			// gibbons TODO: Add in ability to have minute, hour, noon, or midnight beep
			break;
			
		case RTC_RTCAIFG: // User-configurable alarm event
			// gibbons TODO: Add alarm code here
			
			// Indicate that alarm is on
			sAlarm.state = ALARM_ON;
			
			break;
	}

	//gibbons: TODO: Remove this old code? (Associated with Timer0_A0 1-sec ISR)
	// Disable IE 
	//TA0CCTL0 &= ~CCIE;
	// Reset IRQ flag  
	//TA0CCTL0 &= ~CCIFG;  
	// Add 1 sec to TACCR0 register (IRQ will be asserted at 0x7FFF and 0xFFFF = 1 sec intervals)
	//TA0CCR0 += 32768; //gibbons TODO: Should this perhaps be (32768 - 1) ?
	// Enable IE 
	//TA0CCTL0 |= CCIE;
}