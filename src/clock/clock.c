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

struct vlc_clock_main_t
{
    vlc_mutex_t lock;
    vlc_cond_t cond;

    vlc_clock_t * master;

    vlc_clock_t ** slaves;
    size_t nslaves;

    /* Linear function
     * system = pts * coeff / rate + offset
     */
    double coeff; /* TODO use a moving average */
    double rate;
    mtime_t offset;

    mtime_t pause_date;

    mtime_t jitter; /* TODO */
    bool abort;
};

struct vlc_clock_t
{
    mtime_t (*update)(vlc_clock_t * clock, mtime_t timestamp,
                      mtime_t system_now, double rate);
    void (*reset)(vlc_clock_t * clock);
    void (*pause)(vlc_clock_t * clock, bool paused, mtime_t now);
    int (*wait)(vlc_clock_t * clock, mtime_t pts);
    void (*set_jitter)(vlc_clock_t * clock, mtime_t delay, int cr_avg);

    vlc_clock_main_t * owner;
    double rate;
    clock_point_t last;
};

static mtime_t main_system_to_stream(vlc_clock_main_t * main,
                                       mtime_t system)
{
    if (unlikely(main->offset == VLC_TS_INVALID))
        return VLC_TS_INVALID;
    return (mtime_t) ((system - main->offset) * main->rate / main->coeff);
}

static mtime_t main_stream_to_system(vlc_clock_main_t * main,
                                       mtime_t pts)
{
    if (unlikely(main->offset == VLC_TS_INVALID))
        return VLC_TS_INVALID;
    return (mtime_t) (pts * main->coeff / main->rate + main->offset);
}


static mtime_t vlc_clock_master_update(vlc_clock_t * clock, mtime_t timestamp,
                                       mtime_t system_now, double rate)
{
    vlc_clock_main_t * main = clock->owner;

    vlc_mutex_lock(&main->lock);
    if (rate != main->rate)
    {
        main->rate = rate;
        vlc_cond_broadcast(&main->cond);
    }
    if (unlikely(timestamp == VLC_TS_INVALID || system_now == VLC_TS_INVALID))
    {
        vlc_mutex_unlock(&main->lock);
        return VLC_TS_INVALID;
    }

    if (main->offset != VLC_TS_INVALID)
    {
        /* We have a reference so we can update coeff */
        /* TODO*/
    }
    main->offset = system_now - timestamp * main->coeff / rate;

    vlc_mutex_unlock(&main->lock);
    return 0;
}

static void vlc_clock_master_reset(vlc_clock_t * clock)
{
    vlc_clock_main_t * main = clock->owner;

    vlc_mutex_lock(&main->lock);
    main->coeff = 1.0;
    /* TODO Reset coeff avg */
    main->offset = VLC_TS_INVALID;
    main->rate = 1.0;
    vlc_cond_broadcast(&main->cond);
    vlc_mutex_unlock(&main->lock);
}

static void vlc_clock_master_pause(vlc_clock_t * clock, bool paused, mtime_t now)
{
    vlc_clock_main_t * main = clock->owner;
    vlc_mutex_lock(&main->lock);

    if (paused)
        main->pause_date = now;
    else
        main->offset += now - main->pause_date;

    vlc_mutex_unlock(&main->lock);
}

static int vlc_clock_get_rate(vlc_clock_t * clock)
{
    double rate;
    vlc_clock_main_t * main = clock->owner;
    vlc_mutex_lock(&main->lock);

    rate = main->rate;

    vlc_mutex_unlock(&main->lock);

    return rate;
}

static int vlc_clock_master_wait(vlc_clock_t * clock, mtime_t pts)
{
    return 0; /* FIXME unused? */
}

static mtime_t vlc_clock_to_system(vlc_clock_t * clock, mtime_t timestamp)
{
    vlc_clock_main_t * main = clock->owner;
    mtime_t system;
    vlc_mutex_lock(&main->lock);
    system = main_stream_to_system(main, timestamp);
    vlc_mutex_unlock(&main->lock);
    return system;
}

static mtime_t vlc_clock_to_stream(vlc_clock_t * clock, mtime_t system)
{
    vlc_clock_main_t * main = clock->owner;
    mtime_t pts;

    vlc_mutex_lock(&main->lock);
    pts = main_system_to_stream(main, system);
    vlc_mutex_unlock(&main->lock);
    return pts;
}

static void vlc_clock_master_set_jitter(vlc_clock_t * clock, mtime_t delay, int cr_avg)
{
    vlc_clock_main_t * main = clock->owner;

    vlc_mutex_lock(&main->lock);
    main->jitter = delay;
    vlc_mutex_unlock(&main->lock);
}

static mtime_t vlc_clock_get_jitter(vlc_clock_t * clock)
{
    vlc_clock_main_t * main = clock->owner;
    mtime_t jitter;

    vlc_mutex_lock(&main->lock);
    jitter = main->jitter;
    vlc_mutex_unlock(&main->lock);
    return jitter;
}

static mtime_t vlc_clock_slave_update(vlc_clock_t * clock, mtime_t timestamp,
                                      mtime_t system_now, double rate)
{
    mtime_t computed = vlc_clock_to_system(clock, timestamp);
    if (computed == VLC_TS_INVALID)
        computed =
            (timestamp - clock->last.i_stream) / rate + clock->last.i_system;
    return (computed - system_now);
}

static void vlc_clock_slave_reset(vlc_clock_t * clock)
{}

static void vlc_clock_slave_pause(vlc_clock_t * clock, bool paused, mtime_t now)
{}

static int vlc_clock_slave_wait(vlc_clock_t * clock, mtime_t pts)
{
    vlc_clock_main_t * main = clock->owner;
    vlc_mutex_lock(&main->lock);
    while (!main->abort)
    {
        mtime_t deadline = main_stream_to_system(main, pts);
        /* Fallback to clock monotonic when there is no main */
        if (unlikely(deadline == VLC_TS_INVALID))
        {
            if (unlikely(clock->last.i_stream == VLC_TS_INVALID ||
                         clock->last.i_system == VLC_TS_INVALID))
                return 0;

            deadline =
                (pts - clock->last.i_stream) / clock->rate + clock->last.i_system;
        }
        if (vlc_cond_timedwait(&main->cond, &main->lock, deadline))
            return 0;
    }
    return 1;
}

static void vlc_clock_slave_set_jitter(vlc_clock_t * clock, mtime_t delay, int cr_avg)
{}

vlc_clock_main_t * vlc_clock_main_New()
{
    vlc_clock_main_t * main = malloc(sizeof(vlc_clock_main_t));

    if (main == NULL)
        return NULL;

    vlc_mutex_init(&main->lock);
    vlc_cond_init(&main->cond);
    main->master = NULL;

    TAB_INIT(main->slaves, main->nslaves);

    main->rate = 1.0;
    main->coeff = 1.0;
    main->offset = VLC_TS_INVALID;

    main->pause_date = VLC_TS_INVALID;
    main->jitter = VLC_TS_INVALID;
    main->abort = false;

    return main;
}

void vlc_clock_main_Abort(vlc_clock_main_t * main)
{
    size_t i;
    vlc_mutex_lock(&main->lock);
    main->abort = true;
    vlc_cond_broadcast(&main->cond);

    vlc_mutex_unlock(&main->lock);
}

void vlc_clock_main_Delete(vlc_clock_main_t * main)
{
    while (main->nslaves != 0)
        vlc_clock_Delete(main->slaves[0]);

    if (main->master != NULL)
        vlc_clock_Delete(main->master);

    TAB_CLEAN(main->nslaves, main->slaves);
    vlc_mutex_destroy(&main->lock);
    free(main);
}

mtime_t vlc_clock_Update(vlc_clock_t * clock, mtime_t timestamp,
                         mtime_t system_now, double rate)
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

vlc_clock_t * vlc_clock_NewMaster(vlc_clock_main_t * main)
{
    vlc_clock_t * clk = malloc(sizeof(vlc_clock_t));
    if (clk == NULL)
        return NULL;

    clk->owner = main;

    vlc_mutex_lock(&main->lock);
    if (main->master != NULL)
    {
        vlc_clock_master_reset(main->master);
        vlc_clock_set_slave_cbk(main->master);
        TAB_APPEND(main->nslaves, main->slaves, main->master);
    }
    vlc_clock_set_master_cbk(clk);
    main->master = clk;
    vlc_mutex_unlock(&main->lock);

    return clk;
}

vlc_clock_t * vlc_clock_NewSlave(vlc_clock_main_t * main)
{
    vlc_clock_t * clk = malloc(sizeof(vlc_clock_t));
    if (clk == NULL)
        return NULL;

    clk->owner = main;
    clk->last = clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);

    vlc_mutex_lock(&main->lock);
    clk->rate = main->rate;
    vlc_clock_set_slave_cbk(clk);
    TAB_APPEND(main->nslaves, main->slaves, clk);
    vlc_mutex_unlock(&main->lock);

    return clk;
}

void vlc_clock_SetMaster(vlc_clock_main_t * main, vlc_clock_t * clk)
{
    vlc_mutex_lock(&main->lock);
    TAB_REMOVE(main->nslaves, main->slaves, clk);
    if (main->master != NULL)
    {
        vlc_clock_master_reset(main->master);
        vlc_clock_set_slave_cbk(main->master);
        TAB_APPEND(main->nslaves, main->slaves, main->master);
    }
    vlc_clock_set_master_cbk(clk);
    main->master = clk;
    vlc_mutex_unlock(&main->lock);
}


void vlc_clock_Delete(vlc_clock_t * clock)
{
    vlc_clock_main_t * main = clock->owner;
    vlc_mutex_lock(&main->lock);
    if (clock == main->master)
    {
        vlc_clock_master_reset(clock);
        main->master = NULL;
    }
    else
        TAB_REMOVE(main->nslaves, main->slaves, clock);
    vlc_mutex_unlock(&main->lock);
    free(clock);
}


