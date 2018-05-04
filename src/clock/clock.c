/*****************************************************************************
 * clock.c: Ouptut modules synchronisation clock
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
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include "clock.h"
#include "clock_internal.h"

#ifndef LIBVLC_CLOCK_H
#error TA CHATTE TA CHATTE TA CHATTE SALOPE
#endif

struct vlc_clock_main_t
{
    vlc_mutex_t lock;
    vlc_cond_t cond;

    vlc_clock_t * master;

    vlc_clock_t ** slaves;
    int nslaves;

    /* Linear function
     * system = pts * coeff / rate + offset
     */
    average_t coeff_avg;
    float coeff; /* TODO use a moving average */
    float rate;
    mtime_t offset;

    mtime_t pause_date;

    mtime_t jitter; /* TODO */
    bool abort;
};

struct vlc_clock_t
{
    mtime_t (*update)(vlc_clock_t * clock, mtime_t timestamp,
                      mtime_t system_now, float rate);
    void (*reset)(vlc_clock_t * clock);
    void (*pause)(vlc_clock_t * clock, bool paused, mtime_t now);
    int (*wait)(vlc_clock_t * clock, mtime_t pts);
    void (*set_jitter)(vlc_clock_t * clock, mtime_t delay, int cr_avg);

    vlc_clock_main_t * owner;
    float rate;
    clock_point_t last;
};

static mtime_t main_system_to_stream(vlc_clock_main_t * main_clock,
                                       mtime_t system)
{
    if (unlikely(main_clock->offset == VLC_TS_INVALID))
        return VLC_TS_INVALID;
    return (mtime_t) ((system - main_clock->offset) * main_clock->rate / main_clock->coeff);
}

static mtime_t main_stream_to_system(vlc_clock_main_t * main_clock,
                                       mtime_t pts)
{
    if (unlikely(main_clock->offset == VLC_TS_INVALID))
        return VLC_TS_INVALID;
    return (mtime_t) (pts * main_clock->coeff / main_clock->rate + main_clock->offset);
}


static mtime_t vlc_clock_master_update(vlc_clock_t * clock, mtime_t pts,
                                       mtime_t system_now, float rate)
{
    vlc_clock_main_t * main_clock = clock->owner;

    vlc_mutex_lock(&main_clock->lock);
    if (rate != main_clock->rate)
    {
        main_clock->rate = rate;
        vlc_cond_broadcast(&main_clock->cond);
    }
    if (unlikely(pts == VLC_TS_INVALID || system_now == VLC_TS_INVALID))
    {
        vlc_mutex_unlock(&main_clock->lock);
        return VLC_TS_INVALID;
    }

    if (main_clock->offset != VLC_TS_INVALID && pts != clock->last.stream)
    {
        /* We have a reference so we can update coeff */
        float instant_coeff =
            ((float) (system_now - clock->last.system))/(pts - clock->last.stream);
        AvgUpdate(&main_clock->coeff_avg, instant_coeff);
        main_clock->coeff = AvgGet(&main_clock->coeff_avg);
        /* TODO handle rate change?*/
    }
    main_clock->offset = system_now - pts * main_clock->coeff / rate;

    vlc_mutex_unlock(&main_clock->lock);
    return 0;
}

static void vlc_clock_master_reset(vlc_clock_t * clock)
{
    vlc_clock_main_t * main_clock = clock->owner;

    vlc_mutex_lock(&main_clock->lock);
    main_clock->coeff = 1.0f;
    /* TODO Reset coeff avg */
    main_clock->offset = VLC_TS_INVALID;
    main_clock->rate = 1.0f;
    vlc_cond_broadcast(&main_clock->cond);
    vlc_mutex_unlock(&main_clock->lock);
}

static void vlc_clock_master_pause(vlc_clock_t * clock, bool paused, mtime_t now)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);

    if (paused)
        main_clock->pause_date = now;
    else
        main_clock->offset += now - main_clock->pause_date;

    vlc_mutex_unlock(&main_clock->lock);
}

static int vlc_clock_get_rate(vlc_clock_t * clock)
{
    float rate;
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);

    rate = main_clock->rate;

    vlc_mutex_unlock(&main_clock->lock);

    return rate;
}

static int vlc_clock_master_wait(vlc_clock_t * clock, mtime_t pts)
{
    VLC_UNUSED(clock);
    VLC_UNUSED(pts);
    return 0; /* FIXME unused? */
}

static mtime_t vlc_clock_to_system(vlc_clock_t * clock, mtime_t timestamp)
{
    vlc_clock_main_t * main_clock = clock->owner;
    mtime_t system;
    vlc_mutex_lock(&main_clock->lock);
    system = main_stream_to_system(main_clock, timestamp);
    vlc_mutex_unlock(&main_clock->lock);
    return system;
}

static mtime_t vlc_clock_to_stream(vlc_clock_t * clock, mtime_t system)
{
    vlc_clock_main_t * main_clock = clock->owner;
    mtime_t pts;

    vlc_mutex_lock(&main_clock->lock);
    pts = main_system_to_stream(main_clock, system);
    vlc_mutex_unlock(&main_clock->lock);
    return pts;
}

static void vlc_clock_master_set_jitter(vlc_clock_t * clock, mtime_t delay, int cr_avg)
{
    VLC_UNUSED(cr_avg);
    vlc_clock_main_t * main_clock = clock->owner;

    vlc_mutex_lock(&main_clock->lock);
    main_clock->jitter = delay;
    vlc_mutex_unlock(&main_clock->lock);
}

static mtime_t vlc_clock_get_jitter(vlc_clock_t * clock)
{
    vlc_clock_main_t * main_clock = clock->owner;
    mtime_t jitter;

    vlc_mutex_lock(&main_clock->lock);
    jitter = main_clock->jitter;
    vlc_mutex_unlock(&main_clock->lock);
    return jitter;
}

static mtime_t vlc_clock_slave_update(vlc_clock_t * clock, mtime_t timestamp,
                                      mtime_t system_now, float rate)
{
    mtime_t computed = vlc_clock_to_system(clock, timestamp);
    if (computed == VLC_TS_INVALID)
        computed =
            (timestamp - clock->last.stream) / rate + clock->last.system;
    return (computed - system_now);
}

static void vlc_clock_slave_reset(vlc_clock_t * clock)
{
    VLC_UNUSED(clock);
}

static void vlc_clock_slave_pause(vlc_clock_t * clock, bool paused, mtime_t now)
{
    VLC_UNUSED(clock);
    VLC_UNUSED(paused);
    VLC_UNUSED(now);
}

static int vlc_clock_slave_wait(vlc_clock_t * clock, mtime_t pts)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    while (!main_clock->abort)
    {
        mtime_t deadline = main_stream_to_system(main_clock, pts);
        /* Fallback to clock monotonic when there is no main_clock */
        if (unlikely(deadline == VLC_TS_INVALID))
        {
            if (unlikely(clock->last.stream == VLC_TS_INVALID ||
                         clock->last.system == VLC_TS_INVALID))
                return 0;

            deadline =
                (pts - clock->last.stream) / clock->rate + clock->last.system;
        }
        if (vlc_cond_timedwait(&main_clock->cond, &main_clock->lock, deadline))
            return 0;
    }
    return 1;
}

static void vlc_clock_slave_set_jitter(vlc_clock_t * clock, mtime_t delay, int cr_avg)
{
    VLC_UNUSED(clock);
    VLC_UNUSED(delay);
    VLC_UNUSED(cr_avg);
}


vlc_clock_main_t * vlc_clock_main_New(void)
{
    vlc_clock_main_t * main_clock = malloc(sizeof(vlc_clock_main_t));

    if (main_clock == NULL)
        return NULL;

    vlc_mutex_init(&main_clock->lock);
    vlc_cond_init(&main_clock->cond);
    main_clock->master = NULL;

    TAB_INIT(main_clock->nslaves, main_clock->slaves);

    main_clock->rate = 1.0f;
    main_clock->coeff = 1.0f;
    main_clock->offset = VLC_TS_INVALID;

    main_clock->pause_date = VLC_TS_INVALID;
    main_clock->jitter = VLC_TS_INVALID;
    main_clock->abort = false;

    AvgInit(&main_clock->coeff_avg, 10);

    return main_clock;
}

void vlc_clock_main_Abort(vlc_clock_main_t * main_clock)
{
    vlc_mutex_lock(&main_clock->lock);
    main_clock->abort = true;
    vlc_cond_broadcast(&main_clock->cond);

    vlc_mutex_unlock(&main_clock->lock);
}

void vlc_clock_main_Delete(vlc_clock_main_t * main_clock)
{
    while (main_clock->nslaves != 0)
        vlc_clock_Delete(main_clock->slaves[0]);

    if (main_clock->master != NULL)
        vlc_clock_Delete(main_clock->master);

    TAB_CLEAN(main_clock->nslaves, main_clock->slaves);
    vlc_mutex_destroy(&main_clock->lock);
    free(main_clock);
}

mtime_t vlc_clock_Update(vlc_clock_t * clock, mtime_t timestamp,
                         mtime_t system_now, float rate)
{
    clock->rate = rate;
    if (timestamp != VLC_TS_INVALID && system_now != VLC_TS_INVALID)
        clock->last = clock_point_Create(timestamp, system_now);
    return clock->update(clock, timestamp, system_now, rate);
}

void vlc_clock_Reset(vlc_clock_t * clock)
{
    clock->last = clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);
    clock->reset(clock);
}

void vlc_clock_ChangePause(vlc_clock_t * clock, bool paused,
                              mtime_t system_now)
{
    clock->pause(clock, paused, system_now);
}

int vlc_clock_GetRate(vlc_clock_t * clock)
{
    return vlc_clock_get_rate(clock);
}

int vlc_clock_Wait(vlc_clock_t * clock, mtime_t pts)
{
    return clock->wait(clock, pts);
}


mtime_t vlc_clock_ConvertToSystem(vlc_clock_t * clock, mtime_t timestamp)
{
    return vlc_clock_to_system(clock, timestamp);
}

mtime_t vlc_clock_ConvertToStream(vlc_clock_t * clock, mtime_t system)
{
    return vlc_clock_to_stream(clock, system);
}

void vlc_clock_SetJitter(vlc_clock_t * clock, mtime_t delay, int cr_avg)
{
    clock->set_jitter(clock, delay, cr_avg);
}

mtime_t vlc_clock_GetJitter(vlc_clock_t * clock)
{
    return vlc_clock_get_jitter(clock);
}

static void vlc_clock_set_master_cbk(vlc_clock_t * clk)
{

    clk->update = vlc_clock_master_update;
    clk->reset = vlc_clock_master_reset;
    clk->pause = vlc_clock_master_pause;
    clk->wait = vlc_clock_master_wait;
    clk->set_jitter = vlc_clock_master_set_jitter;
}

static void vlc_clock_set_slave_cbk(vlc_clock_t * clk)
{
    clk->update = vlc_clock_slave_update;
    clk->reset = vlc_clock_slave_reset;
    clk->pause = vlc_clock_slave_pause;
    clk->wait = vlc_clock_slave_wait;
    clk->set_jitter = vlc_clock_slave_set_jitter;
}

vlc_clock_t * vlc_clock_NewMaster(vlc_clock_main_t * main_clock)
{
    vlc_clock_t * clk = malloc(sizeof(vlc_clock_t));
    if (clk == NULL)
        return NULL;

    clk->owner = main_clock;

    vlc_mutex_lock(&main_clock->lock);
    if (main_clock->master != NULL)
    {
        vlc_clock_master_reset(main_clock->master);
        vlc_clock_set_slave_cbk(main_clock->master);
        TAB_APPEND(main_clock->nslaves, main_clock->slaves, main_clock->master);
    }
    vlc_clock_set_master_cbk(clk);
    main_clock->master = clk;
    vlc_mutex_unlock(&main_clock->lock);

    return clk;
}

vlc_clock_t * vlc_clock_NewSlave(vlc_clock_main_t * main_clock)
{
    vlc_clock_t * clk = malloc(sizeof(vlc_clock_t));
    if (clk == NULL)
        return NULL;

    clk->owner = main_clock;
    clk->last = clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);

    vlc_mutex_lock(&main_clock->lock);
    clk->rate = main_clock->rate;
    vlc_clock_set_slave_cbk(clk);
    TAB_APPEND(main_clock->nslaves, main_clock->slaves, clk);
    vlc_mutex_unlock(&main_clock->lock);

    return clk;
}

void vlc_clock_SetMaster(vlc_clock_main_t * main_clock, vlc_clock_t * clk)
{
    vlc_mutex_lock(&main_clock->lock);
    TAB_REMOVE(main_clock->nslaves, main_clock->slaves, clk);
    if (main_clock->master != NULL)
    {
        vlc_clock_master_reset(main_clock->master);
        vlc_clock_set_slave_cbk(main_clock->master);
        TAB_APPEND(main_clock->nslaves, main_clock->slaves, main_clock->master);
    }
    vlc_clock_set_master_cbk(clk);
    main_clock->master = clk;
    vlc_mutex_unlock(&main_clock->lock);
}


void vlc_clock_Delete(vlc_clock_t * clock)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    if (clock == main_clock->master)
    {
        vlc_clock_master_reset(clock);
        main_clock->master = NULL;
    }
    else
        TAB_REMOVE(main_clock->nslaves, main_clock->slaves, clock);
    vlc_mutex_unlock(&main_clock->lock);
    free(clock);
}


