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
typedef struct vlc_clock_master_t vlc_clock_master_t;
typedef struct vlc_clock_t vlc_clock_t;

/**
 * This function creates a new slave vlc_clock_t.
 * You must use vlc_clock_Delete to free it.
 */
vlc_clock_t * vlc_clock_NewMaster(vlc_clock_master_t * master);

/**
 * This function creates a new slave vlc_clock_t.
 * You must use vlc_clock_Delete to free it.
 */
vlc_clock_t * vlc_clock_NewSlave(vlc_clock_master_t * master);

/**
 * This function free the resources allocated by vlc_clock_New
 */
void vlc_clock_Delete(vlc_clock_t * clock);

/**
 * This function will update the clock drift and returns the drift
 */
mtime_t vlc_clock_Update(vlc_clock_t * clock, mtime_t timestamp, mtime_t system_now);

/**
 * This function resets the clock drift
 */
void vlc_clock_Reset(vlc_clock_t * clock);

/**
 * This functions allows changing the actual reading speed.
 */
void vlc_clock_ChangeRate(vlc_clock_t * clock, int rate);

/**
 * This function allows changing the pause status.
 */
void vlc_clock_ChangePause(vlc_clock_t * clock, bool paused,
                              mtime_t sysem_now);

/**
 * This function returns the current rate.
 */
int vlc_clock_GetRate(vlc_clock_t * clock);

/**
 * Wait for a timestamp expressed in stream time
 */
int vlc_clock_Wait(vlc_clock_t * clock, mtime_t pts);

/**
 * Abort all the pending vlc_clock_Wait
 */
void vlc_clock_Abort(vlc_clock_t * clock);

/**
 * This function converts a timestamp from stream to system
 */
mtime_t vlc_clock_ConvertToSystem(vlc_clock_t * clock, mtime_t timestamp);

/**
 * This function converts a timestamp from system to stream time
 */
mtime_t vlc_clock_ConvertToStream(vlc_clock_t * clock, mtime_t system);

/**
 * This function sets the synchronization delay ie the maximum delay before the synchro
 * is considered to kick in
 */
void vlc_clock_SetJitter(vlc_clock_t * clock, mtime_t delay, int cr_avg);

/**
 * This function retrieves the synchronization delay
 */
mtime_t vlc_clock_GetJitter(vlc_clock_t * clock);
