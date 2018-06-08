/*****************************************************************************
 * clock.h: Ouptut modules synchronisation clock
 *****************************************************************************
 * Copyright (C) 2018 VLC authors and VideoLAN
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifndef LIBVLC_CLOCK_H
#define LIBVLC_CLOCK_H

enum vlc_clock_master_source
{
    VLC_CLOCK_MASTER_AUDIO = 0,
    VLC_CLOCK_MASTER_VIDEO,
    VLC_CLOCK_MASTER_MONOTONIC,
    VLC_CLOCK_MASTER_DEFAULT = VLC_CLOCK_MASTER_AUDIO,
};

typedef struct vlc_clock_main_t vlc_clock_main_t;
typedef struct vlc_clock_t vlc_clock_t;

/**
 * This function creates the vlc_clock_main_t of the program
 */
vlc_clock_main_t * vlc_clock_main_New(void);

/**
 * Destroy the clock main
 */
void vlc_clock_main_Delete(vlc_clock_main_t * main);

/**
 * Abort all the pending vlc_clock_Wait
 */
void vlc_clock_main_Abort(vlc_clock_main_t * main);

/**
 * Reset the vlc_clock_main_t
 */
void vlc_clock_main_Reset(vlc_clock_main_t * main);

/**
 * This function set the allocated interface as the master making the current
 * master if any a slave.
 */
void vlc_clock_SetMaster(vlc_clock_main_t * main, vlc_clock_t * new_master);

/**
 * This function creates a new slave vlc_clock_t interface.
 * You must use vlc_clock_Delete to free it.
 */
vlc_clock_t * vlc_clock_NewMaster(vlc_clock_main_t * main);

/**
 * This function creates a new slave vlc_clock_t interface.
 * You must use vlc_clock_Delete to free it.
 */
vlc_clock_t * vlc_clock_NewSlave(vlc_clock_main_t * main);

/**
 * This function free the resources allocated by vlc_clock_New
 */
void vlc_clock_Delete(vlc_clock_t * clock);

/**
 * This function will update the clock drift and returns the drift
 */
vlc_tick_t vlc_clock_Update(vlc_clock_t * clock, vlc_tick_t system_now,
                         vlc_tick_t pts, float rate);

/**
 * This function resets the clock drift
 */
void vlc_clock_Reset(vlc_clock_t * clock);

/**
 * This function allows changing the pause status.
 */
void vlc_clock_ChangePause(vlc_clock_t * clock, vlc_tick_t system_now, bool paused);

/**
 * This functions change the clock delay.
 * It returns the amount of time the clock owner need to wait in order to reach
 * the time introduced by the new positive delay.
 */
vlc_tick_t vlc_clock_SetDelay(vlc_clock_t * clock, vlc_tick_t pts_delay);

/**
 * This function returns the current rate.
 */
float vlc_clock_GetRate(vlc_clock_t * clock);

/**
 * Wait for a timestamp expressed in stream time
 */
int vlc_clock_Wait(vlc_clock_t * clock, vlc_tick_t pts, vlc_tick_t max_duration);

/**
 * This function converts a timestamp from stream to system
 */
vlc_tick_t vlc_clock_ConvertToSystem(vlc_clock_t * clock, vlc_tick_t system_now,
                                  vlc_tick_t pts);

void vlc_clock_ConvertArrayToSystem(vlc_clock_t * clock, vlc_tick_t system_now,
                                    vlc_tick_t *pts_array, size_t pts_count);

/**
 * This function converts a timestamp from system to stream time
 */
vlc_tick_t vlc_clock_ConvertToStream(vlc_clock_t * clock, vlc_tick_t system);

/**
 * This function sets the dejitter delay to absorb the clock jitter
 * also used as the ma1ximum delay before the synchro is considered to kick in
 */
void vlc_clock_SetDejitter(vlc_clock_t * clock, vlc_tick_t delay, int cr_avg);

/**
 * This function retrieves the synchronization delay
 */
vlc_tick_t vlc_clock_GetDejitter(vlc_clock_t * clock);

#endif /*LIBVLC_CLOCK_H*/
