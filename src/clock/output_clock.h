/*****************************************************************************
 * output_clock.h: Ouptut modules synchronisation clock
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

typedef struct output_clock_t output_clock_t;

/**/
typedef enum
{
    OUTCLK_SRC_AOUT = 0x01, /* In stream base */
    OUTCLK_SRC_VOUT = 0x02, /* In stream base */
    OUTCLK_SRC_PCR = 0x04, /* In stream base */
    OUTCLK_SRC_EXTERNAL = 0x08, /* For SDI in external base */
} output_clock_source_t;

typedef enum
{
    OUTCLK_BASE_SYSTEM,
    OUTCLK_BASE_STREAM,
    OUTCLK_BASE_EXTERNAL,
} output_clock_base_t;


#define OUTCLK_SRC_ALL \
    (OUTCLK_SRC_AOUT | OUTCLK_SRC_VOUT | OUTCLK_SRC_PCR | OUTCLK_SRC_EXTERNAL)

/**
 * This function creates a new output_clock_t.
 * You must use output_clock_Delete to free it.
 */
output_clock_t * output_clock_New(int rate);

/**
 * This function free the resources allocated by output_clock_New
 */
void output_clock_Delete(output_clock_t * clock);

/**
 * This function will store, depending on the source the clock
 * OUTCLK_BASE_SYSTEM cannot be passed as a base.
 */
int output_clock_Update(output_clock_t * clock, vlc_object_t * p_log,
                        ouptut_clock_src_t source, mtime_t timestamp,
                        mtime_t system);

/**
 * This function resets the clock drift by type of source
 */
void output_clock_Reset(output_clock_t * clock, int source);

/**
 * This functions allows changing the actual reading speed.
 */
void output_clock_ChangeRate(output_clock_t * clock, int rate);

/**
 * This function allows changing the pause status.
 */
void output_clock_ChangePause(output_clock_t * clock, bool paused);

/**
 * This function returns the current rate.
 */
int output_clock_GetRate(output_clock_t * clock);

/**
 * Wait for a timestamp expressed in the specified base
 */
int output_clock_Wait(output_clock_t * clock, mtime_t pts, output_clock_base_t base);

/**
 * Abort all the pending output_clock_Wait
 */
void output_clock_Abort(output_clock_t * clock);

/**
 * This function convert a timestamp from a base to another
 */
mtime_t output_clock_Convert(output_clock_t * clock, mtime_t timestamp,
                             output_clock_base_t src, output_clock_base_t dst);

/*FIXME names are confusing */
/**
 * This function sets the synchronization delay ie the maximum delay before the synchro
 * is considered to kick in
 */
void output_clock_SetJitter(output_clock_t * clock, mtime_t delay, int cr_avg);

/**
 * This function retrieves the synchronization delay
 */
mtime_t output_clock_GetJitter(output_clock_t * clock);
